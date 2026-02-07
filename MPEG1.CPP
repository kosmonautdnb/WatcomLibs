#include "c:/_OPENGL_/GL.H"
#define PL_MPEG_IMPLEMENTATION
#include "c:/_STL_/PLMPEG.HPP"
#include "MPEG1.HPP"

extern int glFrameBufferWidth;
extern int glFrameBufferHeight;
extern unsigned int *glFrameBuffer;

plm_t *plm = NULL;
double plmSeconds = 0;
double plmAspect = 1;
void video_cb(plm_t *plm, plm_frame_t *frame, void *user) {plm_frame_to_rgba(frame, (uint8_t *)user, plm_get_width(plm) * sizeof(unsigned int));}

MPEG1_AUDIOCALLBACK plmAudioCallback;
int plmSamplerate = 0;

void audio_cb(plm_t *plm, plm_samples_t *samples, void *user) {
  if (plmAudioCallback != NULL) {
    plmAudioCallback(samples->interleaved, samples->count, plmSamplerate, samples->time, user);
  }
}

void mpeg1_setAudioCallback(MPEG1_AUDIOCALLBACK callback, void *userData) {
  plmAudioCallback = callback;
  plm_set_audio_decode_callback(plm, audio_cb, userData);
}


GLuint mpeg1_playMovie(const char *movieName) {
  FILE *file = fopen(movieName,"rb");
  if (!file) {
    return 0;
  }
  
  plm = plm_create_with_file(file, true); // true = close when done (destroyed)
  if (!plm) {
    return 0;
  }

  int w = plm_get_width(plm);
  int h = plm_get_height(plm);
  plmAspect = ((double)w/glFrameBufferWidth) / ((double)h/glFrameBufferHeight);

  plmSamplerate = plmSamplerate = plm_get_samplerate(plm);

  unsigned int *frame_buffer = new unsigned int[w * h];
  for (int i = 0; i < w * h; i++) frame_buffer[i] = 0xff000000;
  GLuint texture;
  glGenTextures(1,&texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,frame_buffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  delete[] frame_buffer;
  frame_buffer = glGetTexturePointer(texture);

  plm_set_video_decode_callback(plm, video_cb, frame_buffer);
  plm_set_audio_decode_callback(plm, audio_cb, NULL);
  
  plm_set_loop(plm, false);
  plm_set_audio_enabled(plm, true);
  plm_set_audio_stream(plm, 0);
  
  
  plmSeconds = glSeconds();
  return texture;
}

void mpeg1_close(unsigned int texture) {
  glDeleteTextures(1,&texture);
  if (plm == NULL) return;
  plm_destroy(plm);
  plm = NULL;
}

bool mpeg1_update() {
  double plmSeconds2 = glSeconds();
  plm_decode(plm, plmSeconds2-plmSeconds);
  plmSeconds = plmSeconds2;
  return !plm_has_ended(plm);
}

void mpeg1_display(unsigned int texture) {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(0,0,-0.5f);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture);
  glColor4f(1.f,1.f,1.f,1.f);
  glVertex4f(0.f,0.f,0.f,1.f);
  glBegin(GL_QUADS);
  glTexCoord2f(0.f,0.f); glVertex2f(-1.f*plmAspect,1.f);
  glTexCoord2f(0.f,1.f); glVertex2f(-1.f*plmAspect,-1.f);
  glTexCoord2f(1.f,1.f); glVertex2f(1.f*plmAspect,-1.f);
  glTexCoord2f(1.f,0.f); glVertex2f(1.f*plmAspect,1.f);
  glEnd();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
}

double mpeg1_playTime() {
  return plm_get_time(plm);
}
