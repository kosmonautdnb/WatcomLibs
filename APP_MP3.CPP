#include "SNDBLSTR.HPP"
#define MINIMP3_IMPLEMENTATION
#include "MINIMP3.HPP"
#include <stdio.h>
#include <conio.h>

short samples[MINIMP3_MAX_SAMPLES_PER_FRAME];

#define BUFFERSIZE (5000)
#define RINGBUFFERSIZE (BUFFERSIZE*2)
volatile unsigned char ringBuffer[RINGBUFFERSIZE];
volatile int destPos = 0;
volatile int endPos = 0;

void mp3DecodeCallback(unsigned char *buffer, unsigned int bufferLength, unsigned int playPos) {
  for (int i = 0; i < bufferLength; i++) {
    if (endPos != 0 && destPos>=endPos) {buffer[i]=0; continue;}
    SBL_WriteSample8(buffer, i, ringBuffer[destPos % RINGBUFFERSIZE]);
    destPos++;
  }
}

int main(int argc, const char *argv[]) {

  printf("\n");
  printf("SBEMU MP3 Player\n");
  printf("-------------------\n");
  if (argc != 2) {
    printf("Usage: APP_MP3.EXE <MP3FILE.MP3>\n");
    exit(0);
  }
  const char *fileName = argv[1];

  FILE *in = fopen(fileName,"rb");

  if (in == NULL) {
    printf("MP3<%s> not found.\n",fileName);
    exit(0);
  }

  fseek(in,0,SEEK_END);
  int fileSize = ftell(in);
  fseek(in,0,SEEK_SET);
  unsigned char *fileData = (unsigned char*)malloc(fileSize);
  printf("Loading MP3 <%s> with %dMb.\n", fileName,fileSize/1024/1024);
  fread(fileData,1,fileSize,in);
  fclose(in);                                                     

  SBL_LockMemory((void*)mp3DecodeCallback, 1024);
  SBL_LockMemory((void*)ringBuffer, sizeof(ringBuffer));
  SBL_LockMemory((void*)&destPos, sizeof(int));
  SBL_LockMemory((void*)&endPos, sizeof(int));

  mp3dec_t decoder;
  mp3dec_frame_info_t info;
  mp3dec_init(&decoder);
  double sourcePos = 0;
  destPos = 0;
  endPos = 0;
  memset((void*)ringBuffer,0,sizeof(ringBuffer));
  unsigned char *mp3 = fileData;
  int mp3_bytes = fileSize;

  if (!SBL_Init()) {
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
  SBL_StartPlayback( &p, BUFFERSIZE,2,mp3DecodeCallback);

  int soundBlasterHz = SBL_SampleRateCorrected(SBL_SampleRate);
  int mp3Hz = -1;
  int mp3Channels = -1;
  printf("Sound Blaster Frequency: %d\n",soundBlasterHz);

  printf("Press a key to stop playback.\n");
  while(!kbhit()) {
    int sampleCount = mp3dec_decode_frame(&decoder, mp3, mp3_bytes, samples, &info);
    if (sampleCount==0&&info.frame_bytes==0) endPos = sourcePos;
    if (info.hz != mp3Hz) {mp3Hz = info.hz; mp3Channels = info.channels; printf("MP3 Frequency: %d\n",mp3Hz); printf("MP3 Channels: %d\n",mp3Channels);}
    mp3 += info.frame_bytes;
    mp3_bytes -= info.frame_bytes;
    if (mp3Channels > 0 && mp3Hz > 0) {
      const double rate = (double)soundBlasterHz/mp3Hz;
      if (rate > 1.0) {printf("MP3 Sample Rate to low.\n"); break;}
      while((int)(sourcePos+sampleCount*rate)>destPos) {;}
      int k = 0; int a = 0; int w = 0; int l = 0;
      for (int i = 0; i < sampleCount; i++) {
        for (int j = 0; j < mp3Channels; j++) {a += samples[k++];w++;}
        if ((int)sourcePos != l) {l = (int)sourcePos;ringBuffer[((int)sourcePos) % RINGBUFFERSIZE] = (a/w+32768)>>8; w = 0; a = 0;}
        sourcePos += rate;
      }
    }
  }
  endPos = 1; // mute
  SBL_Done();
  SBL_Free(&p);

  SBL_UnlockMemory((void*)mp3DecodeCallback, 1024);
  SBL_UnlockMemory((void*)ringBuffer, sizeof(ringBuffer));
  SBL_UnlockMemory((void*)&destPos, sizeof(int));
  SBL_UnlockMemory((void*)&endPos, sizeof(int));

  free(fileData);
  return 0;
}
