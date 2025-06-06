#ifndef __SPEAKER_HPP__
#define __SPEAKER_HPP__

// this is not working in dos4gw, in !!!!!pmode/w!!!!! it's working
// samples are played at 18643.4375 Hz (timerirq)
// it somehow "minimally" alters the DOS clock (not the RTC), yet. I didn't found the reason for this, yet.
extern double speakerFrequency;
// how many seconds since enableSamplePlayback
extern volatile double speakerSeconds;
// you can disable sound playback by this
extern volatile bool muteSpeaker;

// data is a bitstream (8 consecutive bits per byte) and len the length in bits 
// (you have to delete the sample by yourself)
void playSample(unsigned char *data, int len, bool loop); // uses a bitstream (use convertSampleFromUInt8)
void stopSample();

int samplePosition();
int sampleLength();

// new sample size is (len+7)/8 // convertes from uint8 samples to bitstream
// len is sample count (not for 8 bit (signed) samples)
unsigned char *convertSampleFromUInt8(unsigned char *data, int len);
unsigned char *convertSampleFromSInt8(char *data, int len);
unsigned char *convertSampleFromUInt16(unsigned short *data, int len);
unsigned char *convertSampleFromSInt16(short *data, int len);

// enables the sample player (timer irq)
void enableSamplePlayback();
// disables the sample player (must be called before another enableSamplePlayback)
void disableSamplePlayback();

// mixing version uses a signed int ringbuffer
void playSample32BitSigned(signed int *data, int len, double volume); // for enable32BitSignedSamplePlayback
void enable32BitSignedSamplePlayback(signed int *audioBuffer, int len);

#endif //__SPEAKER_HPP__
