#include "GL.H"
#include "MPEG1.HPP"
#include "SNDBLSTR.HPP"
#include <stdlib.h>
#include <stdio.h>

#define SBEMUSOUNDBLASTERON

#ifdef SBEMUSOUNDBLASTERON
#define AUDIO_RINGBUFFERSIZE (MPEG1_AUDIO_SAMPLES_PER_FRAME*256)
#define SOUNDBLASTERMEMSIZE (1024) // also used for glSeconds() via glSetTime() so finer granularity = better

volatile float audioRingBuffer[AUDIO_RINGBUFFERSIZE];
volatile int plmSampleRate = 44100; // gets updated later
volatile unsigned int audioRingBufferPos = 0;
volatile double soundBlasterRingBufferPos = 0;
volatile unsigned int soundBlasterPlayTime = 0;
volatile int soundBlasterTicks = 0;

void audioCallback(float *stereoSamples, int sampleCount, int sampleRate, double time, void *userPointer) {
  plmSampleRate = sampleRate;
  audioRingBufferPos = (unsigned int)(time*sampleRate);
  for (int i = 0; i < sampleCount; i++) {
    audioRingBuffer[audioRingBufferPos%AUDIO_RINGBUFFERSIZE] = (stereoSamples[i*2+0]+stereoSamples[i*2+1])*0.5;
    audioRingBufferPos++;
  }
}

void soundBlasterCallback(unsigned char *buffer, unsigned int bufferLength, unsigned int playPos) {
  soundBlasterTicks++;
  soundBlasterPlayTime += bufferLength;
  static unsigned char sample = 0;
  for (int i = 0; i < bufferLength; i++) {
    const float s = audioRingBuffer[((unsigned int)soundBlasterRingBufferPos)%AUDIO_RINGBUFFERSIZE];
    if (soundBlasterRingBufferPos < audioRingBufferPos) {
      sample = (unsigned char)(s * 127.0+128.0);
      SBL_WriteSample8(buffer,i,sample);
      soundBlasterRingBufferPos += (double)plmSampleRate/SBL_SampleRate;
    } else {
      SBL_WriteSample8(buffer,i,sample);
    }
  }
}

void waitSoundBlasterStart() {
  int a = soundBlasterTicks;
  while(a == soundBlasterTicks) {;}
  soundBlasterPlayTime = 0;
}

#else
void waitSoundBlasterStart() {;}
#endif

int main(int argc, const char *argv[]) {

  printf("Mpeg1 movie playback\n");
  printf("----------------------------------\n");
  if (argc != 2) {
    printf("usage: app_mpg.exe <movie.mpg>\n");
    printf("generate videos with ffmpeg and the following commandline:\n");
    printf("ffmpeg -i video.mp4 -c:v mpeg1video -q:v 0 -c:a libtwolame -b:a 224k -format mpeg video.mpg\n");
    printf("\n");
    printf("There should be a low quality test.mpg in the source folder.\n");
    exit(0);
  }
  const char *fileName = argv[1];

  glWatcomPrecisionTimer(GL_TRUE);
  glFastTexturing = GL_TRUE; // only faster in Watcom environment, seems to slow down DJGPP

  if (!glVesa(640,480,16)) glVGA();
  glRefresh();
  unsigned int texture = mpeg1_playMovie(fileName);

#ifdef SBEMUSOUNDBLASTERON
  SBL_LockMemory((void*)soundBlasterCallback,1024);
  SBL_LockMemory((void*)(&soundBlasterPlayTime),sizeof(unsigned int));
  SBL_LockMemory((void*)(&soundBlasterTicks),sizeof(int));
  SBL_LockMemory((void*)audioRingBuffer,sizeof(audioRingBuffer));
  if (!SBL_Init()) {
    printf("Could initialize SoundBlaster / SBEMU\n");
    printf("You could disable SoundBlater option in source code.\n");
    glDone();
    exit(0);
  }
  mpeg1_setAudioCallback(audioCallback, NULL);
  SBL_POINTER sblMem = SBL_Malloc(SOUNDBLASTERMEMSIZE);
  SBL_StartPlayback(&sblMem, SOUNDBLASTERMEMSIZE, 2, soundBlasterCallback);
#endif

  bool quit = false;
  bool started = false;
  while(!quit) {
#ifdef SBEMUSOUNDBLASTERON
    glSetTime((double)soundBlasterPlayTime/SBL_SampleRate);
#endif
    int key = glNextKey();
    if (key == GL_VK_ESCAPE) 
      quit = true;
    if (!started) waitSoundBlasterStart();    
    if (mpeg1_update()) {
      if (!started) waitSoundBlasterStart();    
      started = true;
      mpeg1_display(texture); // this happens (almost) at sound blaster time 0
    }
    glRefresh();
  }

  glDone();


#ifdef SBEMUSOUNDBLASTERON
  SBL_Free(&sblMem);
  SBL_Done();
#endif

  return 0;
}
