// simple encode/decode test
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "RANS.HPP"

static void panic(const char *fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    fputs("Error: ", stderr);
    vfprintf(stderr, fmt, arg);
    va_end(arg);
    fputs("\n", stderr);

    exit(1);
}

static uint8_t* read_file(char const* filename, size_t* out_size)
{
    FILE* f = fopen(filename, "rb");
    if (!f)
        panic("file not found: %s\n", filename);

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t* buf = new uint8_t[size];
    if (fread(buf, size, 1, f) != 1)
        panic("read failed\n");

    fclose(f);
    if (out_size)
        *out_size = size;

    return buf;
}

// ---- Stats

struct SymbolStats
{
    uint32_t freqs[256];
    uint32_t cum_freqs[257];

    void count_freqs(uint8_t const* in, size_t nbytes);
    void calc_cum_freqs();
    void normalize_freqs(uint32_t target_total);
};

void SymbolStats::count_freqs(uint8_t const* in, size_t nbytes)
{
    for (int i=0; i < 256; i++)
        freqs[i] = 0;

    {for (size_t i=0; i < nbytes; i++)
        freqs[in[i]]++;}
}

void SymbolStats::calc_cum_freqs()
{
    cum_freqs[0] = 0;
    for (int i=0; i < 256; i++)
        cum_freqs[i+1] = cum_freqs[i] + freqs[i];
}

void SymbolStats::normalize_freqs(uint32_t target_total)
{
    assert(target_total >= 256);
    
    calc_cum_freqs();
    uint32_t cur_total = cum_freqs[256];
    
    // resample distribution based on cumulative freqs
    for (int i = 1; i <= 256; i++)
        cum_freqs[i] = ((uint64_t)target_total * cum_freqs[i])/cur_total;

    // if we nuked any non-0 frequency symbol to 0, we need to steal
    // the range to make the frequency nonzero from elsewhere.
    //
    // this is not at all optimal, i'm just doing the first thing that comes to mind.
    {for (int i=0; i < 256; i++) {
        if (freqs[i] && cum_freqs[i+1] == cum_freqs[i]) {
            // symbol i was set to zero freq

            // find best symbol to steal frequency from (try to steal from low-freq ones)
            uint32_t best_freq = ~0u;
            int best_steal = -1;
            for (int j=0; j < 256; j++) {
                uint32_t freq = cum_freqs[j+1] - cum_freqs[j];
                if (freq > 1 && freq < best_freq) {
                    best_freq = freq;
                    best_steal = j;
                }
            }
            assert(best_steal != -1);

            // and steal from it!
            if (best_steal < i) {
                for (int j = best_steal + 1; j <= i; j++)
                    cum_freqs[j]--;
            } else {
                assert(best_steal > i);
                for (int j = i + 1; j <= best_steal; j++)
                    cum_freqs[j]++;
            }
        }
    }}

    // calculate updated freqs and make sure we didn't screw anything up
    assert(cum_freqs[0] == 0 && cum_freqs[256] == target_total);
    {for (int i=0; i < 256; i++) {
        if (freqs[i] == 0)
            assert(cum_freqs[i+1] == cum_freqs[i]);
        else
            assert(cum_freqs[i+1] > cum_freqs[i]);

        // calc updated freq
        freqs[i] = cum_freqs[i+1] - cum_freqs[i];
    }}
}

int main(int argc, const char *argv[])
{
  if (argc != 2) {
    printf("ANS COMPRESSOR+DECOMPRESSOR\n");
    printf("---------------------------\n");
    printf("test compression: rans.exe <sourcefilename>\n");
    exit(0);
  }

  size_t in_size;
  uint8_t* in_bytes = read_file(argv[1], &in_size);

  static const uint32_t prob_bits = 14;
  static const uint32_t prob_scale = 1 << prob_bits;

  // cumlative->symbol table
  // this is super brute force
  uint8_t cum2sym[prob_scale];
  RansEncSymbol esyms[256];
  RansDecSymbol dsyms[256];

  SymbolStats stats;
  stats.count_freqs(in_bytes, in_size);
  stats.normalize_freqs(prob_scale);

  for (int s=0; s < 256; s++)
      for (uint32_t i=stats.cum_freqs[s]; i < stats.cum_freqs[s+1]; i++)
          cum2sym[i] = s;

  static size_t out_max_size = 16<<20; // 16MB
  uint8_t* out_buf = new uint8_t[out_max_size];
  uint8_t* dec_bytes = new uint8_t[out_max_size];

  for (int i=0; i < 256; i++) {
      RansEncSymbolInit(&esyms[i], stats.cum_freqs[i], stats.freqs[i], prob_bits);
      RansDecSymbolInit(&dsyms[i], stats.cum_freqs[i], stats.freqs[i]);
  }

  RansState rans; RansEncInit(&rans);

  uint8_t* ptr = out_buf + out_max_size; // *end* of output buffer
  {for (size_t i = in_size; i > 0; i--) { // NB: working in reverse!
      int s = in_bytes[i-1];
      RansEncPutSymbol(&rans, &ptr, &esyms[s]);
  }}
  RansEncFlush(&rans, &ptr);
  uint8_t *rans_begin = ptr;

  printf("Asymmetric numeral system compressed from:[%d] to:[%d] bytes.\n", in_size,(int) (out_buf + out_max_size - rans_begin));


  // test decompressing the data

  memset(dec_bytes, 0xcc, in_size);
  ptr = rans_begin;
  RansDecInit(&rans, &ptr);
  {for (size_t i=0; i < in_size; i++) {
      uint32_t s = cum2sym[RansDecGet(&rans, prob_bits)];
      dec_bytes[i] = (uint8_t) s;
      RansDecAdvanceSymbol(&rans, &ptr, &dsyms[s], prob_bits);
  }}
  if (memcmp(in_bytes, dec_bytes, in_size) == 0)
      printf("encode+decode ok!\n");
  else
      printf("ERROR: bad decoder!\n");

  delete[] dec_bytes;
  delete[] out_buf;
  delete[] in_bytes;
  return 0;
}