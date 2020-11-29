// Function declarations for Program 2: audio
// ECE 209, NC State University, Fall 2019
// DO NOT CHANGE THIS FILE!!!!!

#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>

// open and read input file, which should be in WAV format
// extract information and print to console (see spec for details)
// return values:
// 0 = success
// 1 = could not open input file
// 3 = error in input file format
int readWAV(const char* infileName);

// for each operation below, return values are:
// 0 = success
// 1 = could not open input file (missing or format error)
// 2 = could not open output file
// 3 = any other error

// create a new file that is an exact copy of the original
int copyWAV(const char* infileName, const char* outfileName);

// adds an echo of the original file into the output file
// each output sample = (input sample + delayed input sample) / 2
// # samples in the output file is the same as the original file
int echoWAV(const char* infileName, int delay, const char* outfileName);

// applies a digital filter to the input file
// each output sample = average of previous N input samples
// # samples in the output file is the same as the original file
int filterWAV(const char* infileName, int filterLength, const char* outfileName);

// creates a weighted mixture of two input files
// each output sample = (input sample 1 * perCent1 + input sample 2 * perCent2) / 100
// # samples in the output file is the same as the longest input file
int mixWAV(const char* infileName1, int perCent1, const char* infileName2, int perCent2, const char* outfileName);


// File utility functions -- these are meant to be called by the functions above.
// These functions deal with opening a file and reading/writing the WAV header.
// The file stream must remain open, because it will be used by the caller for
// reading or writing data.

// headerInfo is an array of 5 integers
// 0: number of channels
// 1: sample rate (samples / sec)
// 2: sample block size (bytes) -- all channels
// 3: bits per sample -- per channel
// 4: number of samples

// open WAV file for reading, then read header and extract information
// file must be left open for reading data
// return values:
// if success, return input stream
// if could not open, or if some file format error, return 0
FILE* openWAVRead(const char * name, int * headerInfo);

// open WAV file for writing, then write header using provided info
// file must be left open for writing data
// return values:
// if success, return output stream
// if could not open, return 0
FILE* openWAVWrite(const char * name, const int * headerInfo);

#endif //AUDIO_H
