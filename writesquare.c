#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

const uint32_t SampleRate = 44100;
const uint32_t NumSamples = SampleRate*120; //numSamples = samplerate * duration in S
const uint16_t NumChannels = 1;
const uint16_t BitsPerSample = 16;
const uint32_t ByteRate = SampleRate * NumChannels * BitsPerSample/8;
const uint16_t BlockAlign = NumChannels * BitsPerSample/8;
const uint32_t SubChunk2Size = NumSamples * NumChannels * BitsPerSample/8;

typedef struct {
//char is 8 bits/1 byte
uint8_t chunkID[4]; //BIG ENDIAN 0x52494646 ASCII of "RIFF"
uint8_t chunkSize[4]; //LITTLE ENDIAN. chunk size is a 4 byte integer, equals size of entire file in bytes minux 8 bytes for ChunkID and ChunkSize
//chunkSize = 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
uint8_t format[4]; //BIG ENDIAN. contains letters "WAVE" or in hex 0x57415645
uint8_t  subChunk1ID[4]; //BIG ENDIAN contains "fmt " or 0x666d7420
uint8_t subChunk1Size[4]; //LITTLE ENDIAN. size of rest of subchunk that follows this number
uint8_t AudioFormat[2]; //PCM = 1. We will only use PCM to represent audio
uint8_t numChannels[2]; //mono = 1, stereo = 2, etc
uint8_t sampleRate[4]; //LITTLE ENDIAN. i.e. cd sample rate is 44100 times/s or 44.1kHz
uint8_t byteRate[4]; //LITTLE ENDIAN == sampleRate * NumChannels * BitsPerSample/8
uint8_t blockAlign[2]; //== NumChannels * BitsPerSample/8
uint8_t bitsPerSample[2]; //8 bits = 8, 16 bits = 16, etc
uint8_t subChunk2ID[4]; //BIG ENDIAN, contains letters "data" or 0x64617461
uint8_t subChunk2Size[4]; //== NumSamples * NumChannels * BitsPerSample/8. This is the number of bytes in the data

} wavHeader;

void writeHeader(FILE *wavfile, wavHeader wavh){

fwrite(wavh.chunkID, sizeof(uint8_t), 4, wavfile);

fwrite(wavh.chunkSize, sizeof(uint8_t), 4, wavfile);

fwrite(wavh.format, sizeof(uint8_t), 4, wavfile);

fwrite(wavh.subChunk1ID, sizeof(uint8_t), 4, wavfile);

fwrite(wavh.subChunk1Size, sizeof(uint8_t), 4, wavfile);

fwrite(wavh.AudioFormat, sizeof(uint8_t), 2, wavfile);

fwrite(wavh.numChannels, sizeof(uint8_t), 2, wavfile);

fwrite(wavh.sampleRate, sizeof(uint8_t), 4, wavfile);

fwrite(wavh.byteRate, sizeof(uint8_t), 4, wavfile);

fwrite(wavh.blockAlign, sizeof(uint8_t), 2, wavfile);

fwrite(wavh.bitsPerSample, sizeof(uint8_t), 2, wavfile);

fwrite(wavh.subChunk2ID, sizeof(uint8_t), 4, wavfile);

fwrite(wavh.subChunk2Size, sizeof(uint8_t), 4, wavfile);

}

void writeSquare(FILE *wavfile, int frequency) {
    //start at an offset of 44 bytes (just after the header)
    fseek(wavfile, 44, SEEK_SET);
    //write a frequency hz (frequency oscillations per second) square wave
    //one oscillation is just "up, down, up"
  

}

void writeNoise(FILE *wavfile) {
    //start at offset of 44 bytes
    fseek(wavfile, 44, SEEK_SET);
    int sampleAbsMax = 25000;
    //16 bit SIGNED int
    for (int i = 0; i < NumSamples; i++) {
        int16_t sample = rand() % (sampleAbsMax + 1);
        if (i % 2 == 0) {
        fwrite(&sample, sizeof(int16_t), 2, wavfile);
        } else {
            sample = ~sample +1; //negate sample
            fwrite(&sample, sizeof(int16_t), 2, wavfile);
        }
    }
    

}
int main() {

wavHeader wavh; //declare a new  wav header struct to write from and populate it
wavh.chunkID[0] = (uint8_t)'R';
wavh.chunkID[1] = (uint8_t)'I';
wavh.chunkID[2] = (uint8_t)'F';
wavh.chunkID[3] = (uint8_t)'F';

wavh.chunkSize[0] = 20; //0x14
wavh.chunkSize[1] = 96; //0x60
wavh.chunkSize[2] = 40;//0x28
wavh.chunkSize[3] = 0;//0x00
//0x 00 28 60 14 == 2646036 bytes, the file length minus 8 bytes for chunkSize and chunkID

wavh.format[0] = (uint8_t)'W';
wavh.format[1] = (uint8_t)'A';
wavh.format[2] = (uint8_t)'V';
wavh.format[3] = (uint8_t)'E';

wavh.subChunk1ID[0] = (uint8_t)'f';
wavh.subChunk1ID[1] = (uint8_t)'m';
wavh.subChunk1ID[2] = (uint8_t)'t';
wavh.subChunk1ID[3] = (uint8_t)' ';

wavh.subChunk1Size[0] = 16; //0x10

wavh.AudioFormat[0] = 1; //1 for PCM
//2 bytes but next byte gets left as a 0

wavh.numChannels[0] = 1; //mono for now
//also 2 bytes but next byte stays a 0

wavh.bitsPerSample[0] = 16; //bit depth of 16
//same here

wavh.blockAlign[0] = 2;

//byte rate  to 88100
wavh.byteRate[0] = 136;//0x88
wavh.byteRate[1] = 88; //0x58
wavh.byteRate[2] = 1; //0x01

//set sample rate to 44100kHz
wavh.sampleRate[0] = 68; //0x44
wavh.sampleRate[1] = 172; //0xAC
//0x 00 00 AC 44 == 44100

wavh.subChunk2ID[0] = (uint8_t)'d';
wavh.subChunk2ID[1] = (uint8_t)'a';
wavh.subChunk2ID[2] = (uint8_t)'t';
wavh.subChunk2ID[3] = (uint8_t)'a';


wavh.subChunk2Size[0] = 240; //0xF0
wavh.subChunk2Size[1] = 95; //0x5F
wavh.subChunk2Size[2] = 40; //0x28
//0x 00 28 5F F0  == 2646000

//create wav file
FILE *wavfile = fopen("sound.wav", "wb");

writeHeader(wavfile, wavh);
//at this point is the file pointer still pointed to the end of the header??
writeNoise(wavfile);



//high level, i need to loop through the rest of the samples and write SIGNED bits to them between -32,768 and +32,767

}