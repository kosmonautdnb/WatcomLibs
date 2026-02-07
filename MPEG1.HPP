#ifndef __MPEG1_HPP__
#define __MPEG1_HPP__

unsigned int mpeg1_playMovie(const char *movieName); // returns opengl texture
void mpeg1_close(unsigned int texture);
bool mpeg1_update();
void mpeg1_display(unsigned int texture);
double mpeg1_playTime();

#define MPEG1_AUDIO_SAMPLES_PER_FRAME 1152
typedef void (*MPEG1_AUDIOCALLBACK)(float *stereoSamples, int sampleCount, int frequency, double time, void *userData); // SAMPLE_COUNT is always MPEG1_AUDIO_SAMPLES_PER_FRAME
void mpeg1_setAudioCallback(MPEG1_AUDIOCALLBACK callback, void *userData);

#endif // __MPEG1_HPP__
