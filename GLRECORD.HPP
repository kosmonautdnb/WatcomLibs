#ifndef __GLRECORD_HPP__
#define __GLRECORD_HPP__

// ------------

// main function, you only need to place just this before your gl functions (but after glVesa/glVGA/glDirect) and check the generated file for instructions on how to use this.
// best directly before the mainloop since there is a glSetTime(0) involved.
// maybe use overideGLRecordVideoResolution before to set video width and height
// for youtube you need at least 1280x720, it will be very bad looking in youtube anotherway.
void useGLRecordWithDescFile(const char *fileName);

// use this to serialize the keyboard state if you are using the keyboard matrix
void glKeyMatrix(bool *keys, int keyCount);

 // direct next key.. can be used to clear the keyboard buffer without generating events for that. Or just to query real keys to stop playback etc..
unsigned short glNextKeyx();
// ------------


#define MAXGLRECORDFILENAMELENGTH 1024

typedef enum GLRECORDMODE {
  GLRECORD_OFF = 0,
  GLRECORD_RECORDING = 1,
  GLRECORD_PLAYBACK = 2,
  GLRECORD_RENDERH264 = 3
} GLRECORDMODE;

void overideGLRecordVideoResolution(int width, int height);
void setGLRecordRenderFrameRate(double frameRate);
void setGLRecordFileName(const char *fileName);
void setGLRecordH264FileName(const char *fileName);
void setGLRecordMode(int glRecordMode);

#endif // __GLRECORD_HPP__
