#ifndef __GLRECORD_HPP__
#define __GLRECORD_HPP__

// ------------

// main function, you only need to place just this before your gl functions (but after glVesa/glVGA/glDirect) and check the generated file for instructions on how to use this.
void useGLRecordWithDescFile(const char *fileName);

// ------------

#define MAXGLRECORDFILENAMELENGTH 1024

typedef enum GLRECORDMODE {
  GLRECORD_OFF = 0,
  GLRECORD_RECORDING = 1,
  GLRECORD_PLAYBACK = 2,
  GLRECORD_RENDERH264 = 3
} GLRECORDMODE;

void setGLRecordRenderFrameRate(double frameRate);
void setGLRecordFileName(const char *fileName);
void setGLRecordH264FileName(const char *fileName);
void setGLRecordMode(int glRecordMode);

#endif // __GLRECORD_HPP__
