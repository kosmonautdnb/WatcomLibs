#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "SNDBLSTR.HPP"
#include "SA2.HPP"

volatile bool adlib_playSong = false;

// No timer interrupt speed manipulations, so it doesn't interfere with the DOS/BIOS harddrive stuff at all..

#pragma off(check_stack)
void soundBlasterCallback(unsigned char *buffer, unsigned int bufferLength, unsigned int playPos) {
  for (int i = 0; i < bufferLength; i++) SBL_WriteSample8(buffer, i, 0);
  if (adlib_playSong)
    play_line();
}
#pragma on(check_stack)

int main(int argc, const char *argv[]) {

  printf("\n");
  printf("Surprise!Productions\n");
  printf("Adlib Tracker 2.0\n");
  printf("SoundBlaster replayer...\n");
  printf("-------------------\n");
  if (argc != 2) {
    printf("Usage: APP_SA2.EXE <SA2FILE.SA2>\n");
    printf("in the source folder there should be a test.sa2\n");
    exit(0);
  }

  FILE *in = fopen(argv[1],"rb");
  if (in == NULL) {
    printf("File not found:<%s>\n",argv[1]);
    exit(0);
  }
  fseek(in,0,SEEK_END);
  long int fileSize = ftell(in);
  fseek(in,0,SEEK_SET);
  unsigned char *data = (unsigned char*)malloc(fileSize);
  fread(data,1,fileSize,in);
  fclose(in);

  adlib_playSong = false;
  SBL_LockMemory((void*)&adlib_playSong, 4);

  adlibTrackerLock(data,fileSize); 
  prepare_song(data);
  SBL_SampleRate = 22222;
  unsigned int bufferParts = 2;
  unsigned int bufferSize = SBL_SampleRate*60/(current_bpm*24)*bufferParts;

  SBL_Init();
  SBL_POINTER p = SBL_Malloc(bufferSize);
  SBL_StartPlayback( &p, bufferSize,bufferParts,soundBlasterCallback);
  SBL_SetSampleVolume(0x44); // we need to reduce sample volume on SBEMU to hear adlib

  adlib_playSong = true;
  while(!kbhit()) {
  }
  adlib_playSong = false;

  stop_all();
  SBL_Done();
  SBL_Free(&p);
  adlibTrackerUnlock(data,fileSize); free(data);

  SBL_UnlockMemory((void*)&adlib_playSong, 4);

  return 0;
}
