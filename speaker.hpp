#ifndef __SPEAKER_HPP__
#define __SPEAKER_HPP__

// this is not working in dos4gw, in !!!!!pmode/w!!!!! it's working
// samples are played at 18643.4375 Hz (timerirq)
extern double sampleFrequency;

// data is a bitstream (8 consecutive bits per byte) and len the length in bits 
// (you have to delete the sample by yourself)
void playSample(unsigned char *data, int len, bool loop); // uses a bitstream (use convertSampleFromUInt8)
void stopSample();

int samplePosition();
int sampleLength();

// new sample size is (len+7)/8 // convertes from uint8 samples to bitstream
// len is sample count
unsigned char *convertSampleFromUInt8(unsigned char *data, int len);
unsigned char *convertSampleFromSInt8(char *data, int len);
unsigned char *convertSampleFromUInt16(unsigned short *data, int len);
unsigned char *convertSampleFromSInt16(short *data, int len);

// enables the sample player (timer irq)
void enableSamplePlayback();
// disables the sample player (must be called before another enableSamplePlayback)
void disableSamplePlayback();

#endif //__SPEAKER_HPP__
