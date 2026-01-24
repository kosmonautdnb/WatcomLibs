//------------
// Simple Sound Blaster support.. Just the early Sound Blaster versions.
// E.g. for Soundblaster Pro 2.. But actually just for SBEMU.
// It's limited, if you want to use sound in your projects better take another library.
// This was just to have Sample Playback with the Sound Blaster emulation of SBEMU.EXE here, and it works..
// So to say it's a minimal example of sample playback with SBEMU.
// Currently not for DJGPP..
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
extern unsigned short SBL_SampleRate; // defaul 22050 (the Sound Blaster changes the frequency from this to a more "suitable" one)
extern unsigned short SBL_Dma; // default 1

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

// ---------

#endif //__SNDBLSTR_HPP__
