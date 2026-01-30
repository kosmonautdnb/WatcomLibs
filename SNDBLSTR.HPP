//------------
// Simple Sound Blaster support.. Just the early Sound Blaster versions.
// E.g. for Soundblaster Pro 2.. But actually just for SBEMU.
// It's limited, if you want to use sound in your projects better take another library.
// This was just to have Sample Playback with the Sound Blaster emulation of SBEMU.EXE here, and it works..
// So to say it's a minimal example of sample playback with SBEMU.
// Currently not for DJGPP..
// And currently missing all cli/sti, however maybe that is not a bigger problem.
//------------
#ifndef __SNDBLSTR_HPP__
#define __SNDBLSTR_HPP__

// ---------

// try this, it outputs a test tone. If it doesn't something may not be right.. As said, it's mainly for SBEMU.
// it's a good example on how to use this.
void soundBlasterTestFunction();

// ---------

extern unsigned short SBL_Port; // default 0x220
extern unsigned short SBL_Irq; // default 7 (values only 0..7)
extern unsigned short SBL_SampleRate; // default 22222 (actually the older Sound Blasters doesn't support 22050 look at SBL_SampleRateCorrected())
extern unsigned short SBL_Dma; // default 1

unsigned short SBL_SampleRateCorrected(unsigned short sampleRate); // The Sound Blaster standard doesn't support all thinkable frequencies

// ---------

typedef struct {
  unsigned short selector;
  unsigned short segment; 
  void *ptr;
} SBL_POINTER;

typedef void (*SBL_FillCallback)(unsigned char *buffer, unsigned int bufferLength, unsigned int playPos);

// ---------

bool SBL_Init(); // may be used to test if port is available (in range 0x210 to 0x280)
void SBL_Done();

unsigned char SBL_GetSampleVolume(); // %lll0rrr0 (l = left(0..7), r = right(0..7))
void SBL_SetSampleVolume(unsigned char data); // %lll0rrr0 (l = left(0..7), r = right(0..7))
void SBL_StartPlayback(SBL_POINTER *dosMemory, unsigned int dosMemorySize, unsigned int div, SBL_FillCallback callback);
void SBL_StopPlayback();

// ---------

SBL_POINTER SBL_Malloc(unsigned int size);
bool SBL_Free(const SBL_POINTER *mem);
bool SBL_LockMemory(void *address, unsigned int byteSize);
bool SBL_UnlockMemory(void *address, unsigned int byteSize);

#ifdef __WATCOMC__
#define SBL_WriteSample8(__8bitptr__,__index__,__8bitvalue__) {((unsigned char*)__8bitptr__)[__index__] = (unsigned char)(__8bitvalue__);}
#endif // __WATCOMC__

#ifdef __DJGPP__
#include <sys/movedata.h>
#define SBL_WriteSample8(__8bitptr__,__index__,__8bitvalue__) {unsigned char source = (unsigned char)(__8bitvalue__); dosmemput(&source,1,(unsigned int)(__8bitptr__)+__index__);}
#endif // __DJGPP__

// ---------

#endif //__SNDBLSTR_HPP__
