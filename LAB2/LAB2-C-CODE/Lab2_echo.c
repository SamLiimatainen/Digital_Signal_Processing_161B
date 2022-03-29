#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <sndfile.h>
#include <math.h>

#define PI 3.14159265

void iir_filter(float* x,float* a, float* b,float* y);
void fir_filter(float* x,float* h,float* y);


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
    // Load data
    float x[960000];//input array
    float y[960000];//output array
    int Ne = 0.5*48000;
    for(ii=0; ii < sndInfo.frames; ii++)
    {
        sf_readf_float(sndFile, &x[ii], 1);

        if(ii - Ne > 0)
        {
            y[ii] = 0.7*x[ii]+0.3*x[ii - Ne];
        }
        else
        {
            y[ii] = 0.7*x[ii];
        }

        //Do something to the variable buffer here

        sf_writef_float(sndFileOut, &y[ii], 1);
    }

	sf_close(sndFile);

	sf_write_sync(sndFileOut);//sound with echo
	sf_close(sndFileOut);

	return 1;
}

