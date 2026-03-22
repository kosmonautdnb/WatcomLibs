/*
  MIT License

  Copyright (c) 2026 Stefan Mader

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
//------
// From 286 assembly to 386 flatmode C rather fastly converted DOS replay routine for the:
//------
// Surprise! productions Adlib Tracker Version 2.0 >>> Beta Release <<< (SADT2) by Erik Pojar / S!P
//------
// To be used with WatcomC++/OpenWatcom/DJGPP
//------
// This file is not intended to look good and maybe there are some conversion errors left.
// Conversion done by me (Stefan Mader,Mad^Still^Bauknecht^Puls4r)
//------
// From the Surprise Adlib Tracker 2.0 Author
// Welcome to the SADT V2.0 Editor / Player for Adlib!
// I know this program is out-dated by new soundcards, and I have not used this
// program for a long time now.
// But still I like the sound of FM chips and therefore I wanted to share it with
// YOU.You are free to use this program in any form you like with 3 restrictions:
//
//        * This program package may not be sold.
//          (this means especially PD companies or other assholes who try
//           to earn money with the work of idealistic programmers..)
//
//        * If you use this program, you HAVE to send me a postcard
//          (and a nice one..) My address is at the end of the file.
//
//        * If you use the replay routine in a commercial production
//          (a game or so..) you HAVE to send me a copy of it.
//
// I will kick everybody's ass who violates any of these restrictions.
// I would be pleased if you send me music you composed with my little editor.
// If you have any ideas or comments to make, send them to me immediately.
// ------------------------------------------------------------------------------
//    Erik Pojar                 Voice: ++43 (1) 2574457 (regular times please!)
//    Siegfriedgasse 55
//    1210 Wien                  epojar@stud1.tuwien.ac.at
//    Austria
// ------------------------------------------------------------------------------
//------
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>
#ifdef __DJGPP__
#include <dpmi.h>
#include <go32.h>
#endif // __DJGPP__

#pragma off(check_stack)

#define UNUSED(__p__) (void)(__p__)

unsigned int adlibTrackerDataLockStart = 0;
void adlibTrackerCodeLockStart() {
  inp(0x388);
}

#define REGB register_bypass2
volatile unsigned char register_bypass2[246] = {0};
volatile unsigned char register_map[9] = {0,1,2,8,9,10,16,17,18};
volatile unsigned short InsTab[32] = {0};

volatile unsigned char mainvol = 0;
volatile unsigned short current_bpm = 0;
volatile unsigned short segnum = 0;
volatile unsigned char count = 0;
volatile unsigned char _eff = 0;
volatile unsigned char _para = 0;

const unsigned int _patternorder = 966;
const unsigned int _songlength = 1096;
const unsigned int _restart = 1097;
const unsigned int _bpm = 1098;
const unsigned int _arpeggio = 1100;
const unsigned int _acommands = 1356;
const unsigned int _trackorder = 1612;
const unsigned int _channels = 2188;
const unsigned int _trackdata = 2190;

#define _B16(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16) (((unsigned long int)(_1)<<15)|((_2)<<14)|((_3)<<13)|((_4)<<12)|((_5)<<11)|((_6)<<10)|((_7)<<9)|((_8)<<8)|((_9)<<7)|((_10)<<6)|((_11)<<5)|((_12)<<4)|((_13)<<3)|((_14)<<2)|((_15)<<1)|((_16)<<0))
#define _B8(_1,_2,_3,_4,_5,_6,_7,_8) (((unsigned long int)(_1)<<15)|((_2)<<14)|((_3)<<13)|((_4)<<12)|((_5)<<11)|((_6)<<10)|((_7)<<9)|((_8)<<8))
const unsigned int Okt1 = _B16(0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0);
const unsigned int Okt2 = _B16(0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0);
const unsigned int Okt3 = _B16(0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0);
const unsigned int Okt4 = _B16(0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0);
const unsigned int Okt5 = _B16(0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0);
const unsigned int Okt6 = _B16(0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0);
const unsigned int Okt7 = _B16(0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0);
const unsigned int Okt8 = _B16(0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0);

unsigned short NoteTab[] = {0,                                                         // dummy note
  Okt1+343,Okt1+363,Okt1+385,Okt1+408,Okt1+432,Okt1+458,
  Okt1+485,Okt1+514,Okt1+544,Okt1+577,Okt1+611,Okt1+647,     // Oktave3
  Okt2+343,Okt2+363,Okt2+385,Okt2+408,Okt2+432,Okt2+458,
  Okt2+485,Okt2+514,Okt2+544,Okt2+577,Okt2+611,Okt2+647,     // Oktave3
  Okt3+343,Okt3+363,Okt3+385,Okt3+408,Okt3+432,Okt3+458,
  Okt3+485,Okt3+514,Okt3+544,Okt3+577,Okt3+611,Okt3+647,     // Oktave3
  Okt4+343,Okt4+363,Okt4+385,Okt4+408,Okt4+432,Okt4+458,
  Okt4+485,Okt4+514,Okt4+544,Okt4+577,Okt4+611,Okt4+647,     // Oktave4
  Okt5+343,Okt5+363,Okt5+385,Okt5+408,Okt5+432,Okt5+458,
  Okt5+485,Okt5+514,Okt5+544,Okt5+577,Okt5+611,Okt5+647,     // Oktave5
  Okt6+343,Okt6+363,Okt6+385,Okt6+408,Okt6+432,Okt6+458,
  Okt6+485,Okt6+514,Okt6+544,Okt6+577,Okt6+611,Okt6+647,     // Oktave6
  Okt7+343,Okt7+363,Okt7+385,Okt7+408,Okt7+432,Okt7+458,
  Okt7+485,Okt7+514,Okt7+544,Okt7+577,Okt7+611,Okt7+647,     // Oktave7
  Okt8+343,Okt8+363,Okt8+385,Okt8+408,Okt8+432,Okt8+458,
  Okt8+485,Okt8+514,Okt8+544,Okt8+577,Okt8+611,Okt8+647    // Oktave8
};
unsigned char arpeggio_table[] = {0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,1};
unsigned char vibratotab[] = {0, 24, 49, 74, 97,120,141,161,180,197,212,224,235,244,250,253,255,253,250,244,235,224,212,197,180,161,141,120, 97, 74, 49, 24};

volatile unsigned char stopped[9] = {0};
volatile unsigned char channel_active[9] = {0};
volatile unsigned char channel_ins[9] = {0};
volatile unsigned char channel_note[9] = {0};
volatile unsigned char channel_note2[9] = {0};
volatile unsigned char channel_vol2[9] = {0};
volatile unsigned char channel_vol[9] = {0};
volatile unsigned char channel_vpos[9] = {0};
volatile unsigned char channel_vspeed[9] = {0};
volatile unsigned char channel_vdepth[9] = {0};
volatile unsigned short channel_eff[9] = {0};
volatile unsigned short channel_freq[9] = {0};
volatile unsigned short channel_fshift[9] = {0};
volatile unsigned short channel_pdest[9] = {0};
volatile unsigned short channel_pspeed[9] = {0};
volatile unsigned char channel_aspeed[9] = {0};
volatile unsigned char channel_acurpos[9] = {0};
volatile unsigned char channel_acursp[9] = {0};
volatile unsigned char channel_vsubt[9] = {0};
volatile unsigned char current_speed[9] = {0};
volatile unsigned char current_line[9] = {0};
volatile unsigned char break_pos[9] = {0};
volatile unsigned char pattern_break[9] = {0};
volatile unsigned char position_jump[9] = {0};
volatile unsigned char *segments[9] = {NULL};
volatile unsigned char channel_segment[9] = {0};
volatile unsigned int work_pattern[9] = {_trackorder,_trackorder,_trackorder,_trackorder,_trackorder,_trackorder,_trackorder,_trackorder,_trackorder};
volatile unsigned char current_pos[9] = {0};
volatile unsigned char s_count[9] = {0};
volatile unsigned char new_position[9] = {0};

#define ADLIB_PORTA 0x388
#define ADLIB_PORTB 0x389

void waitlong() {
  for (int i = 0; i < 35; i++) inp(ADLIB_PORTA);
}

void WriteThrough(unsigned int ax, unsigned int bx) {
  if (bx >= 246) exit(0);
  REGB[bx] = (unsigned char)(ax>>8);
  outp(ADLIB_PORTA,bx & 255);
  waitlong();
  outp(ADLIB_PORTB,ax >> 8);
}

void fastwrite(unsigned int ax, unsigned int bx) {
  if (bx >= 246) exit(0);
  if (REGB[bx] == (ax>>8)) return;
  REGB[bx] = (unsigned char)(ax>>8);
  outp(ADLIB_PORTA,bx & 255);
  waitlong();
  outp(ADLIB_PORTB,ax >> 8);
}

void Adwrite(unsigned int r, unsigned int w) {
  outp(ADLIB_PORTA,r);
  waitlong();
  outp(ADLIB_PORTB,w);
}

void DefineBoth(volatile unsigned char *ds, unsigned int &si, unsigned int bx, unsigned int cl) {
  bx += cl;
  unsigned short ax = *((unsigned short*)&ds[si]); si += 2;
  unsigned int ch = ax>>8;
  ax = (unsigned short)((ax & 0x00ff)|((ax<<8)&0xff00));
  fastwrite(ax,bx); // Connector
  bx += 3;
  ax = (unsigned short)((ax & 0x00ff)|((ch<<8)&0xff00));
  fastwrite(ax,bx); // Carrier
}

void init_adlib() {
  for (int bx = 1; bx < 1 + 245; bx++) {
    WriteThrough(0,bx);
  }
  WriteThrough(0x20<<8,1);
  WriteThrough(0x40<<8,8);
  WriteThrough(0xc0<<8,0xbd);
  unsigned short ax = 5;
  for (int i = 0; i < 31; i++) {
    InsTab[i] = ax;
    ax += 15;
  }
  mainvol = 64;
}

void init_adlib_tables() {
  for (int cx = 0; cx < 9; cx++) {
    stopped[cx] = 1;
    channel_active[cx] = 0;
    channel_ins[cx] = 0;
    channel_note[cx] = 0;
    channel_note2[cx] = 0;
    channel_vol[cx] = 0;
    channel_vol2[cx] = 0;
    channel_vpos[cx] = 0;
    channel_vspeed[cx] = 0;
    channel_vdepth[cx] = 0;
    channel_eff[cx] = 0;
    channel_freq[cx] = 0;
    channel_fshift[cx] = 0;
    channel_pdest[cx] = 0;
    channel_pspeed[cx] = 0;
    channel_aspeed[cx] = 0;
    channel_acurpos[cx] = 0;
    channel_acursp[cx] = 0;
    channel_vsubt[cx] = 0;
    current_speed[cx] = 6;
    current_line[cx] = 0;
    break_pos[cx] = 0;
    pattern_break[cx] = 0;
    position_jump[cx] = 0;
  }
  mainvol = 64;
}

void mute_voice(int i) {
  unsigned int bx = i;
  unsigned int dl = register_map[bx];
  unsigned int cl = dl;
  dl += 0x60;
  bx = (bx & 0xff00)|dl;
  unsigned int di = bx;
  unsigned int ax = 0xff00;
  fastwrite(ax,bx);
  bx = di + 0x03;
  ax = 0xff00;
  fastwrite(ax,bx);
  bx = di + 0x20;
  ax = 0xff00;
  fastwrite(ax,bx);
  bx = di + 0x23;
  ax = 0xff00;
  fastwrite(ax,bx);
  bx = i + 0xb0;
  di = bx;
  ax = 0x2000;
  fastwrite(ax,bx);
  bx = (bx & 0xff00)|cl;
  bx += 0x40;
  ax = (ax & 0x00ff)|(REGB[bx]<<8);
  ax |= 0x3f00;
  fastwrite(ax,bx);
  ax = ax & 0x00ff;
  bx = di;
  fastwrite(ax,bx);
}

void clear_all_voices() {
  for (int i = 0; i < 9; i++) mute_voice(i);
}

void stop_all() {
  clear_all_voices();
  init_adlib();
  init_adlib_tables();
}

void init_device() {
  init_adlib();
  init_adlib_tables();
}

bool load_song(unsigned int bl, volatile unsigned char *ds) {
  if (bl > 8) return false;
  if (segments[bl]!=NULL) return false;
  unsigned long int ax = *((unsigned short*)&ds[_channels]);
  {for (int si = 0; si < 9; si++) {
    ax *= 2;
    if (ax>=0x10000) {
      ax &= 0xffff;
      if (channel_active[si]!=0) return false;
    }
  }}
  ax = *((unsigned short*)&ds[_channels]);
  {for (int si = 0; si < 9; si++) {
    ax *= 2;
    if (ax>=0x10000) {
      ax &= 0xffff;
      channel_active[si] = 0;
      channel_segment[si] = (unsigned char)bl;
    }
  }}
  stopped[bl] = 1;
  segments[bl] = ds;
  current_bpm = *((unsigned short*)&ds[_bpm]);
  return true;
}

bool unload_song(unsigned int bl) {
  if (bl > 8) return false;
  if (segments[bl] == NULL) return false;
  volatile unsigned char *ds = segments[bl];
  segments[bl] = NULL;
  unsigned long int ax = *((unsigned short*)&ds[_channels]);
  {for (int si = 0; si < 9; si++) {
    ax *= 2;
    if (ax>=0x10000) {
      ax &= 0xffff;
      if (channel_active[si]==0) return false;
      if (channel_segment[si]!=bl) return false;
    }
  }}

  ax = *((unsigned short*)&ds[_channels]);
  {for (int si = 0; si < 9; si++) {
    ax *= 2;
    if (ax>=0x10000) {
      ax &= 0xffff;
      channel_active[si] = 0;
      channel_segment[si] = 0;
      mute_voice(si);
    }
  }}
  stopped[bl] = 1;
  segments[bl] = NULL;
  return true;
}

void calc_work_pattern(unsigned int bx, volatile unsigned char *ds, unsigned int ax) {
  work_pattern[bx] = (unsigned int)ds[(ax&255) + _patternorder] * 9 + _trackorder;
}

bool start_song(unsigned int bl) {
  if (bl > 8) return false;
  if (segments[bl] == NULL) return false;
  volatile unsigned char *ds = segments[bl];
  calc_work_pattern(bl,ds,0);
  current_line[bl] = 0;
  current_speed[bl] = 6;
  pattern_break[bl] = 0;
  current_pos[bl] = 0;
  s_count[bl] = 0;
  position_jump[bl] = 0;
  stopped[bl] = 0;
  unsigned long int ax = *((unsigned short*)&ds[_channels]);
  for (int si = 0; si < 9; si++) {
    ax *= 2;
    if (ax>=0x10000) {
      ax &= 0xffff;
      channel_active[si] = 1;
    }
  }
  return true;
}

bool prepare_song(unsigned char *data) {
  bool succeeded = true;
  init_device();
  succeeded = succeeded && load_song(0,data);
  succeeded = succeeded && start_song(0);
  return succeeded;
}

int calcvol(int al) {
  return -(((-(long int)(al - 63))*mainvol)/64-63);
}

bool set_main_volume(unsigned char bl) {
  if (bl > 63) return false;
  if (mainvol == bl) return true;
  mainvol = bl;
  for (int si = 0; si < 9; si++) {
    if (channel_active[si]!=0) {
      unsigned int al = channel_vol2[si];
      unsigned int bx = register_map[si] + 0x40;
      unsigned int ax = (calcvol(al)<<8)+(REGB[bx]&0xc0);
      ax |= (ax&255)<<8;
      fastwrite(ax,bx);
      bx += 3;
      al = channel_vol[si];
      ax = (calcvol(al)<<8)+(REGB[bx]&0xc0);
      ax |= (ax&255)<<8;
      fastwrite(ax,bx);
    }
  }
  return true;
}

bool testadlib() {
  Adwrite(1,0);
  Adwrite(4,0x60);
  Adwrite(4,0x80);
  int al = inp(ADLIB_PORTA);
  al &= 0xe0;
  int bl = al;
  Adwrite(2,0xff);
  Adwrite(4,0x31);
  for (int cx = 0; cx<6; cx++) waitlong();
  al = inp(ADLIB_PORTA);
  al &= 0xe0;
  int bh = al;
  Adwrite(4,0x60);
  Adwrite(4,0x80);
  if (bl != 0) return false;
  if (bh != 0xc0) return false;
  return true;
}

bool test_device() {
  return testadlib();
}

//-------------

typedef void (*SoundEffect)(unsigned int bp, unsigned int ax);

void set_freq(unsigned int bp, unsigned int cx) {
  unsigned int bx = bp + 0xa0;
  fastwrite((cx<<8) & 0xff00,bx);
  bx += 0x10;
  unsigned int ah = REGB[bx];
  ah &= 32;
  cx &= 0x1f00;
  ah |= cx>>8;
  fastwrite(ah<<8,bx);
  channel_freq[bp] = (unsigned short)cx;
}

void effect_arpeggio(unsigned int bp, unsigned int ax) {
  unsigned int cl = channel_note2[bp];
  unsigned int bl = count;
  bl = arpeggio_table[bl];
  if (bl == 1) {
    cl += (ax>>8) & 0x0f;
  } else if (bl != 0) {
    cl += (ax>>8>>4);
  }
  if (cl >= 96) cl = 96;
  set_freq(bp,NoteTab[cl]);
}

unsigned int calc_freq_up(unsigned int cx, unsigned int ax) {
  unsigned int dx = cx;
  unsigned int di = cx;
  dx &= _B16(0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0);
  cx &= 1023;
  cx += ax;
  if (cx < 647) return cx|dx;
  ax = di;
  do {
    ax &= _B16(0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0);
    if (ax == _B16(0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0)) {
      cx = di;
      return cx|dx;
    }
    cx >>= 1; if (cx & 0x4000) cx |= 0x8000;
    ax += _B16(0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0);
    di &= _B16(0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0);
    ax |= di;
    dx = ax;
  } while(cx > 647);
  return cx|dx;
}

unsigned int calc_freq_down(unsigned int cx, unsigned int ax) {
  unsigned int dx = cx;
  unsigned int di = cx;
  dx &= _B16(0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0);
  cx &= 1023;
  cx -= ax;
  if (cx > 343) return cx|dx;
  ax = di;
  do {
    ax &= _B16(0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0);
    if (ax == 0) {
      cx = di;
      return cx|dx;
    }
    cx *= 2;
    int ah = (((ax>>8)/2)-1)*2;
    ax = (ax & 0x00ff)|(ah<<8);
    di &= _B16(0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0);
    ax |= di;
    dx = ax;
  } while(cx < 343);
  return cx|dx;
}

void frequenz_up2(unsigned int bp, unsigned int ax) {
  unsigned int cx = channel_freq[bp];
  cx = calc_freq_up(cx,ax);
  int bx = bp + 0xa0;
  fastwrite((cx & 255)<<8,bx);
  bx += 0x10;
  unsigned int ah = REGB[bx];
  ah &= 32;
  unsigned int ch = (cx>>8) & 31;
  ah |= ch;
  fastwrite(ah<<8,bx);
}

void frequenz_down2(unsigned int bp, unsigned int ax) {
  unsigned int cx = channel_freq[bp];
  cx = calc_freq_down(cx,ax);
  int bx = bp + 0xa0;
  fastwrite((cx & 255)<<8,bx);
  bx += 0x10;
  unsigned int ah = REGB[bx];
  ah &= 32;
  unsigned int ch = (cx>>8) & 31;
  ah |= ch;
  fastwrite(ah<<8,bx);
}


void do_vibrato(unsigned int bp, unsigned int ax, unsigned int dl) {
  int bl = channel_vpos[bp] + dl;
  channel_vpos[bp] = (unsigned char)bl;
  int dh = bl;
  bl &= 0x1f;
  dl = vibratotab[dl];
  ax *= dl * 2;
  ax = ((ax>>8)&0x00ff)|((ax<<8)&0xff00);
  ax &= 0x01ff;
  if ((dh & 32)!=0) {
    frequenz_up2(bp,ax);
  } else {
    frequenz_down2(bp,ax);
  }
}

void execute_vibrato(unsigned int bp) {
  do_vibrato(bp, channel_vdepth[bp], channel_vspeed[bp]);
}


void effect_vibrato(unsigned int bp, unsigned int ax) {
  if (count == 0 && (ax>>8)!=0) {
    unsigned int al = ax>>8;
    unsigned int ah = al;
    al &= 15;
    ah >>= 4;
    channel_vspeed[bp] = (unsigned char)ah;
    channel_vdepth[bp] = (unsigned char)al;
    int dl = ah;
    do_vibrato(bp,al|(ah<<8),dl);
  } else {
    execute_vibrato(bp);
  }
}

void shift_helper(unsigned int bp, unsigned int ax, unsigned int cx) {
  unsigned int bx = bp;
  if ((short)ax<0) {
    ax = -ax;
    cx = calc_freq_down(cx,ax);
  } else {
    cx = calc_freq_up(cx,ax);
  }
  bx += 0xa0;
  fastwrite((cx&255)<<8,bx);
  bx += 0x10;
  unsigned int al = REGB[bx];
  al &= 32;
  unsigned int ah = (cx>>8) & 255;
  ah &= _B8(1,1,0,1,1,1,1,1);
  ah |= al;
  fastwrite(ah<<8,bx);
  channel_freq[bp] = (unsigned short)cx;
}


void effect_slide_up(unsigned int bp, unsigned int ax) {
  unsigned int bx = bp;
  unsigned int ah = (ax>>8);
  unsigned int al = ah;
  ah = 0;
  channel_fshift[bx] += (unsigned char)al;
  ax = channel_fshift[bx];
  unsigned int bl = channel_note2[bp];
  unsigned int cx = NoteTab[bl];
  shift_helper(bp,ax,cx);
}

void effect_slide_down(unsigned int bp, unsigned int ax) {
  unsigned int bx = bp;
  unsigned int ah = (ax>>8);
  unsigned int al = ah;
  ah = 0;
  channel_fshift[bx] -= (unsigned char)al;
  ax = channel_fshift[bx];
  unsigned int bl = channel_note2[bp];
  unsigned int cx = NoteTab[bl];
  shift_helper(bp,ax,cx);
}

void execute_portamento(unsigned int bp) {
  unsigned int bx = bp;
  bx += 0xa0;
  unsigned int al = REGB[bx];
  bx += 0x10;
  unsigned int ah = REGB[bx];
  bx = bp;
  unsigned int dx = channel_pdest[bx];
  unsigned int cx = channel_pspeed[bx];
  unsigned int ax = al|(ah<<8);
  if (ax != cx) {
    if (ax > cx) {
      bx = bp;
      ax = cx;
      unsigned int p = dx;
      bx += 0xb0;
      unsigned int ch = REGB[bx];
      bx -= 0x10;
      unsigned int cl = REGB[bx];
      cx = calc_freq_down(cl|(ch<<8),ax);
      dx = p;
      bx = cx;
      ax = dx;
      bx &= _B16(0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0);
      ax &= _B16(0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0);
      if (bx<=ax) {
        bx = cx;
        ax = dx;
        bx &= _B16(0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1);
        ax &= _B16(0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1);
        if (bx<=ax) {
          cx = dx;
        }
      }
      set_freq(bp,cx);
    } else {
      bx = bp;
      ax = cx;
      unsigned int p = dx;
      bx += 0xb0;
      unsigned int ch = REGB[bx];
      bx -= 0x10;
      unsigned int cl = REGB[bx];
      cx = calc_freq_up(cl|(ch<<8),ax);
      dx = p;
      bx = cx;
      ax = dx;
      bx &= _B16(0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0);
      ax &= _B16(0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0);
      if (bx>=ax) {
        bx = cx;
        ax = dx;
        bx &= _B16(0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1);
        ax &= _B16(0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1);
        if (bx>=ax) {
          cx = dx;
        }
      }
      set_freq(bp,cx);
    }
  }
}


void effect_portamento(unsigned int bp, unsigned int ax) {
  unsigned int al = ax & 255;
  unsigned int ah = ax>>8;
  if (count == 0) {
    unsigned int bx = bp;
    if (ah != 0) {
      al = ah;
      ah = 0;
      channel_pspeed[bx] = (unsigned short)(al | (ah<<8));
    }
    unsigned int bl = channel_note[bp];
    if (bl != 0) {
      channel_pdest[bp] = NoteTab[bl];
    }
  }
  execute_portamento(bp);
}

void set_volume(unsigned int bx, unsigned int ax) {
  unsigned int di = bx;
  bx = 0x40 + register_map[bx];
  unsigned int cx = ax;
  unsigned int al = (cx>>8);
  al &= 63;
  channel_vol2[di] = (unsigned char)al;
  ax = (calcvol(al)<<8) | (REGB[bx] & 0xc0);
  ax |= (ax & 255)<<8;
  fastwrite(ax,bx);
  bx += 3;
  al = cx & 255;
  al &= 63;
  channel_vol[di] = (unsigned char)al;
  ax = (calcvol(al)<<8) | (REGB[bx] & 0xc0);
  ax |= (ax & 255)<<8;
  fastwrite(ax,bx);
}

void effect_volume_slide(unsigned int bp, unsigned int ax) {
  unsigned int ah = ax >> 8;
  if (count == 0) {
    unsigned int dl = ax>>8;
    if (dl >= 16) {
      dl >>= 4;
      dl = -dl;
    }
    *(((unsigned char*)&channel_eff[bp])+1) = (unsigned char)dl;
    ah = dl;
    channel_vsubt[bp] = 0;
  }
  unsigned int dl = ah;
  unsigned int dh = channel_vsubt[bp];
  dh += dl;
  unsigned int ch = dh;
  ch /= 4;
  ah = channel_vol2[bp];
  unsigned int al = channel_vol[bp];
  ah += ch;
  al += ch;
  if ((signed char)al<0) al = 0;  
  if ((signed char)ah<0) ah = 0;  
  if (al>63) al = 63;  
  if (ah>63) ah = 63;  
  ch *= 4;
  dh -= ch;
  channel_vsubt[bp] = (unsigned char)dh;
  set_volume(bp,al|(ah<<8));
}

void effect_portamento_volume_slide(unsigned int bp, unsigned int ax) {
  effect_volume_slide(bp,ax);
  execute_portamento(bp);
}

void effect_vibrato_volume_slide(unsigned int bp, unsigned int ax) {
  effect_volume_slide(bp,ax);
  execute_vibrato(bp);
}

void effect_set_volume(unsigned int bp, unsigned int ax) {
  unsigned int ah = ax >> 8;
  if (ah>63) ah = 63;
  ah = 63-ah;
  set_volume(bp, ah|(ah<<8));
}

void effect_position_jump(unsigned int bp, unsigned int ax) {
  unsigned int bx = segnum;
  position_jump[bx] = 1;
  unsigned int ah = ax>>8;
  unsigned int al = ah;
  ah = 0;
  new_position[bx] = (unsigned char)al;
  break_pos[bx] = 0;
  pattern_break[bx] = 1;
  channel_eff[bp] = 0;
}

void effect_pattern_break(unsigned int bp, unsigned int ax) {
  unsigned int bx = segnum;
  pattern_break[bx] = 1;
  bx = ax;
  unsigned int ah = (ax>>8);
  unsigned int al = (ah>>4);
  al *= 10;
  bx = bx & 0x0fff;
  al += bx >> 8;
  ah = 0;
  if (bx > 8) exit(0);
  break_pos[bx] = (unsigned char)al;
  channel_eff[bp] = 0;
}

unsigned int release_note(unsigned int bx) {
  bx += 0xb0;
  unsigned int ah = REGB[bx];
  if ((ah & 32)!=0) {
    ah &= 255-32;
    WriteThrough(ah<<8,bx);
  }
  return bx;
}

void effect_release_note(unsigned int bp, unsigned int ax) {
  UNUSED(ax);
  unsigned int bx = bp; release_note(bx);
  channel_eff[bp]  = 0;
}

void effect_set_speed(unsigned int bp, unsigned int ax) {
  unsigned int bx = segnum;
  unsigned int ah = ax >> 8;
  if (ah == 0) ah = 1;
  if (ah > 31) {
    // not implemented, yet, in this c port..
  } else {
    current_speed[bx] = (unsigned char)ah;
  }
  channel_eff[bp] = 0;
}

void effect_empty(unsigned int bp, unsigned int ax) {
  UNUSED(bp); UNUSED(ax);
}

//-------------

SoundEffect effect_calls[0x10] = {
  effect_arpeggio,                       //0
  effect_slide_up,                       //1
  effect_slide_down,                     //2
  effect_portamento,                     //3
  effect_vibrato,                        //4
  effect_portamento_volume_slide,        //5
  effect_vibrato_volume_slide,           //6
  effect_empty,                                 //7
  effect_release_note,                   //8
  effect_empty,                                 //9
  effect_volume_slide,                   //A
  effect_position_jump,                  //B
  effect_set_volume,                     //C
  effect_pattern_break,                  //D
  effect_empty,                                 //E
  effect_set_speed                      //F
};

void play_line_effects(unsigned int bp) {
  unsigned int ax = channel_eff[bp];
  unsigned int bx = ax;
  if (bx != 0) {
    bx &= 0x00ff;
    effect_calls[bx](bp,ax);
  }
}

void do_volume(unsigned int bp, volatile unsigned char *ds, int al) {
  unsigned int bx = (al - 1) & 255;
  if (bx>=32) exit(0);
  unsigned int si = InsTab[bx] + 9;
  unsigned int ah = ds[si];
  al = ds[si+1];
  unsigned int ax = ((ah<<8)+al) & 0x3F3F;
  set_volume(bp,ax);
}

void init_instrument2(volatile unsigned char *ds,unsigned int bp, unsigned int ch) {
  if (ch == 0||ch>=32) exit(0);
  unsigned int si = InsTab[ch-1];
//;--- spec. arpeggio init ----------------------------------
  unsigned int bx = si;
  si += 11;
  unsigned int al = ds[si]; si++;
  channel_acurpos[bp] = (unsigned char)al;
  al = ds[si]; si++;
  if (al != 0) al++;
  channel_acursp[bp] = 0;
  channel_aspeed[bp] = (unsigned char)al;
  si = bx;
//;----------------------------------------------------------
  bx = bp;
  unsigned int cl = bx & 255;
  bx = 0xc0 + cl;
  al = ds[si]; si++;
  unsigned int ah = al;
  ch = al;
  fastwrite(al+(ah<<8),bx);
  cl = register_map[bp];
  
  bx = 0x20;
  DefineBoth(ds,si,bx,cl);

  bx = 0x60;
  DefineBoth(ds,si,bx,cl);

  bx = 0x80;
  DefineBoth(ds,si,bx,cl);

  bx = 0xe0;
  DefineBoth(ds,si,bx,cl);

  bx = 0x40 + cl;

  unsigned int cx = *((unsigned short*)&ds[si]); si+=2;
  
  unsigned int ax = channel_vol2[bp]|(channel_vol[bp]<<8);
  ax &= _B16(0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1);
  cx &= _B16(1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0);
  ax |= cx;
  cx |= ax;
  ax = calcvol(ax & 63);
  ax |= ax << 8;
  cl = cx & 0xc0;
  ax |= cl << 8;
  fastwrite(ax,bx);
  bx += 3;
  ax = (ax & 0xff00) | (cx>>8);
  ax = calcvol(ax & 63);
  ax |= ax << 8;
  ch = (cx>>8) & 0xc0;
  ax |= ch << 8;
  fastwrite(ax,bx);
}

void play_note2(unsigned int bp, unsigned int cx) {
  unsigned int bx = release_note(bp);
  if ((cx & 255) >= sizeof(NoteTab)/2) exit(0);
  cx = NoteTab[cx & 255];
  if (bx < 0x10) exit(0);
  fastwrite((cx&0x00ff)<<8,bx-0x10);
  WriteThrough(cx&0xff00,bx);
  channel_freq[bp] = (unsigned short)cx;
  channel_fshift[bp] = 0;
}

unsigned int retrig_note(unsigned int bx) {
  unsigned int bp = bx;
  bx += 0xb0;
  unsigned int ah = REGB[bx];
  if ((ah & 32) == 0) {
    ah |= 32;
    WriteThrough(ah<<8,bx);
    return bx;
  }
  release_note(bp);
  retrig_note(bp);
  return bx;
}

void select_wave(unsigned int bx, unsigned int cx) {
  unsigned int bl = register_map[bx];
  unsigned int bh = 0;
  bx = bl|(bh<<8);
  bx += 0xe0;
  unsigned int ah = cx & 255;
  if (ah >= 1) {
    ah--;
    fastwrite(ah<<8,bx);
  }
  bx += 3;
  ah = cx >> 8;
  if (ah >= 1) {
    ah--;
    fastwrite(ah<<8,bx);
  }
}


void spec_arpeggio(volatile unsigned char *ds, unsigned int bp) {
  // not tested
  if (channel_acurpos[bp] == 0) return;
  channel_acursp[bp]--;
  if (channel_acursp[bp] > 0) return;
  unsigned int al = channel_acurpos[bp];
  unsigned int ah = 0;
  unsigned int di = al | (ah<<8);
  al = ds[_acommands+di];
  if (al<254) {
    if (al==255) {
      channel_acurpos[bp] = 0;
      return;
    }
    al = ds[_arpeggio+di];
    di = al|(ah<<8);
    channel_acurpos[bp] = (unsigned char)al;
    al = ds[_acommands+di];
    if (al != 0) {
      if (al <= 44) {
        ah = 0;
        unsigned int cx = (al/10)<<8;
        select_wave(bp,cx);
      } else {
        if (al == 253) {
          release_note(bp);
        }
        if (al == 252) {
          effect_set_volume(bp,ds[_arpeggio+di]<<8);
        }        
        if (al == 251) {
          ah = ds[_arpeggio+di];
          effect_set_volume(bp,ah<<8);
          retrig_note(bp);
          al = channel_aspeed[bp]; // no_half
          channel_acursp[bp] = (unsigned char)al;
          channel_acurpos[bp]++;
          return;
        }
      }
    }
  }

  al = ds[_arpeggio+di];
  if (al <= 100) {
    ah = channel_note[bp];
    al += ah;
  } else {
    al -= 99;
  }
  if (al>96) al = 96;
  ah = 0;
  channel_note2[bp] = (unsigned char)al;
  unsigned int cx = NoteTab[al];
  unsigned int ax = channel_fshift[bp];
  shift_helper(bp,ax,cx);
  al = channel_aspeed[bp]; // no_half
  channel_acursp[bp] = (unsigned char)al;
  channel_acurpos[bp]++;
}

void play_line() {
  for (int bp = 0; bp < 9; bp++) {
    if (channel_active[bp] == 1) {
      unsigned int bl = channel_segment[bp];
      segnum = (unsigned char)bl;
      unsigned int al = s_count[bl];
      count = (unsigned char)al;
      volatile unsigned char *ds = segments[bl];
      if (al != 0) {
        spec_arpeggio(ds,bp);
        play_line_effects(bp);
      } else {
        al = current_line[bl];
        unsigned int si = al * 3;
        al = ds[work_pattern[bl]+bp];
        if (al == 0) {
        } else {
          al--;
          si += al * 192 + _trackdata;
          if (count <= 0) { //jbe donote
            al = ds[si]; si++;
            unsigned int cl = al;
            al = ds[si]; si++;
            unsigned int ah = al;
            al = ds[si]; si++;
            _para = (unsigned char)al;
            _eff = (unsigned char)(ah & 15);
            ah = ah & 0xf0;
            ah = (ah>>1)|((cl & 1)<<7);
            cl >>= 1;
            ah = (ah>>1)|((ah & 1)<<7);
            ah = (ah>>1)|((ah & 1)<<7);
            ah = (ah>>1)|((ah & 1)<<7);
            al = ah;

            unsigned int ch = al;
            if ((cl|(ch<<8))!=0) {
              if (al != 0) {
                channel_ins[bp] = (unsigned char)al;
                do_volume(bp,ds,al);
              }
              if (cl != 0) {
                channel_note[bp] = (unsigned char)cl;
                channel_note2[bp] = (unsigned char)cl;
                if (_eff != 3) {
                  init_instrument2(ds, bp, channel_ins[bp]);
                  channel_vpos[bp] = 0;
                  play_note2(bp,cl|(ch<<8));
                }
              }
            }
            channel_eff[bp] = (unsigned short)(_eff | (_para << 8));
          }
          spec_arpeggio(ds,bp);
          play_line_effects(bp);
        }
      }
    }
  }

  for (int si = 0; si < 9; si++) {
    volatile unsigned char *ds = segments[si];
    if (ds != NULL && stopped[si] != 1) {
      s_count[si]++;
      if (s_count[si] >= current_speed[si]) {
        s_count[si] = 0;
        current_line[si]++;
        if (current_line[si]==64 || pattern_break[si]==1) {
          current_pos[si]++;
          unsigned int al = current_pos[si];
          if (current_pos[si] >= ds[_songlength]) {
            al = ds[_restart];
          }
          if (position_jump[si]!=0) {
            al = new_position[si];
          }
          current_pos[si] = (unsigned char)al;
          calc_work_pattern(si, ds, al);
          current_line[si] = break_pos[si];
          break_pos[si] = 0;
          pattern_break[si] = 0;
          position_jump[si] = 0;
        }
      }
    }
  }
}

void adlibTrackerCodeLockEnd() {
  inp(0x388);
}
unsigned int adlibTrackerDataLockEnd = 0;

#ifdef __WATCOMC__
bool ADLIB_LockMemory(void *address, long byteSize) {
  union REGS Regs;
  unsigned long linear = (unsigned long)address;
  Regs.w.ax = 0x600;
  Regs.w.bx = (unsigned short)(linear >> 16);
  Regs.w.cx = (unsigned short)(linear & 0xFFFF);
  Regs.w.si = (unsigned short)(byteSize >> 16);
  Regs.w.di = (unsigned short)(byteSize & 0xFFFF);
  int386(0x31, &Regs, &Regs);
  if (Regs.w.cflag)
    return false;
  return true;
}
bool ADLIB_UnlockMemory(void *address, long byteSize) {
  union REGS Regs;
  unsigned long linear = (unsigned long)address;
  Regs.w.ax = 0x601;
  Regs.w.bx = (unsigned short)(linear >> 16);
  Regs.w.cx = (unsigned short)(linear & 0xFFFF);
  Regs.w.si = (unsigned short)(byteSize >> 16);
  Regs.w.di = (unsigned short)(byteSize & 0xFFFF);
  int386(0x31, &Regs, &Regs);
  if (Regs.w.cflag)
    return false;
  return true;
}
#endif //__WATCOMC__

void adlibTrackerLock(void *data, unsigned long size) {
#ifdef __DJGPP__
  _go32_dpmi_lock_data((void*)&adlibTrackerDataLockStart,(long)&adlibTrackerDataLockEnd-(long)&adlibTrackerDataLockStart);
  _go32_dpmi_lock_code((void*)adlibTrackerCodeLockStart,(long)adlibTrackerCodeLockEnd-(long)adlibTrackerCodeLockStart);
  _go32_dpmi_lock_data(data,size);
#endif // __DJGPP__
#ifdef __WATCOMC__
  ADLIB_LockMemory((void*)&adlibTrackerDataLockStart,(long)&adlibTrackerDataLockEnd-(long)&adlibTrackerDataLockStart);
  ADLIB_LockMemory((void*)adlibTrackerCodeLockStart,(long)adlibTrackerCodeLockEnd-(long)adlibTrackerCodeLockStart);
  ADLIB_LockMemory(data,size);
#endif // __WATCOMC__
}
void adlibTrackerUnlock(void *data, unsigned long size) {
#ifdef __WATCOMC__
  ADLIB_UnlockMemory((void*)&adlibTrackerDataLockStart,(long)&adlibTrackerDataLockEnd-(long)&adlibTrackerDataLockStart);
  ADLIB_UnlockMemory((void*)adlibTrackerCodeLockStart,(long)adlibTrackerCodeLockEnd-(long)adlibTrackerCodeLockStart);
  ADLIB_UnlockMemory(data,size);
#endif // __WATCOMC__
}

#pragma on(check_stack)
