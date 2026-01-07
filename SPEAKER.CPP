#include "speaker.hpp"
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#ifdef __WATCOMC__
#include <i86.h>
#include <dos.h>
#endif // __WATCOMC__
#ifdef __DJGPP__
// todo: not implemented, yet
#include <dos.h>
#endif // __DJGPP__

#ifdef __WATCOMC__
#pragma aux speakerCLI = "cli";
#pragma aux speakerSTI = "sti";
#endif // __WATCOMC__

#ifdef __DJGPP__
void speakerCLI() {
  __asm__ __volatile__("cli");
}
void speakerSTI() {
  __asm__ __volatile__("sti");
}
#endif // __WATCOMC__

void speaker_tone_freq(double hz) {
  outp(0x43,0xb6); // set speaker timer 2 to square wave
  int a = (int)(1193180 / hz);
  speakerCLI();
  outp(0x42,a & 255);
  outp(0x42,(a>>8) & 255);
  speakerSTI();
}

void speaker_tone_on() {
  unsigned char p61 = inp(0x61);
  p61 |= 0x03; // bit 0 = timer 2 output to speaker, bit 1 = speaker data on
  outp(0x61,p61);
}

void speaker_tone_off() {
  unsigned char p61 = inp(0x61);
  p61 &= 255 - 0x03; // bit 0 = timer 2 output to speaker, bit 1 = speaker data on
  outp(0x61,p61);
}


#ifdef __WATCOMC__
#define HARDWARE_TIMER_FREQUENCY 1193180 // 1193180 / 65536  = 18.2 (found in the internet that it's more like 1193181.66666666 instead of 1193180)
#define TIMER_MULTIPLICATOR 1024 // 1193180 * 1024 / 65536
double speakerFrequency = (double)HARDWARE_TIMER_FREQUENCY/65536.0*TIMER_MULTIPLICATOR;

typedef void (__interrupt __far* TimerIrqHandler)();
static TimerIrqHandler oldTimerIrqHandler;

volatile bool playSampleOn = false;
volatile unsigned char *bitSample = NULL;
volatile int bitSamplePos = 0;
volatile int bitSampleLength = 0;
volatile int timerIrqPos = 0;
volatile bool loopSample = true;
volatile double speakerSeconds = 0;
volatile bool muteSpeaker = false;
volatile bool mode8Bit = false;

static void initSample() {
  mode8Bit = false;
  playSampleOn = false;
  bitSample = NULL;
  bitSamplePos = 0;
  bitSampleLength = 0;
  loopSample = true;
}

void playSample(unsigned char *data, int len, bool loop) {
  if (mode8Bit) 
    return;
  initSample();
  bitSample = data;
  bitSampleLength = len;
  loopSample = loop;
  playSampleOn = true;
}

void stopSample() {
  if (!mode8Bit) playSampleOn = false;
}

int samplePosition() {
  return bitSamplePos;
}

int sampleLength() {
  return bitSampleLength;
}

unsigned char *convertSampleFromUInt8(unsigned char *data, int len) {
  unsigned char *data2 = new unsigned char[(len+7)/8];
  memset(data2,0,(len+7)/8);
  for (int i = 0; i < len-1; i++) {
    int k0 = data[i];
    int k1 = data[i+1];
    int k = k1 > k0 ? 1 : 0;
    int i2 = i>>3;
    int i3 = 1<<(i & 7);
    data2[i2]|=k*i3;
  }
  return data2;
}

unsigned char *convertSampleFromSInt8(char *data, int len) {
  unsigned char *data2 = new unsigned char[(len+7)/8];
  memset(data2,0,(len+7)/8);
  for (int i = 0; i < len-1; i++) {
    int k0 = data[i];
    int k1 = data[i+1];
    int k = k1 > k0 ? 1 : 0;
    int i2 = i>>3;
    int i3 = 1<<(i & 7);
    data2[i2]|=k*i3;
  }
  return data2;
}

unsigned char *convertSampleFromUInt16(unsigned short *data, int len) {
  unsigned char *data2 = new unsigned char[(len+7)/8];
  memset(data2,0,(len+7)/8);
  for (int i = 0; i < len-1; i++) {
    int k0 = data[i];
    int k1 = data[i+1];
    int k = k1 > k0 ? 1 : 0;
    int i2 = i>>3;
    int i3 = 1<<(i & 7);
    data2[i2]|=k*i3;
  }
  return data2;
}

unsigned char *convertSampleFromSInt16(short *data, int len) {
  unsigned char *data2 = new unsigned char[(len+7)/8];
  memset(data2,0,(len+7)/8);
  for (int i = 0; i < len-1; i++) {
    int k0 = data[i];
    int k1 = data[i+1];
    int k = k1 > k0 ? 1 : 0;
    int i2 = i>>3;
    int i3 = 1<<(i & 7);
    data2[i2]|=k*i3;
  }
  return data2;
}
                  
static void (__interrupt __far timerIrqHandler) (void)
{
  if (bitSample!=NULL&&bitSampleLength!=0&&playSampleOn) {
    int a = inp(0x61) & (~3); // bit 0 = connect to PIT, bit 1 = ON/OFF
    int k = 0;
    if (!mode8Bit) {
      // direct
      k = bitSample[bitSamplePos>>3]&(1<<(bitSamplePos&7)) ? 2 : 0;
      bitSamplePos++;
      if (bitSampleLength>0) bitSamplePos %= bitSampleLength;
      if (!loopSample) if (bitSamplePos==0) playSampleOn = false;
    } else {
      // mode8Bit (with looping audio with mixing)
      int k0 = ((signed int*)bitSample)[(bitSamplePos - 1 + bitSampleLength) % bitSampleLength];
      ((signed int*)bitSample)[(bitSamplePos - 1 + bitSampleLength) % bitSampleLength]=0;      
      int k1 = ((signed int*)bitSample)[bitSamplePos % bitSampleLength];
      bitSamplePos++;
      k = k1>k0?2:0;
    }
    if (!muteSpeaker)
      outp(0x61,a|k);
  }

  speakerSeconds += (double)1.0/((double)HARDWARE_TIMER_FREQUENCY * TIMER_MULTIPLICATOR / 65536.0);
  timerIrqPos++;
  if (timerIrqPos>=TIMER_MULTIPLICATOR) {
    timerIrqPos = 0;
    oldTimerIrqHandler();
  } else {
    outp(0x20,0x20);
  }
}

void enableSamplePlayback() {

  initSample();
  speakerSeconds = 0;
  timerIrqPos = 0;

  oldTimerIrqHandler = _dos_getvect(0x08);
  int here = 65536/TIMER_MULTIPLICATOR;
  _dos_setvect(0x08,timerIrqHandler);

  outp(0x43,0x36);
  speakerCLI();
  outp(0x40,here & 255);
  outp(0x40,(here>>8) & 255);
  speakerSTI();
}

void disableSamplePlayback() {

  initSample();
  outp(0x43,0x36);
  speakerCLI();
  outp(0x40,0);
  outp(0x40,0);
  speakerSTI();
  _dos_setvect(0x08,oldTimerIrqHandler);
}

void enable32BitSignedSamplePlayback(signed int *audioBuffer, int len) {
  enableSamplePlayback();
  memset(audioBuffer,0,len*sizeof(signed int*));
  mode8Bit = true;
  bitSampleLength = len;
  bitSample = (unsigned char*)audioBuffer;
  playSampleOn = true;
}

void playSample32BitSigned(signed int *data, int len, double volume) {
  if (!mode8Bit) 
    return;
  if (bitSample==NULL|bitSampleLength<=0) return;
  int pos = bitSamplePos+100; // play a tiny bit in the future
  for (int i = 0; i < len; i++) {
    pos %= bitSampleLength;
    int a = ((signed int*)bitSample)[pos];
    a += data[i]*volume;
    ((signed int*)bitSample)[pos] = a;
    pos++;
  }
}

#endif // __WATCOMC__

#ifdef __DJGPP__

double speakerFrequency = 0;

volatile bool playSampleOn = false;
volatile unsigned char *bitSample = NULL;
volatile int bitSamplePos = 0;
volatile int bitSampleLength = 0;
volatile int timerIrqPos = 0;
volatile bool loopSample = true;
volatile double speakerSeconds = 0; // important
volatile bool muteSpeaker = false;
volatile bool mode8Bit = false;

static void initSample() {
}

void playSample(unsigned char *data, int len, bool loop) {
}

void stopSample() {
}

int samplePosition() {
  return 0;
}

int sampleLength() {
  return 0;
}

unsigned char *convertSampleFromUInt8(unsigned char *data, int len) {
  return NULL;
}

unsigned char *convertSampleFromSInt8(char *data, int len) {
  return NULL;
}

unsigned char *convertSampleFromUInt16(unsigned short *data, int len) {
  return NULL;
}

unsigned char *convertSampleFromSInt16(short *data, int len) {
  return NULL;
}
                  
void enableSamplePlayback() {
}

void disableSamplePlayback() {
}

void enable32BitSignedSamplePlayback(signed int *audioBuffer, int len) {
}

void playSample32BitSigned(signed int *data, int len, double volume) {
}

#endif // __DJGPP__


