#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <sndfile.h>
#include <math.h>

#define PI 3.14159265

void iir_filter(float* x,float* a, float* b,float* y);


int main(int argc, char *argv[])
{
	int ii;

	//Require 2 arguments: input file and output file
	if(argc < 2)
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
	SNDFILE *sndFileOut = sf_open(argv[2], SFM_WRITE, &sndInfoOut);//audi with echo

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

	// Your implementation
    float theta = 0.05*2*3.14;
    float r = 0.98;
    float a[] = {1,-2*cos(theta),1};
    float b[] = {1,-2*r*cos(theta),r*r};
    float w[960000];//input array - signal to be filtered
    float y[960000];//output array - output of filter

    for(ii=0; ii < sndInfo.frames; ii++)
    {
        sf_readf_float(sndFile, &w[ii], 1);
        //Do something to the variable buffer here
        //sf_writef_float(sndFileOut, &y[ii], 1);
    }

    iir_filter(w,a,b,y);

    for(ii=0; ii < sndInfo.frames; ii++)
    {
        //sf_readf_float(sndFile, &y[ii], 1);
        //Do something to the variable buffer here
        sf_writef_float(sndFileOut, &y[ii], 1);
    }

	sf_close(sndFile);

	sf_write_sync(sndFileOut);//sound with echo
	sf_close(sndFileOut);

	return 1;
}

void iir_filter(float* x,float* a, float* b,float* y) {
    y[0] = (a[0]*x[1])/b[0];
    y[1] = (a[0]*x[2] + a[1]*x[2-1] - b[1]*y[2-1])/b[0];
    for(int n = 2; n < 960000; n++)
    {
        y[n] = (a[0]*x[n] + a[1]*x[n-1] + a[2]*x[n-2] - b[1]*y[n-1] - b[2]*y[n-2]) / b[0];
    }
}
