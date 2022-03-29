#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <sndfile.h>
#include <math.h>

#define PI 3.14159265


void fir_filter(float* w,float* h,float* y);


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
    float w[960000];//input array - signal to be filtered
    float y[960000];//output array - output of filter
    float g[15];
    // make the time domain notch filter
    float wc = (2400/48000)*2*3.14;
    int n[] = {-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7};
    for(int i = 0; i <=14; i++)
    {
        if(n[i] != 0)
        {
            g[i]=2*(sin(wc*n[i])/(3.14*n[i]));
        }
        else
        {
            g[i] = (2*wc/3.14)-1;
        }
    }

    for(ii=0; ii < sndInfo.frames; ii++)
    {
        sf_readf_float(sndFile, &w[ii], 1);
    }

    fir_filter(w,g,y);

    for(ii=0; ii < sndInfo.frames; ii++)
    {
        sf_writef_float(sndFileOut, &y[ii], 1);
    }

	sf_close(sndFile);

	sf_write_sync(sndFileOut);//sound with echo
	sf_close(sndFileOut);



	return 1;
}


void fir_filter(float* w,float* h,float* y) {
    for(int n = 14; n < 960000; n++)
    {
        for(int j = 0; j < 15; j++)
        {
            y[n] = y[n] + h[j]*w[n-j];
        }
    }
}