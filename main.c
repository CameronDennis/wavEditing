// Program 2 -- processing audio (WAV) files
// ECE 209, NC State University, Fall 2019
// DO NOT CHANGE THIS FILE!!!!!!!

#include <stdio.h>
#include <string.h>
#include "audio.h"

// This is the user interface part of the program
// This will not be tested directly -- your functions
// will be called and evaluated individually.
// This just gives you a way to test your functions.
int main() {
    char cmd[7] = "";
    char infile1[100] = "";
    char infile2[100] = "";
    char outfile[100] = "";
    int status = 0;

    // User enter a command:  read, copy, echo, filter, mix
    printf("Command: ");
    fflush(stdout);
    scanf("%6s", cmd);

    // read command: get input file and output file
    if (strcmp(cmd, "read") == 0) {
        printf("Input file: ");
        fflush(stdout);
        scanf("%99s", infile1);
        status = readWAV(infile1);
    }
    // copy command: get input file and output file
    else if (strcmp(cmd, "copy") == 0) {
        printf("Input file: ");
        fflush(stdout);
        scanf("%99s", infile1);
        printf("Output file: ");
        fflush(stdout);
        scanf("%99s", outfile);
        status = copyWAV(infile1, outfile);
    }
    // echo command: get input file, delay (samples), and output file
    else if (strcmp(cmd, "echo") == 0) {
        int delay;
        printf("Input file: ");
        fflush(stdout);
        scanf("%99s", infile1);
        printf("Delay (samples): ");
        fflush(stdout);
        scanf("%d", &delay);
        if (delay <= 0) {
            printf("ERROR: Delay must be greater than zero.\n");
            status = 4;
        }
        else {
            printf("Output file: ");
            fflush(stdout);
            scanf("%99s", outfile);
            status = echoWAV(infile1, delay, outfile);
        }
    }
    // mix command:  get two input files, percentages for each, and output file
    else if (strcmp(cmd, "mix") == 0) {
        int part1, part2;
        printf("First input file: ");
        fflush(stdout);
        scanf("%99s", infile1);
        printf("Percentage: ");
        fflush(stdout);
        scanf("%d", &part1);
        printf("Second input file: ");
        fflush(stdout);
        scanf("%99s", infile2);
        printf("Percentage: ");
        fflush(stdout);
        scanf("%d", &part2);
        if ((part1 <= 0) || (part2 <= 0) || (part1 + part2 != 100)) {
            printf("ERROR: Percentages must be positive and sum to 100.\n");
            status = 4;
        } else {
            printf("Output file: ");
            fflush(stdout);
            scanf("%99s", outfile);
            status = mixWAV(infile1, part1, infile2, part2, outfile);
        }
    }
    // filter command: get input file, filter length, output file
    else if (strcmp(cmd, "filter") == 0) {
        int filterLength;
        printf("Input file: ");
        fflush(stdout);
        scanf("%99s", infile1);
        printf("Filter length: ");
        fflush(stdout);
        scanf("%d", &filterLength);
        if (filterLength <= 0) {
            printf("ERROR: Filter length must be greater than zero.\n");
            status = 4;
        }
        else {
            printf("Output file: ");
            fflush(stdout);
            scanf("%99s", outfile);
            status = filterWAV(infile1, filterLength, outfile);
        }
    }

    if (status == 1) {
        printf("Could not open input file.\n");
    }
    else if (status == 2) {
        printf("Could not open output file.\n");
    }
    else if (status == 3) {
        printf("File format error.\n");
    }

    return 0;
}
