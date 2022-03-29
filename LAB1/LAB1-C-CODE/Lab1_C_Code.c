#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <sndfile.h>
#include <math.h>

#define PI 3.14159265
#define Fs 8000

// Declare function
void generate_note(float f, float L, float* x);
void iir_filter(float* x,float* a, float* b,float* y);

int main(int argc, char *argv[])
{

	//Require 2 arguments: input file and output file
	if(argc < 2)
	{
		printf("Not enough arguments \n");
		return -1;
	}

	SF_INFO sndInfoOut;;
	sndInfoOut.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	sndInfoOut.channels = 1;
	sndInfoOut.samplerate = Fs;
	SNDFILE *sndFileOut = sf_open(argv[1], SFM_WRITE, &sndInfoOut); // file for complete song x
    SNDFILE *sndFileOut2 = sf_open(argv[2], SFM_WRITE, &sndInfoOut); // file for iir filtered song
	
    // start here

	// load parameters and coefficients
    float tableTwo[] = {440,0,440,0,660,0,660,0,733,0,733,0,660,0,0,0,587,0,587,0,550,0,550,0,495,0,495,0,440,0,0,0};
    float tableThree[] = {220,330,275,330,220,330,275,330,220,367,293,367,220,330,275,330,206,330,293,330,220,330,275,330,206,330,293,330,220,330,275,330};
    float a[] = {0.62477732,-2.444978,3.64114,-2.444978,0.62477732}; //coef for iir filter
    float b[] = {1,-3.1820023,3.9741082,-2.293354,0.52460587}; //coef for iit filter
	float t = 0.5;
	float L = t * Fs; //4000

	//Melody array = x_m[32*4000] this will initialize an empty array with 128000 spaces
	float x_m[128000];
    //Chorus array = x_c[32*4000] this will initialize an empty array with 128000 spaces
    float x_c[128000];
    // Complete song = x[32*4000]
    float x[128000];
    //iir filtered song = y[32*4000]
    float y[128000];


	for(int i = 0;i < 32;i++)
    {
	    //this will build x_m
        generate_note(tableTwo[i],L,&(x_m[(int) L*i]));
        //this will build x_c
        generate_note(tableThree[i],L,&(x_c[(int) L*i]));
    }

	//write the composed song to a WAV file
	for(int j=0;j < 128000;j++)
    {
        // build the complete song x[] array now that you have x_m and x_c
	    x[j] = (0.6*x_m[j]) + (0.4*x_c[j]);
        //write the complete song x[] to a file now
        sf_writef_float(sndFileOut, &(x[j]), 1);
    }

	//call the iir filter to filter the full song x
    iir_filter(x,a,b,y);

	//write the filtered song to a WAV file
    for(int k=0;k < 128000;k++)
    {
        //write the filtered song y[] to a file now
        //this will write to a diff WAV file than the one x[] went to
        sf_writef_float(sndFileOut2, &(y[k]), 1);
    }

	sf_write_sync(sndFileOut);
	sf_close(sndFileOut);

    sf_write_sync(sndFileOut2);
    sf_close(sndFileOut2);

//	free(x); not needed bc i used static variable
//	free(x_m);
//	free(x_c);

	return 1;
}

// Implement function
// This fucntion is like the generate_sigs(f,Fs,L) function in our MatLab code
void generate_note(float f, float L, float* x) {
    for (int i = 0; i < L; i++)
    {
        x[i] = cos(2*PI*f*((float)i/Fs));
    }
}


void iir_filter(float* x,float* a, float* b,float* y) {
    y[0] = (a[0]*x[1])/b[0];
    y[1] = (a[0]*x[2] + a[1]*x[2-1] - b[1]*y[2-1])/b[0];
    y[2] = (a[0]*x[3] + a[1]*x[3-1] + a[2]*x[3-2] - b[1]*y[3-1] - b[2]*y[3-2])/b[0];
    y[3] = (a[0]*x[4] + a[1]*x[4-1] + a[2]*x[4-2] + a[3]*x[4-3] - b[1]*y[4-1] - b[2]*y[4-2] - b[3]*y[4-3])/b[0];
    for(int n = 4; n < 128000; n++)
    {
        y[n] = (a[0]*x[n] + a[1]*x[n-1] + a[2]*x[n-2] + a[3]*x[n-3] + a[4]*x[n-4] - b[1]*y[n-1] - b[2]*y[n-2] - b[3]*y[n-3] - b[4]*y[n-4]) / b[0];
    }

}
