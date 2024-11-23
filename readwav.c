#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

struct wavHeader {
//char is 8 bits/1 byte
char chunkID[4]; //BIG ENDIAN 0x52494646 ASCII of "RIFF"
uint32_t chunkSize; //LITTLE ENDIAN. chunk size is a 4 byte integer, equals size of entire file in bytes minux 8 bytes for ChunkID and ChunkSize
//chunkSize = 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
char format[4]; //BIG ENDIAN. contains letters "WAVE" or in hex 0x57415645
char subChunk1ID[4]; //BIG ENDIAN contains "fmt " or 0x666d7420
uint32_t subChunk1Size; //LITTLE ENDIAN. size of rest of subchunk that follows this number
uint16_t AudioFormat; //PCM = 1. We will only use PCM to represent audio
uint16_t numChannels; //mono = 1, stereo = 2, etc
uint32_t sampleRate; //LITTLE ENDIAN. i.e. cd sample rate is 44100 times/s or 44.1kHz
uint32_t byteRate; //LITTLE ENDIAN == sampleRate * NumChannels * BitsPerSample/8
uint16_t blockAlign; //== NumChannels * BitsPerSample/8
uint16_t bitsPerSample; //8 bits = 8, 16 bits = 16, etc
char subChunk2ID[4]; //BIG ENDIAN, contains letters "data" or 0x64617461
uint32_t subChunk2Size; //== NumSamples * NumChannels * BitsPerSample/8. This is the number of bytes in the data

};

int main() {

struct wavHeader wavh; //make a wavHeader struct

//buffer_size for reading audio: = sample_rate * num_channels * bytes_per_sample * duration
FILE *wavfile = fopen("432square.wav", "rb"); //"rb" means 'read binary'

if (wavfile == NULL) {
    printf("Could not locate file \n");
    exit (-1);
}
//read chunkID ("RIFF")
uint8_t buffer; //read 1 byte at a time. 

for (int i = 0; i < 4; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.chunkID[i] = buffer;
    printf("%c", buffer); //print whatever is in buffer
}

//file pointer is now at LSB of chunkSize
//next, read chunkSize, which is a little endian 4-byte number 
for (int i = 0; i < 4; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.chunkSize +=  buffer*(unsigned int)pow(256, i); //calculate chunk size based on little endian scheme
 printf("Byte %d: %02X\n", i, buffer);

} 
    printf("Chunk size: %u\n", wavh.chunkSize);

//next, read format, should be next 4 BIG ENDIAN bytes that read "WAVE"
for (int i = 0; i < 4; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.format[i] = buffer;
    printf("%c", buffer); //print whatever is in buffer
}

//next, read ID of first subchunk (subChunk1ID), should be next 4 BIG ENDIAN bytes that read "fmt " (WITH SPACE)
for (int i = 0; i < 4; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.subChunk1ID[i] = buffer;
    printf("%c", buffer); //print whatever is in buffer
}

//now, read size of first subchunk. 4-byte LITTLE endian number
for (int i = 0; i < 4; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.subChunk1Size +=  buffer*(unsigned int)pow(256, i); //calculate chunk size based on little endian scheme
    printf("Byte %d: %02X\n", i, buffer);
} 
 printf("Sub-chunk 1 size: %u\n", wavh.subChunk1Size);

//next, read next 2 bytes into audio format
fread(&buffer, 1, 1,  wavfile);
wavh.AudioFormat += buffer;
fread(&buffer, 1, 1,  wavfile);
wavh.AudioFormat += buffer;
printf("Audio format(should be 1): %d\n",wavh.AudioFormat);

//read next 2 bytes for numChannels
fread(&buffer, 1, 1,  wavfile);
wavh.numChannels += buffer;
fread(&buffer, 1, 1,  wavfile);
wavh.numChannels += buffer;
printf("Number of channels: %d\n",wavh.numChannels);

//read next 4 bytes to get sample rate. This is a little endian number
for (int i = 0; i < 4; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.sampleRate +=  buffer*(unsigned int)pow(256, i); //calculate chunk size based on little endian scheme
    printf("Byte %d: %02X\n", i, buffer);
} 
   printf("Sample rate: %u\n", wavh.sampleRate);

//read byte rate little endian value 
for (int i = 0; i < 4; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.byteRate +=  buffer*(unsigned int)pow(256, i); //calculate chunk size based on little endian scheme
    printf("Byte %d: %02X\n", i, buffer);
} 
  printf("Byte rate %u\n", wavh.byteRate);

//read little endian block align value: 
for (int i = 0; i < 2; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.blockAlign+=  buffer*(unsigned int)pow(256, i); //calculate chunk size based on little endian scheme   
    printf("Byte %d: %02X\n", i, buffer);
} 
printf("Block align: %u\n", wavh.blockAlign);

//little endian bits per sample: 
for (int i = 0; i < 2; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.bitsPerSample+=  buffer*(unsigned int)pow(256, i); //calculate chunk size based on little endian scheme
    printf("Byte %d: %02X\n", i, buffer);
} 
printf("Bits per sample: %u\n", wavh.bitsPerSample);

//big endian subchunk2ID. Should just be "DATA"
for (int i = 0; i < 4; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.subChunk2ID[i] = buffer;
    printf("%c", buffer); //print whatever is in buffer
}

//little endian subchunk2size:
for (int i = 0; i < 4; i++) {
    fread(&buffer, 1, 1,  wavfile);
    wavh.subChunk2Size +=  buffer*(unsigned int)pow(256, i); //calculate chunk size based on little endian scheme
    printf("Byte %d: %02X\n", i, buffer);
}
printf("Sub-chunk 2 size: %u\n", wavh.subChunk2Size);

fclose(wavfile);
return 0;
}

