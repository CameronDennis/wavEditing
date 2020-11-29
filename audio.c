// Program 2
// ECE 209, NC State University, Fall 2019
// Cameron Dennis

#include "audio.h"
#include <string.h>


//loop reading bytes, converting to int, adding to sample char
int sampleInt(FILE* inFile, const int bits);
void intSample(int lilEnd, const int bits, unsigned char * sample);
//reading in before the function

//file pointer and # of bits in sample (one channel), outputs littleEndian in integer
int sampleInt(FILE* inFile, const int bits) {
    int byteNum = bits / 8;
    int lilEnd = 0;
    unsigned char sample[byteNum];
    fread(sample, 1, byteNum, inFile);      //reads in all bytes of sample (1channel)
    for (int i = 0; i < byteNum; ++i) {
        lilEnd += (sample[i] << (8 * i));
    }
    if (lilEnd>>(bits-1) == 1) {
        lilEnd = (-1 << bits) + lilEnd;
    }
    return lilEnd;
}
//input little endian integer, number of bits in sample (one channel), writes sample in sample array
void intSample(int lilEnd, const int bits, unsigned char * sample) {
    int byteNum = bits / 8;
    if (lilEnd>>(sizeof(int)*8-1) == 1) {
        lilEnd =  lilEnd&((1<<bits)-1);
    }
    for (int i = 0; i < byteNum; ++i) {
        sample[i] = (lilEnd>>(8*i))%256;
    }
}



// headerInfo is an array of 5 integers
// 0: number of channels
// 1: sample rate (samples / sec)
// 2: sample block size (bytes) -- all channels
// 3: bits per sample -- per channel
// 4: number of samples

int copyWAV(const char* infileName, const char* outfileName) {
    int headerInfo[6];
    FILE* inFile = NULL;
    FILE* outFile = NULL;
    inFile = openWAVRead(infileName, headerInfo);
    outFile = openWAVWrite(outfileName, headerInfo);
    if (headerInfo[5] == 1) return 3;
    if (inFile == 0) return 1;      //could not open input file
    if (outFile == 0) return 2;      //could not open output file
    unsigned char sample[headerInfo[3]/8];
    for (int i = 0; i < headerInfo[4]*headerInfo[0]; ++i) {
        int lilEnd = sampleInt(inFile, headerInfo[3]);
        intSample(lilEnd, headerInfo[3], sample);
        fwrite(sample, 1, headerInfo[3]/8, outFile);
    }
    fclose(outFile);
    return 0;
}


int echoWAV(const char* infileName, int delay, const char* outfileName) {
    int headerInfo[6];
    FILE* inFile = NULL;
    FILE* outFile = NULL;
    inFile = openWAVRead(infileName, headerInfo);
    outFile = openWAVWrite(outfileName, headerInfo);
    if (headerInfo[5] == 1) return 3;
    if (inFile == 0) return 1;      //could not open input file
    if (outFile == 0) return 2;      //could not open output file
    unsigned char sample[headerInfo[3]/8];
    int storage[delay];
    for (int i = 0; i < delay; ++i) {
        storage[i] = 0;
    }
    for (int i = 0; i < headerInfo[4]*headerInfo[0]; ++i) {
        int lilEnd = sampleInt(inFile, headerInfo[3]);
        int hold = storage[i%delay];
        storage[i%delay] = lilEnd;
        lilEnd += hold;
        lilEnd /= 2;
        intSample(lilEnd, headerInfo[3], sample);
        fwrite(sample, 1, headerInfo[3]/8, outFile);
    }
    return 0;
}

int filterWAV(const char* infileName, int length, const char* outfileName) {
    int headerInfo[6];
    FILE* inFile = NULL;
    FILE* outFile = NULL;
    inFile = openWAVRead(infileName, headerInfo);
    outFile = openWAVWrite(outfileName, headerInfo);
    if (headerInfo[5] == 1) return 3;
    if (inFile == 0) return 1;      //could not open input file
    if (outFile == 0) return 2;      //could not open output file
    unsigned char sample[headerInfo[3]/8];
    int storage[length];
    for (int i = 0; i < length; ++i) {
        storage[i] = 0;
    }
    for (int i = 0; i < headerInfo[4]*headerInfo[0]; ++i) {
        int lilEnd = sampleInt(inFile, headerInfo[3]);
        //int hold = storage[i%length];
        storage[i%length] = lilEnd;
        int average = 0;
        if (i < length) {
            for (int j = 0; j <= i; ++j) {
                average += storage[j];
            }
            average /= i+1;
        }
        else {
            for (int j = 0; j < length; ++j) {
                average += storage[j];
            }
            average /= length;
        }
        intSample(average, headerInfo[3], sample);
        fwrite(sample, 1, headerInfo[3]/8, outFile);
    }
    return 0;
}

int mixWAV(const char* infileName1, int perCent1, const char* infileName2, int perCent2, const char* outfileName) {
    int headerInfo1[6];
    int headerInfo2[6];
    int headerInfo[6];
    FILE* inFile1 = NULL;
    FILE* inFile2 = NULL;
    FILE* outFile = NULL;
    inFile1 = openWAVRead(infileName1, headerInfo1);
    inFile2 = openWAVRead(infileName2, headerInfo2);
    if (headerInfo1[0]!=headerInfo2[0]) return 3; //channels not same
    if (headerInfo1[1]!=headerInfo2[1]) return 3; //rates not same
    if (headerInfo1[3]!=headerInfo2[3]) return 3; //bits not same
    if ((perCent1 + perCent2)!=100) return 3; //percents dont't add
    unsigned char sample[headerInfo[3]/8];
    if (headerInfo2[4] > headerInfo1[4]) {
        for (int i = 0; i < 5; ++i) {
            headerInfo[i] = headerInfo2[i];
        }
    }
    else {
        for (int i = 0; i < 5; ++i) {
            headerInfo[i] = headerInfo1[i];
        }
    }
    outFile = openWAVWrite(outfileName, headerInfo);
    if (headerInfo1[5] == 1) return 3;
    if (inFile1 == 0) return 1;      //could not open input file
    if (inFile2 == 0) return 1;      //could not open input file
    if (outFile == 0) return 2;      //could not open output file
    if (headerInfo2[4] > headerInfo1[4]) {
        for (int i = 0; i < headerInfo[4]*headerInfo[0]; ++i) {
            if (i < headerInfo1[4]) {
                int lilEnd1 = perCent1*sampleInt(inFile1, headerInfo[3]);
                int lilEnd2 = perCent2*sampleInt(inFile2, headerInfo[3]);
                int lilEnd = lilEnd1 + lilEnd2;
                intSample(lilEnd/100, headerInfo[3], sample);
                fwrite(sample, 1, headerInfo[3]/8, outFile);
            }
            else {
                int lilEnd = perCent1*sampleInt(inFile1, headerInfo[3]);
                intSample(lilEnd/100, headerInfo[3], sample);
                fwrite(sample, 1, headerInfo[3]/8, outFile);
            }
        }
    }
    else {
        for (int i = 0; i < headerInfo[4]*headerInfo[0]; ++i) {
            if (i < headerInfo2[4]) {
                int lilEnd1 = perCent1*sampleInt(inFile1, headerInfo[3]);
                int lilEnd2 = perCent2*sampleInt(inFile2, headerInfo[3]);
                int lilEnd = lilEnd1 + lilEnd2;
                intSample(lilEnd/100, headerInfo[3], sample);
                fwrite(sample, 1, headerInfo[3]/8, outFile);
            }
            else {
                int lilEnd = perCent2*sampleInt(inFile2, headerInfo[3]);
                intSample(lilEnd/100, headerInfo[3], sample);
                fwrite(sample, 1, headerInfo[3]/8, outFile);
            }
        }
    }


    return 0;
}

int readWAV(const char* infileName) {
    int fmtCode = 0;
    int fmtSize = 0;
    int channel = 0;
    int sampleRate = 0;
    int dataRate = 0;
    int blockAlign = 0;
    int bitsPer = 0;
    int chunkSize = 0;
    unsigned char holdByte[4];
    FILE* inFile = NULL;                //creates and initializes inFile as pointer
    inFile = fopen(infileName, "rb");   //opens file and assigns the pointer to inFile
    if (inFile == NULL) return 1;       //if the file does not open, returns 1
    fread(holdByte, 4, 1, inFile);
    if ((holdByte[0]=='R')&&(holdByte[1]=='I')&&(holdByte[2]=='F')&&(holdByte[3]=='F')) {
        printf("RIFF confirmed.\n");    //if RIFF tag present
    }
    else {
        printf("RIFF not found.\n");    //if RIFF tag not present
        fclose(inFile);
        return 3;
    }
    fread(holdByte, 4, 1, inFile);      //skips chunk size
    fread(holdByte, 4, 1, inFile);      //reads WAVE tag
    if ((holdByte[0]=='W')&&(holdByte[1]=='A')&&(holdByte[2]=='V')&&(holdByte[3]=='E')) {
        printf("WAVE confirmed.\n");    //if WAVE tag present
    }
    else {
        printf("WAVE not found.\n");    //if WAVE tag not present
        fclose(inFile);
        return 3;
    }
    fread(holdByte, 4, 1, inFile);      //reads "fmt " tag
    if ((holdByte[0]=='f')&&(holdByte[1]=='m')&&(holdByte[2]=='t')&&(holdByte[3]==' ')) {
        printf("----- fmt block -----\n");    //if fmt tag present
    }
    else {
        printf("fmt block not found.\n");   //if fmt tag not present
        fclose(inFile);
        return 3;
    }
    fread(holdByte, 4, 1, inFile);          //reads in format size
    fmtSize = holdByte[0]+(holdByte[1]<<8)+(holdByte[2]<<16)+(holdByte[3]<<24); //littleEndian to int
    printf("fmt size: %u\n", fmtSize);
    fread(holdByte, 2, 1, inFile);
    fmtCode = holdByte[0]+(holdByte[1]<<8);     //littleEndian to int
    printf("fmt code: %u\n", fmtCode);
    if (fmtSize != 16) {
        printf("Unsupported format.\n");    //if fmt size is not 16
        fclose(inFile);
        return 3;
    }
    if (fmtCode != 1) {
        printf("Unsupported format.\n");    //if fmt code is not 1
        fclose(inFile);
        return 3;
    }
    fread(holdByte, 2, 1, inFile);
    channel = holdByte[0]+(holdByte[1]<<8);     //littleEndian to int
    printf("Channels: %u\n", channel);
    if ((channel < 1)||(channel > 2)) {
        printf("Unsupported format.\n");    //if channel is not 1 or 2
        fclose(inFile);
        return 3;
    }
    fread(holdByte, 4, 1, inFile);
    sampleRate = holdByte[0]+(holdByte[1]<<8)+(holdByte[2]<<16)+(holdByte[3]<<24); //littleEndian to int
    printf("Sample rate (samples/sec): %u\n", sampleRate);
    fread(holdByte, 4, 1, inFile);
    dataRate = holdByte[0]+(holdByte[1]<<8)+(holdByte[2]<<16)+(holdByte[3]<<24); //littleEndian to int
    printf("Data rate (bytes/sec): %u\n", dataRate);
    fread(holdByte, 2, 1, inFile);
    blockAlign = holdByte[0]+(holdByte[1]<<8);     //littleEndian to int
    printf("Bytes per sample (all channels): %u\n", blockAlign);
    fread(holdByte, 2, 1, inFile);
    bitsPer = holdByte[0]+(holdByte[1]<<8);     //littleEndian to int
    printf("Bits per sample (one channel): %u\n", bitsPer);
    fread(holdByte, 4, 1, inFile);
    if ((holdByte[0]=='d')&&(holdByte[1]=='a')&&(holdByte[2]=='t')&&(holdByte[3]=='a')) {
        printf("----- data block -----\n");    //if data tag present
    }
    else {
        printf("data block not found.\n");   //if data tag not present
        fclose(inFile);
        return 3;
    }
    fread(holdByte, 4, 1, inFile);
    chunkSize = (holdByte[0]+(holdByte[1]<<8)+(holdByte[2]<<16)+(holdByte[3]<<24))/blockAlign; //littleEndian to int
    printf("Samples: %d\n", chunkSize);
    fclose(inFile);
    return 0;
}

FILE* openWAVRead(const char* name, int * headerInfo) {
    headerInfo[5] = 0;
    FILE* inFile = NULL;                //creates and initializes inFile as pointer
    inFile = fopen(name, "rb");   //opens file and assigns the pointer to inFile
    unsigned char holdByte[8];
    if (inFile == NULL) return 0;       //if the file does not open, returns 0
    fread(holdByte, 4, 1, inFile);      //reads RIFF tag
    if (!((holdByte[0]=='R')&&(holdByte[1]=='I')&&(holdByte[2]=='F')&&(holdByte[3]=='F'))) {
        //if RIFF tag not present
        headerInfo[5] = 1;
        fclose(inFile);
        return 0;
    }
    fread(holdByte, 4, 1, inFile);      //skips chunk size
    fread(holdByte, 4, 1, inFile);      //reads WAVE tag
    if (!((holdByte[0]=='W')&&(holdByte[1]=='A')&&(holdByte[2]=='V')&&(holdByte[3]=='E'))) {
        //if WAVE tag not present
        headerInfo[5] = 1;
        fclose(inFile);
        return 0;
    }
    fread(holdByte, 4, 1, inFile);      //reads "fmt " tag
    if (!((holdByte[0]=='f')&&(holdByte[1]=='m')&&(holdByte[2]=='t')&&(holdByte[3]==' '))) {
        //if fmt tag not present
        headerInfo[5] = 1;
        fclose(inFile);
        return 0;
    }
    fread(holdByte, 4, 1, inFile);          //reads in format size
    if ((holdByte[0]+(holdByte[1]<<8)+(holdByte[2]<<16)+(holdByte[3]<<24)) != 16) return 0; //littleEndian to int)
    fread(holdByte, 2, 1, inFile);          //reads in format code
    if ((holdByte[0]+(holdByte[1]<<8)) != 1) return 0;
    fread(holdByte, 2, 1, inFile);          //reads channels
    if (((holdByte[0]+(holdByte[1]<<8)) < 1)||((holdByte[0]+(holdByte[1]<<8)) > 2)) {
        fclose(inFile);
        headerInfo[5] = 1;
        return 0;
    }
    headerInfo[0] = holdByte[0]+(holdByte[1]<<8);     //places number of channels in headerInfo
    fread(holdByte, 4, 1, inFile);          //reads samples/sec
    headerInfo[1] = holdByte[0]+(holdByte[1]<<8)+(holdByte[2]<<16)+(holdByte[3]<<24); //places samples/sec in headerInfo
    fread(holdByte, 4, 1, inFile);          //skip data rate
    fread(holdByte, 2, 1, inFile);          //reads sample block size
    headerInfo[2] = holdByte[0]+(holdByte[1]<<8);     //places sample block size in headerInfo
    fread(holdByte, 2, 1, inFile);          //reads bits/sample/channel
    headerInfo[3] = holdByte[0]+(holdByte[1]<<8);     //places bits/sample/channel in headerInfo
    fread(holdByte, 4, 1, inFile);          //reads in data tag
    if (!((holdByte[0]=='d')&&(holdByte[1]=='a')&&(holdByte[2]=='t')&&(holdByte[3]=='a'))) {
        //if data tag not present
        headerInfo[5] = 1;
        fclose(inFile);
        return 0;
    }
    fread(holdByte, 4, 1, inFile);
    headerInfo[4] = (holdByte[0]+(holdByte[1]<<8)+(holdByte[2]<<16)+(holdByte[3]<<24))/(headerInfo[2]); //number of samples
    return inFile;
}

FILE* openWAVWrite(const char* name, const int * headerInfo) {

    FILE* inFile = NULL;                //creates and initializes inFile as pointer
    inFile = fopen(name, "wr");   //opens file and assigns the pointer to inFile
    unsigned char holdByte[32];
    int dataSize = headerInfo[4] + 36;
    fwrite("RIFF", 4, 1, inFile);   //Writes RIFF tag
    for (int i = 0; i < 4; ++i) {       //writes chunk size in bytes for top level output
        holdByte[i] = ((dataSize>>(8*i))%256);
    }
    fwrite(holdByte, 1, 4, inFile);
    fwrite("WAVE", 4, 1, inFile);   //Writes WAVE tag
    fwrite("fmt ", 4, 1, inFile);   //Writes fmt tag
    for (int i = 0; i < 4; ++i) {       //writes chunk size in bytes for format output
        holdByte[i] = ((16>>(8*i))%256);
    }
    fwrite(holdByte, 1, 4, inFile);
    for (int i = 0; i < 2; ++i) {       //writes audio type PCM for format output
        holdByte[i] = ((1>>(8*i))%256);
    }
    fwrite(holdByte, 1, 2, inFile);
    for (int i = 0; i < 2; ++i) {       //writes channels for format output
        holdByte[i] = (((headerInfo[0])>>(8*i))%256);
    }
    fwrite(holdByte, 1, 2, inFile);
    for (int i = 0; i < 4; ++i) {       //writes sample rate for format output
        holdByte[i] = (((headerInfo[1])>>(8*i))%256);
    }
    fwrite(holdByte, 1, 4, inFile);
    for (int i = 0; i < 4; ++i) {       //writes data rate for format output
        holdByte[i] = (((headerInfo[1]*headerInfo[2])>>(8*i))%256);
    }
    fwrite(holdByte, 1, 4, inFile);
    for (int i = 0; i < 2; ++i) {       //writes block align for format output
        holdByte[i] = ((headerInfo[2])>>(8*i))%256;
    }
    fwrite(holdByte, 1, 2, inFile);
    for (int i = 0; i < 2; ++i) {       //writes bits per sample for format output
        holdByte[i] = ((headerInfo[3])>>(8*i))%256;
    }
    fwrite(holdByte, 1, 2, inFile);
    fwrite("data", 4, 1, inFile);   //Writes data tag
    for (int i = 0; i < 4; ++i) {       //writes chunk size data output
        holdByte[i] = (((headerInfo[4]*(headerInfo[2]))>>(8*i))%256);
    }
    fwrite(holdByte, 1, 4, inFile);
    return inFile;
}

