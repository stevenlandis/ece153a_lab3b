#include "fft.h"
#include "complex.h"
#include "trig.h"
#include <xil_printf.h>
#include "stream_grabber.h"

static float new_[512];
static float new_im[512];

void read_fsl_values(float* q, int n) {
   int i;
   unsigned int x;
//   stream_grabber_wait_enough_samples(512);
   stream_grabber_wait_enough_samples(SAMPLES);
   for(i = 0; i < n; i+=DOWN_SAMPLES) {
//      int_buffer[i] = stream_grabber_read_sample(i);
//      int_buffer[i+1] = stream_grabber_read_sample(i+1);
//      int_buffer[i+2] = stream_grabber_read_sample(i+2);
//      int_buffer[i+3] = stream_grabber_read_sample(i+3);
      // xil_printf("%d\n",int_buffer[i]);
//      x = (stream_grabber_read_sample(i)+ stream_grabber_read_sample(i+1)+ stream_grabber_read_sample(i+2)+stream_grabber_read_sample(i+3))/4;
	   x = stream_grabber_read_sample(i);
	   q[i/DOWN_SAMPLES] = .000000049173832*x;
   }
}

/*
q: input signals
w: all zeros initially
n: 512
m: 9
sample_f: 100*1000*1000/2048.0
*/
float fft(float* q, float* w, int n, int m, float sample_f) {
	int a,b, bi,r,d,e,c;
	int k,place;
	a=n/2;
	b=1;
	int i,j;
	float real=0,imagine=0;
	float max,frequency,fc,fs;

	// ORdering algorithm
	for(i=0; i<(m-1); i++){
		d=0;
		for (j=0; j<b; j++){
			for (c=0; c<a; c++){	
				e=c+d;
				new_[e]=q[(c*2)+d];
				new_im[e]=w[(c*2)+d];
				new_[e+a]=q[2*c+1+d];
				new_im[e+a]=w[2*c+1+d];
			}
			d+=(n/b);
		}		
		for (r=0; r<n;r++){
			q[r]=new_[r];
			w[r]=new_im[r];
		}
		b*=2;
		a=n/(2*b);
	}
	//end ordering algorithm

	b=1;
	bi = 0;
	k=0;
	for (j=0; j<m; j++){	
	//MATH
		for(i=0; i<n; i+=2){
			if (i%(n/b)==0 && i!=0)
				k++;
//			real=mult_real(q[i+1], w[i+1], cosine(-PI*k/b), sine(-PI*k/b));
//			imagine=mult_im(q[i+1], w[i+1], cosine(-PI*k/b), sine(-PI*k/b));
			fc = fastCos(k, bi);
			fs = fastSin(k, bi);
			real=mult_real(q[i+1], w[i+1], fc, fs);
			imagine=mult_im(q[i+1], w[i+1], fc, fs);
			new_[i]=q[i]+real;
			new_im[i]=w[i]+imagine;
			new_[i+1]=q[i]-real;
			new_im[i+1]=w[i]-imagine;

		}
		for (i=0; i<n; i++){
			q[i]=new_[i];
			w[i]=new_im[i];
		}
	//END MATH

	//REORDER
		for (i=0; i<n/2; i++){
			new_[i]=q[2*i];
			new_[i+(n/2)]=q[2*i+1];
			new_im[i]=w[2*i];
			new_im[i+(n/2)]=w[2*i+1];
		}
		for (i=0; i<n; i++){
			q[i]=new_[i];
			w[i]=new_im[i];
		}
	//END REORDER	
		b*=2;
		bi++;
		k=0;		
	}

	//find magnitudes
	max=0;
	place=1;
	for(i=1;i<(n/2);i++) { 
		new_[i]=q[i]*q[i]+w[i]*w[i];
		if(max < new_[i]) {
			max=new_[i];
			place=i;
		}
	}
	
	float s=sample_f/n; //spacing of bins
	
	frequency = s*place;

	//curve fitting for more accuarcy
	//assumes parabolic shape and uses three point to find the shift in the parabola
	//using the equation y=A(x-x0)^2+C
	float y1=new_[place-1],y2=new_[place],y3=new_[place+1];
	float x0=s+(2*s*(y2-y1))/(2*y2-y1-y3);
	x0=x0/s-1;
	
	if(x0 <0 || x0 > 2) { //error
		return 0;
	}
	if(x0 <= 1)  {
		frequency=frequency-(1-x0)*s;
	}
	else {
		frequency=frequency+(x0-1)*s;
	}
	
	return frequency;
}
