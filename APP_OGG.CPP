#include "SNDBLSTR.HPP"
#include "CURSOR.HPP"
#include "STBOGG.HPP"
#include "STBPRINT.HPP"
#include <stdio.h>
#include <conio.h>

#define MAXSHORTS 2048
#define FORCECHANNELCOUNT 2
short samples[MAXSHORTS];

#define BUFFERSIZE (5000)
#define RINGBUFFERSIZE (BUFFERSIZE*2)
volatile unsigned char ringBuffer[RINGBUFFERSIZE];
volatile int destPos = 0;
volatile int endPos = 0;

void sampleOutputCallback(unsigned char *buffer, unsigned int bufferLength, unsigned int playPos) {
  for (int i = 0; i < bufferLength; i++) {
    if (endPos != 0 && destPos>=endPos) {buffer[i]=0; continue;}
    SBL_WriteSample8(buffer, i, ringBuffer[destPos % RINGBUFFERSIZE]);
    destPos++;
  }
}

void printPlayPos(int samplePos,int hz) {
  printf("\n");
  static unsigned char cx = 255;
  static unsigned char cy = 255;
  if (cx == 255) {getBiosCursorPosition(&cx,&cy);cy--;}
  setBiosCursorPosition(cx,cy);
  int ms = (samplePos/(hz/1000)) % 1000; // not so precise
  int s = (samplePos/hz) % 60;
  int m = (samplePos/hz/60) % 60;
  int h = (samplePos/hz/60/60);
  printf("Playtime: %dh %dmin %dsec %dmsec            ",h,m,s,ms);
}

int main(int argc, const char *argv[]) {

  printf("\n");
  printf("SBEMU OGG Player\n");
  printf("-------------------\n");
  if (argc != 2) {
    printf("Usage: APP_OGG.EXE <OGGFILE.MP3>\n");
    printf("in the source folder there should be a test.ogg\n");
    exit(0);
  }
  const char *fileName = argv[1];

  FILE *in = fopen(fileName,"rb");

  if (in == NULL) {
    printf("OGG<%s> not found.\n",fileName);
    exit(0);
  }

  fseek(in,0,SEEK_END);
  int fileSize = ftell(in);
  fseek(in,0,SEEK_SET);
  unsigned char *fileData = (unsigned char*)malloc(fileSize);
  printf("Loading OGG <%s> with %dMb.\n", fileName,fileSize/1024/1024);
  fread(fileData,1,fileSize,in);
  fclose(in);                                                     

  SBL_LockMemory((void*)sampleOutputCallback, 1024);
  SBL_LockMemory((void*)ringBuffer, sizeof(ringBuffer));
  SBL_LockMemory((void*)&destPos, sizeof(int));
  SBL_LockMemory((void*)&endPos, sizeof(int));

  double sourcePos = 0;
  destPos = 0;
  endPos = 0;
  memset((void*)ringBuffer,0,sizeof(ringBuffer));

  unsigned char *ogg = fileData;
  int ogg_bytes = fileSize;

  int oggError = 0;
  stb_vorbis *decoder = stb_vorbis_open_memory(ogg, ogg_bytes, &oggError, NULL);
  if (oggError != VORBIS__no_error) {
    printf("Couldn't start to decode OGG file due to some error.\n");
    free(fileData);
    exit(0);
  }

  stb_vorbis_info info = stb_vorbis_get_info(decoder);
  int sample_rate = info.sample_rate;
  int channels = info.channels;
  printf("OGG channels: %d\n", channels);
  printf("OGG sample rate: %d\n", sample_rate);

  if (sample_rate < 100 || (!SBL_Init())) {
    printf("Sound Blaster not found!\n");
    printf("Maybe the IRQ/DMA/ADDRESS settings are wrong,\n");
    printf("Or you just don't have a Sound Blaster Card installed.\n");
    printf("The App is tested with SBEMU, a Sound Blaster emulation\n");
    printf("for many modern Sound Cards.\n");
    printf("The App is just a testcase for SBEMU.EXE maybe it doesn't\n");
    printf("work with real Sound Blasters at all..\n");
    printf("SBEMU can be found here: https://github.com/crazii/SBEMU\n");
    const char *a = "//";
    free(fileData);
    exit(0);
  }

  SBL_POINTER p = SBL_Malloc(BUFFERSIZE);
  SBL_StartPlayback( &p, BUFFERSIZE,2,sampleOutputCallback);
  int startPos = destPos;

  int soundBlasterHz = SBL_SampleRateCorrected(SBL_SampleRate);
  double rate = (double)soundBlasterHz/sample_rate;

  printf("Sound Blaster Frequency: %d\n",soundBlasterHz);

  printf("Press a key to stop playback.\n");
  while(!kbhit()) {
    int sampleCount = stb_vorbis_get_samples_short_interleaved(decoder, FORCECHANNELCOUNT, samples, MAXSHORTS);

    while((int)(sourcePos+sampleCount*rate)>destPos) {;}

    int k = 0; int a = 0; int w = 0; int l = 0;
    for (int i = 0; i < sampleCount; i++) {
      for (int j = 0; j < FORCECHANNELCOUNT; j++) {a += samples[k++];w++;}
      if ((int)sourcePos != l) {l = (int)sourcePos;ringBuffer[((int)sourcePos) % RINGBUFFERSIZE] = (a/w+32768)>>8; w = 0; a = 0;}
      sourcePos += rate;
    }

    printPlayPos(destPos-startPos,soundBlasterHz);
  }
  endPos = 1; // mute
  SBL_Done();
  SBL_Free(&p);

  SBL_UnlockMemory((void*)sampleOutputCallback, 1024);
  SBL_UnlockMemory((void*)ringBuffer, sizeof(ringBuffer));
  SBL_UnlockMemory((void*)&destPos, sizeof(int));
  SBL_UnlockMemory((void*)&endPos, sizeof(int));

  free(fileData);
  return 0;
}
