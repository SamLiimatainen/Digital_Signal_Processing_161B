#include <stdlib.h>
#include <stdio.h>
#include <float.h>
//#include "wave.h"
#include <sndfile.h>
#include <math.h>

#define PI 3.14159265

int main(int argc, char *argv[])
{
	int ii;

	//Require 2 arguments: input file and output file
	if(argc < 3)
	{
		printf("Not enough arguments \n");
		return -1;
	}

	SF_INFO sndInfo;
	SNDFILE *sndFile = sf_open(argv[1], SFM_READ, &sndInfo);
	if (sndFile == NULL) {
		fprintf(stderr, "Error reading source file '%s': %s\n", argv[1], sf_strerror(sndFile));
		return 1;
	}

	SF_INFO sndInfoOut = sndInfo;
	sndInfoOut.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	sndInfoOut.channels = 1;
	sndInfoOut.samplerate = sndInfo.samplerate;
	SNDFILE *sndFileOut = sf_open(argv[2], SFM_WRITE, &sndInfoOut);

	// Check format - 16bit PCM
	if (sndInfo.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
		fprintf(stderr, "Input should be 16bit Wav\n");
		sf_close(sndFile);
		return 1;
	}

	// Check channels - mono
	if (sndInfo.channels != 1) {
		fprintf(stderr, "Wrong number of channels\n");
		sf_close(sndFile);
		return 1;
	}

	/* Before you actually start coding:
	1. understand the function of the buffer
	2. print out sndInfo.frames, and what do you see?
	3. check the usage of sf_readf_float and sf_writef_float at http://www.mega-nerd.com/libsndfile/
	*/

	// Allocate memory
	float *buffer = malloc(sizeof(double));
	if (buffer == NULL) {
		fprintf(stderr, "Could not allocate memory for file\n");
		sf_close(sndFile);
		return 1;
	}

	// Load data
	for(ii=0; ii < sndInfo.frames; ii++)
	{
		sf_readf_float(sndFile, buffer, 1);
		
		//Do something to the variable buffer here
        *buffer = (*buffer)*(0.5);
		
		sf_writef_float(sndFileOut, buffer, 1);
	}
	
	sf_close(sndFile);
	sf_write_sync(sndFileOut);
	sf_close(sndFileOut);
	free(buffer);

	return 1;
}
