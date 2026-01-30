#include "SNDBLSTR.HPP"
#ifdef __WATCOMC__
#include <i86.h>
#endif // __WATCOMC__
#ifdef __DJGPP__
#include <dpmi.h>
#include <go32.h>
#endif // __DJGPP__
#include <dos.h>
#include <conio.h>
#include <stdio.h>

// ------------

#define __UNUSED(__x__) (void)(__x__)

// ------------

// SBL_Port + ...
#define SBL_LEFT_FM_SELECT_AND_STATUS 0x00
#define SBL_LEFT_FM_DATA 0x01
#define SBL_RIGHT_FM_SELECT_AND_STATUS 0x02
#define SBL_RIGHT_FM_DATA 0x03
#define SBL_MIXER_INDEX 0x04
#define SBL_MIXER_DATA 0x05
#define SBL_DSP_RESET 0x06
#define SBL_BOTH_FM_SELECT_AND_STATUS 0x08
#define SBL_BOTH_FM_DATA 0x09
#define SBL_DSP_READ_DATA 0x0A
#define SBL_DSP_DATA_AND_COMMAND_AND_STATUS 0x0C
#define SBL_TIMER_INTERRUPT_CLEAR 0x0D
#define SBL_DATA_STATE 0x0E
#define SBL_16BIT_VOICE_INTR_CLEAR 0x0F
#define SBL_CDROM_DATA 0x10
#define SBL_CDROM_COMMAND 0x11
#define SBL_CDROM_RESET 0x12
#define SBL_CDROM_ENABLE 0x13
#define SBL_FM_REGISTER_STATUS_PORT 0x338
#define SBL_FM_REGISTER_SELECT_PORT 0x339
#define SBL_ADVANCED_FM_REGISTER_SELECT_PORT 0x38B
#define SBL_ADVANCED_FM_REGISTER_DATA_PORT 0x38B

// to be sent to SBL_Port + SBL_DSP_DATA_AND_COMMAND_AND_STATUS
#define SBL_CMD_8BIT_OUTPUT_DIRECT 0x10
#define SBL_CMD_8BIT_OUTPUT_DMA 0x14
#define SBL_CMD_2BIT_COMPRESSED_SAMPLE_OUTPUT_DMA 0x16
#define SBL_CMD_2BIT_COMPRESSED_SAMPLE_OUTPUT_WITH_REFERENCE_BYTE_DMA 0x17
#define SBL_CMD_8BIT_OUTPUT_DMA_WITH_AUTOINIT 0x1c // not checked with the docs
#define SBL_CMD_DIRECT_RECORDING 0x20
#define SBL_CMD_8BIT_SAMPLE_RECORDING_DMA 0x24
#define SBL_CMD_DIRECT_MIDI_INPUT 0x30
#define SBL_CMD_MIDI_INPUT_VIA_INTERRUPT 0x31
#define SBL_CMD_DIRECT_MIDI_INPUT_WITH_TIMESTAMP 0x32
#define SBL_CMD_MIDI_INPUT_VIA_INTERRUPT_WITH_TIMESTAMP 0x33
#define SBL_CMD_MIDI_UART_DIRECT 0x34
#define SBL_CMD_MIDI_UART_VIA_INTERRUPT 0x35
#define SBL_CMD_MIDI_UART_VIA_INTERRUPT_WITH_TIMESTAMP 0x37
#define SBL_CMD_SEND_MIDI_CODE 0x38
#define SBL_CMD_SET_SAMPLE_RATE 0x40
#define SBL_CMD_DMA_MULTIBLOCK_CONTINUE_SB16 0x45
#define SBL_CMD_SET_BLOCKSIZE 0x48
#define SBL_CMD_4BIT_COMPRESSED_SAMPLE_OUTPUT_DMA 0x74
#define SBL_CMD_4BIT_COMPRESSED_SAMPLE_OUTPUT_WITH_REFERENCE_BYTE_DMA 0x75
#define SBL_CMD_2POINT6BIT_COMPRESSED_SAMPLE_OUTPUT_DMA 0x76
#define SBL_CMD_2POINT6BIT_COMPRESSED_SAMPLE_OUTPUT_WITH_REFERENCE_BYTE_DMA 0x77
#define SBL_CMD_DEFINE_MUTED_BLOCK 0x80
#define SBL_CMD_8BIT_OUTPUT_VIA_DMA_HIGHSPEED 0x91
#define SBL_CMD_8BIT_INPUT_VIA_DMA_HIGHSPEED 0x99
#define SBL_CMD_16BIT_DATA_OUTPUT_SB16 0xB6
#define SBL_CMD_16BIT_DATA_INPUT_SB16 0xB9
#define SBL_CMD_8BIT_DATA_OUTPUT_SB16 0xC6
#define SBL_CMD_8BIT_DATA_INPUT_SB16 0xC9
#define SBL_CMD_STOP_DMA 0xD0
#define SBL_CMD_ENABLE_SPEAKER 0xD1
#define SBL_CMD_DISABLE_SPEAKER 0xD3
#define SBL_CMD_CONTINUE_DMA 0xD4
#define SBL_CMD_REQUEST_SPEAKER_CONFIGURATION 0xD8
#define SBL_CMD_GET_VERSION 0xE1

// to be sent to SBL_Port + SBL_MIXER_INDEX  (from SBPro onwards.)
#define SBL_MIX_RESET 0x00
#define SBL_MIX_DSP_VOLUME 0x02
#define SBL_MIX_DSP_VOLUME_SBPRO 0x04 // not verfied
#define SBL_MIX_MIC_VOLUME 0x0A
#define SBL_MIX_FILTER_INPUT 0x0C
#define SBL_MIX_FILTER_OUTPUT_AND_STEREO 0x0E
#define SBL_MIX_MASTER_VOLUME 0x22
#define SBL_MIX_FM_VOLUME 0x26
#define SBL_MIX_CD_VOLUME 0x28
#define SBL_MIX_LINE_VOLUME 0x2E
#define SBL_MIX16_LEFT_MASTER_VOLUME 48
#define SBL_MIX16_RIGHT_MASTER_VOLUME 49
#define SBL_MIX16_LEFT_VOICE_VOLUME 50
#define SBL_MIX16_RIGHT_VOICE_VOLUME 51
#define SBL_MIX16_LEFT_MIDI_VOLUME 52
#define SBL_MIX16_RIGHT_MIDI_VOLUME 53
#define SBL_MIX16_LEFT_CD_VOLUME 54
#define SBL_MIX16_RIGHT_CD_VOLUME 55
#define SBL_MIX16_LEFT_LINE_VOLUME 56
#define SBL_MIX16_RIGHT_LINE_VOLUME 57
#define SBL_MIX16_MIC_VOLUME 58
#define SBL_MIX16_PCSPEAKER_VOLUME 59
#define SBL_MIX16_OUTPUT_CONTROL 60
#define SBL_MIX16_LEFT_INPUT_CONTROL 61
#define SBL_MIX16_RIGHT_INPUT_CONTROL 62
#define SBL_MIX16_LEFT_INPUT_GAIN 63
#define SBL_MIX16_RIGHT_INPUT_GAIN 64
#define SBL_MIX16_LEFT_OUTPUT_GAIN 65
#define SBL_MIX16_RIGHT_OUTPUT_GAIN 66
#define SBL_MIX16_AUTOMATIC_GAIN_CONTROL_ON_OFF 67
#define SBL_MIX16_LEFT_TREBLE 68
#define SBL_MIX16_RIGHT_TREBLE 69
#define SBL_MIX16_LEFT_BASS 70
#define SBL_MIX16_RIGHT_BASS 71

// ------------

unsigned short SBL_Port = 0x220;
unsigned short SBL_Irq = 7; // has to be below 8 currently
unsigned short SBL_SampleRate = 22222;
unsigned short SBL_Dma = 1;

// ------------

unsigned int SBL_IsPlaying = 0;
unsigned int SBL_PortionCount = 0;
unsigned int SBL_PortionSize = 0;
unsigned int SBL_PortionPlaying = 0;
unsigned char *SBL_PortionOutput = NULL;
SBL_FillCallback SBL_Callback = NULL;

// ------------

unsigned char SBL_InterruptController1Mask = 0x00;
unsigned char SBL_InterruptController2Mask = 0x00;
unsigned char SBL_InitialVolume = 0x00;
const int SBL_Interrupts[]  = {-1,-1,0xA,0xB,-1,0xD,-1,0xF,-1,-1,0x72,0x73,0x74,-1,-1,0x77};
unsigned short SBL_Interrupt = 0;

// ------------

#define SBL_ServiceInterrupt_length 4095
void SBL_InterruptPart() {
  if (SBL_IsPlaying) {
    SBL_PortionPlaying++;
    if (SBL_Callback != NULL) {
      int fillUpPortion = (SBL_PortionPlaying+1)%SBL_PortionCount;
      unsigned char *writeTo = SBL_PortionOutput+SBL_PortionSize*fillUpPortion;
      SBL_Callback(writeTo,SBL_PortionSize,SBL_PortionSize*(SBL_PortionPlaying+1));
    }
  }
  inp(SBL_Port + SBL_DATA_STATE);
}

#ifdef __WATCOMC__
typedef void (__interrupt __far* SBL_InterruptHandler)();
SBL_InterruptHandler SBL_OldInterruptVector = NULL;
void __interrupt __far SBL_ServiceInterrupt() {
  SBL_InterruptPart();
  if (SBL_Irq > 7) outp(0xA0,0x20);
  outp(0x20,0x20);
}
#endif // __WATCOMC__

#ifdef __DJGPP__
static _go32_dpmi_seginfo SBL_old_handler;
static _go32_dpmi_seginfo SBL_new_handler;
void SBL_ServiceInterrupt() {
  SBL_InterruptPart();
  if (SBL_Irq > 7) outp(0xA0,0x20);
  outp(0x20,0x20);
}
#endif // __DJGPP

// ------------

void SBL_Wait() {for (int i = 0; i < 100; i++) inp(0x80);}

void SBL_WritePort(unsigned short reg, unsigned char val) { outp(SBL_Port+reg,val);}

unsigned char SBL_ReadPort(unsigned short reg) { return (unsigned char)inp((unsigned short)SBL_Port+reg); }

void SBL_WriteMixer(unsigned char index, unsigned char data) {SBL_WritePort(SBL_MIXER_INDEX,index);  SBL_WritePort(SBL_MIXER_DATA,data);}

unsigned char SBL_ReadMixer(unsigned char index) { SBL_WritePort(SBL_MIXER_INDEX,index); return SBL_ReadPort(SBL_MIXER_DATA);}

void SBL_WriteDSP(unsigned char value) {while(SBL_ReadPort(SBL_DSP_DATA_AND_COMMAND_AND_STATUS) & 128) {;} SBL_WritePort(SBL_DSP_DATA_AND_COMMAND_AND_STATUS,value);}

#ifdef __WATCOMC__

bool SBL_LockMemory(void *address, unsigned int byteSize) {
  union REGS Regs;
  unsigned int linear = (unsigned int)address;
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

bool SBL_UnlockMemory(void *address, unsigned int byteSize) {
  union REGS Regs;
  unsigned int linear = (unsigned int)address;
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

#endif // __WATCOMC__

#ifdef __DJGPP__

bool SBL_LockMemory(void *address, unsigned int byteSize) {
  _go32_dpmi_lock_data(address,byteSize);
  // missing _go32_dpmi_lock_code();
  return true;
}

bool SBL_UnlockMemory(void *address, unsigned int byteSize) {
  __UNUSED(address);
  __UNUSED(byteSize);
  // missing
  return true;
}

#endif // __DJGPP__

#define SBL_Lock(__var__) SBL_LockMemory(&(__var__),sizeof(__var__))

#define SBL_LockFunction(__func__, __size__) SBL_LockMemory((void*)__func__,__size__)

#define SBL_LockRegion(__start__, __end__) SBL_LockMemory(&(__start__),(unsigned int)(__end__)-(unsigned int)(__start__))

#define SBL_Unlock(__var__) SBL_UnlockMemory(&(__var__),sizeof(__var__))

#define SBL_UnlockFunction(__func__, __size__) SBL_UnlockMemory((void*)__func__,__size__)

#define SBL_UnlockRegion(__start__, __end__) SBL_UnlockMemory(&(__start__),(unsigned int)(__end__)-(unsigned int)(__start__))

void SBL_DisableInterrupt() {
  if (SBL_Irq < 8) {
    unsigned char mask = (unsigned char)(inp(0x21) & ~(1 << SBL_Irq));
    mask |= (unsigned char)(SBL_InterruptController1Mask & (1 << SBL_Irq));
    outp(0x21, mask);
  }
  else
  {
    unsigned char mask = (unsigned char)(inp(0x21) & ~(1 << 2));
    mask |= (unsigned char)(SBL_InterruptController1Mask & (1 << 2));
    outp(0x21, mask);
    mask = (unsigned char)(inp(0xA1) & ~(1 << (SBL_Irq - 8)));
    mask |= (unsigned char)(SBL_InterruptController2Mask & (1 << (SBL_Irq - 8)));
    outp(0xA1, mask);
  }
}

void SBL_EnableInterrupt() {
  if (SBL_Irq < 8)
  {
    unsigned char mask = (unsigned char)(inp(0x21) & ~(1 << SBL_Irq));
    outp(0x21, mask);
  }
  else
  {
    unsigned char mask = (unsigned char)(inp(0xA1) & ~(1 << (SBL_Irq - 8)));
    outp( 0xA1, mask);
    mask = (unsigned char)(inp(0x21) & ~(1 << 2));
    outp(0x21, mask);
  }
}

SBL_POINTER SBL_NullPointer() {SBL_POINTER k; k.selector = 0; k.segment = 0; k.ptr = NULL; return k;}

SBL_POINTER SBL_Pointer(unsigned short selector, unsigned short segment) {SBL_POINTER k; k.selector = selector; k.segment = segment; k.ptr = (void*)((unsigned int)segment * 16); return k;}

SBL_POINTER SBL_Malloc(unsigned int size) {
  if (size>0x10000) return SBL_NullPointer();
  union REGS regs;
  regs.w.ax = 0x100;
  regs.w.bx = (unsigned short)((size*2+15)/16); // size is *2 here
  regs.w.cflag = false;
  int386(0x31, &regs, &regs);
  if (regs.w.cflag) {
    return SBL_NullPointer();
  }
  SBL_POINTER b = SBL_Pointer(regs.w.dx,regs.w.ax); // dma should not cross 64k boundaries
  unsigned int a = (unsigned int)b.ptr;
  unsigned int c = a + size;
  if ((c>>16)>(a>>16)) {
    b.ptr = (void*)c;
  }
  return b;
}

bool SBL_Free(const SBL_POINTER *mem) {
  union REGS regs;
  regs.w.ax = 0x101;
  regs.w.dx = mem->selector;
  regs.w.cflag = false;
  int386(0x31, &regs, &regs);
  if (regs.w.cflag) {
    return false;
  }
  return true;
}

static unsigned short SBL_dma_single_mask[8] = {0x0A,0x0A,0x0A,0x0A,0xD4,0xD4,0xD4,0xD4};
static unsigned short SBL_dma_clear_flipflop[8] = {0x0C,0x0C,0x0C,0x0C,0xD8,0xD8,0xD8,0xD8};
static unsigned short SBL_dma_adr[8] = {0x00,0x02,0x04,0x06,0xc0,0xC4,0xC8,0xCC};
static unsigned short SBL_dma_length[8] = {0x01,0x03,0x05,0x07,0xC2,0xC6,0xCA,0xCE};
static unsigned short SBL_dma_pagelo[8] = {0x87,0x83,0x81,0x82,0x00,0x8B,0x89,0x8A};
//static unsigned short SBL_dma_pagehi[8] = {0x487,0x483,0x481,0x482,0x00,0x48B,0x489,0x48A};
static unsigned short SBL_dma_write_mode[8] = {0x0B,0x0B,0x0B,0x0B,0xD6,0xD6,0xD6,0xD6};

void SBL_EndDMA() {
  outp(SBL_dma_single_mask[SBL_Dma], (SBL_Dma & 0x03) | 4);
  outp(SBL_dma_clear_flipflop[SBL_Dma], 0);
}

void SBL_StartDMA(void *address, unsigned int byteSize) {
  unsigned int offs = (unsigned int)address;
  byteSize--;
  outp(SBL_dma_single_mask[SBL_Dma], (SBL_Dma & 0x3) | 4);
  outp(SBL_dma_clear_flipflop[SBL_Dma], 0);
  outp(SBL_dma_write_mode[SBL_Dma],SBL_Dma | 0x58);
  outp(SBL_dma_adr[SBL_Dma], offs & 255);
  outp(SBL_dma_adr[SBL_Dma], (offs>>8) & 255);
  outp(SBL_dma_pagelo[SBL_Dma], (offs>>16) & 255);
  outp(SBL_dma_length[SBL_Dma], byteSize & 255);
  outp(SBL_dma_length[SBL_Dma], byteSize>>8);
  outp(SBL_dma_single_mask[SBL_Dma], (SBL_Dma & 0x3));
}

// ------------

bool SBL_Reset() {
  SBL_WritePort(SBL_DSP_RESET,1);  SBL_Wait();
  SBL_WritePort(SBL_DSP_RESET,0);  SBL_Wait();
  for (int i = 0; i < 100; i++) {
    int stat = SBL_ReadPort(SBL_DATA_STATE);
    stat = SBL_ReadPort(SBL_DSP_READ_DATA);
    if (stat == 0xAA) return true;
    SBL_Wait();
  }
  return false;
}

unsigned char SBL_GetSampleVolume() {return SBL_ReadMixer(SBL_MIX_DSP_VOLUME_SBPRO);}
void SBL_SetSampleVolume(unsigned char data) {SBL_WriteMixer(SBL_MIX_DSP_VOLUME_SBPRO,data);}

void SBL_SetSampleRate(int freq) {
  int tc = 256-(1000000 / freq);
  SBL_WriteDSP(SBL_CMD_SET_SAMPLE_RATE);
  SBL_WriteDSP((unsigned char)tc);
}

unsigned short SBL_SampleRateCorrected(unsigned short sampleRate) {
  int tc = 256-(1000000 / sampleRate);
  return (unsigned short)(1000000/-(tc-256));
}

void SBL_StopPlayback() {
  SBL_SetSampleVolume(0x00);
  SBL_WriteDSP(SBL_CMD_DISABLE_SPEAKER);
  SBL_IsPlaying = 0;
  SBL_Callback = NULL;
  SBL_DisableInterrupt();
  SBL_WriteDSP(SBL_CMD_STOP_DMA);
  SBL_EndDMA();
  SBL_WriteDSP(SBL_CMD_DISABLE_SPEAKER);
}

void SBL_StartPlayback(SBL_POINTER *dosMemory, unsigned int dosMemorySize, unsigned int div, SBL_FillCallback callback) {
  SBL_StopPlayback();
  SBL_SetSampleVolume(0xEE);
  if ((dosMemorySize % div)!=0) {
    return;
  }
  SBL_Callback = callback;
  SBL_WriteMixer(SBL_MIX_FILTER_OUTPUT_AND_STEREO,(unsigned char)(SBL_ReadMixer(SBL_MIX_FILTER_OUTPUT_AND_STEREO) & (~2))); // SBPro mono
  SBL_StartDMA(dosMemory->ptr, dosMemorySize);
  SBL_SetSampleRate(SBL_SampleRate);
  SBL_WriteDSP(SBL_CMD_ENABLE_SPEAKER);
  unsigned int oneSpan = dosMemorySize / div;
  SBL_PortionCount = div;
  SBL_PortionSize = oneSpan;
  SBL_PortionPlaying = 0;
  SBL_PortionOutput = (unsigned char*)dosMemory->ptr;
  oneSpan--;
  SBL_WriteDSP(SBL_CMD_SET_BLOCKSIZE);
  SBL_WriteDSP((unsigned char)(oneSpan & 255));
  SBL_WriteDSP((unsigned char)((oneSpan>>8) & 255));
  SBL_IsPlaying = 1;
  SBL_Callback(SBL_PortionOutput,SBL_PortionSize,0);
  if (div > 1)
    SBL_Callback(SBL_PortionOutput+SBL_PortionSize,SBL_PortionSize,SBL_PortionSize);
  SBL_WriteDSP(SBL_CMD_8BIT_OUTPUT_DMA_WITH_AUTOINIT);
}

void SBL_LockInterruptMemory() {
  SBL_LockFunction( SBL_ServiceInterrupt, SBL_ServiceInterrupt_length);
  SBL_LockFunction( SBL_InterruptPart, SBL_ServiceInterrupt_length);
  SBL_Lock(SBL_Port);
  SBL_Lock(SBL_Irq);
  SBL_Lock(SBL_IsPlaying);
  SBL_Lock(SBL_PortionCount);
  SBL_Lock(SBL_PortionSize);
  SBL_Lock(SBL_PortionPlaying);
  SBL_Lock(SBL_PortionOutput);
  SBL_Lock(SBL_Callback);
  SBL_Lock(SBL_SampleRate);
}

void SBL_UnlockInterruptMemory() {
  SBL_UnlockFunction( SBL_ServiceInterrupt, SBL_ServiceInterrupt_length);
  SBL_UnlockFunction( SBL_InterruptPart, SBL_ServiceInterrupt_length);
  SBL_Unlock(SBL_Port);
  SBL_Unlock(SBL_Irq);
  SBL_Unlock(SBL_IsPlaying);
  SBL_Unlock(SBL_PortionCount);
  SBL_Unlock(SBL_PortionSize);
  SBL_Unlock(SBL_PortionPlaying);
  SBL_Unlock(SBL_PortionOutput);
  SBL_Unlock(SBL_Callback);
  SBL_Unlock(SBL_SampleRate);
}


bool SBL_Init() {
  SBL_InterruptController1Mask = (unsigned char)inp(0x21);
  SBL_InterruptController2Mask = (unsigned char)inp(0xA1);
  if (!SBL_Reset())  return false;
  SBL_InitialVolume = SBL_GetSampleVolume();   
  SBL_WriteMixer(SBL_MIX_FILTER_OUTPUT_AND_STEREO,(unsigned char)(SBL_ReadMixer(SBL_MIX_FILTER_OUTPUT_AND_STEREO) & (~2))); // SBPro mono
  SBL_SetSampleRate(SBL_SampleRate);
  SBL_Interrupt = (unsigned short)SBL_Interrupts[SBL_Irq]; // attention here maybe -1
  SBL_LockInterruptMemory();
#ifdef __WATCOMC__
  SBL_OldInterruptVector = _dos_getvect(SBL_Interrupt);
  _dos_setvect(SBL_Interrupt, SBL_ServiceInterrupt);
#endif // __WATCOMC__
#ifdef __DJGPP__
  _go32_dpmi_get_protected_mode_interrupt_vector(SBL_Interrupt,&SBL_old_handler); 
  SBL_new_handler.pm_offset = (unsigned long)SBL_ServiceInterrupt;
  SBL_new_handler.pm_selector = _go32_my_cs();
  _go32_dpmi_chain_protected_mode_interrupt_vector(SBL_Interrupt,&SBL_new_handler);
#endif // __DJGPP__
  return true;
}

void SBL_Done() {
  SBL_StopPlayback();
  SBL_SetSampleVolume(SBL_InitialVolume);
  SBL_Reset();
#ifdef __WATCOMC__
  _dos_setvect(SBL_Interrupt, SBL_OldInterruptVector);
#endif // __WATCOMC__
#ifdef __DJGPP__
  _go32_dpmi_set_protected_mode_interrupt_vector(SBL_Interrupt,&SBL_old_handler);
#endif // __DJGPP__
  SBL_UnlockInterruptMemory();
}

// ------------

void SBL_TestCallback(unsigned char *buffer, unsigned int bufferSize, unsigned int playPos) {
  for (unsigned int i = 0; i < bufferSize; i++) {
    SBL_WriteSample8(buffer,i,(playPos/100) & 1 ? 96 : 16);
    playPos++;
  }
}

// ------------

void soundBlasterTestFunction() {
  SBL_Init();

  unsigned int size = 16384;
  SBL_POINTER p = SBL_Malloc(size);
  SBL_LockFunction(SBL_TestCallback,1024);
  SBL_StartPlayback( &p, size,2,SBL_TestCallback);

  printf("Press a key to stop playback.\n");
  getch();

  SBL_Done();
  SBL_Free(&p);
  SBL_UnlockFunction(SBL_TestCallback,1024);
}
