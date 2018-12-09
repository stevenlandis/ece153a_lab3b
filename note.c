#include "note.h"
#include "xil_printf.h"
#include "math.h"
//#include "lcd.h"

//array to store note names for findNote
static char notes[12][3]={"C ","C#","D ","D#","E ","F ","F#","G ","G#","A ","A#","B "};

static const float a4_to_c0 = 1/(16*root2*root2*root2*root2*root2*root2*root2*root2*root2);
float C0;

void setA4(float A4) {
	C0 = A4* a4_to_c0;
}

float fLog(float f) {
	if (f < C0) return 0;
	return log2f(f/C0);
}

char* getNoteStr(int note) {
	return notes[note];
}

//finds and prints note of frequency and deviation from note
//finds and prints note of frequency and deviation from note
char* findNote(float f) {
	float c=261.63;
	float r;
	int oct=4;
	int note=0;
	//determine which octave frequency is in
	if(f >= c) {
		while(f > c*2) {
			c=c*2;
			oct++;
		}
	}
	else { //f < C4
		while(f < c) {
			c=c/2;
			oct--;
		}
	
	}

	//find note below frequency
	//c=middle C
	r=c*root2;
	while(f > r) {
		c=c*root2;
		r=r*root2;
		note++;
	}

//	xil_printf("Note: %s\n", notes[note]);


	//determine which note frequency is closest to
	if((f-c) <= (r-f)) { //closer to left note
//		WriteString("N:");
//		WriteString(notes[note]);
//		WriteInt(oct);
//		WriteString(" D:+");
//		WriteInt((int)(f-c+.5));
//		WriteString("Hz");
	}
	else { //f closer to right note
		note++;
		if(note >=12) note=0;
//		WriteString("N:");
//		WriteString(notes[note]);
//		WriteInt(oct);
//		WriteString(" D:-");
//		WriteInt((int)(r-f+.5));
//		WriteString("Hz");
	}
	return notes[note];

}

int getOctave(float f) {
	return floor(fLog(f));

//	int res = 0;
//	float fC0 = fA4 * a4_to_c0;
//	while (f > fC0) {
//		f /= 2;
//		res++;
//	}
//
//	return res-1;
}

int getNoteN(float f) {
	float fl = fLog(f);

	return floor(12*(fl - floor(fl)));
}

void getFreqInfo(float f, int* oct, int* note, int* cents) {
	*oct = 0;
	*note = 0;
	*cents = 0;
	float testF = C0;

	while (2*testF <= f) {
//		xil_printf("testF: %d\n",(int)testF);
		testF *= 2;
		(*oct)++;
	}

	while (root2*testF <= f) {
//		xil_printf("testF: %d\n",(int)testF);
		testF *= root2;
		(*note)++;
	}

	while (root1200*testF <= f) {
		testF *= root1200;
		(*cents)++;
	}

	// do the rounding
	if ((*cents) > 50) {
		(*cents) -= 100;
		(*note)++;
		if ((*note) >= 12) {
			(*note) = 0;
			(*oct)++;
		}
	}
}

int findNoteFreq(float f) {
	float c=261.63;
	float r;
	int oct=4;
	int note=0;
	//determine which octave frequency is in
	if(f >= c) {
		while(f > c*2) {
			c=c*2;
			oct++;
		}
	}
	else { //f < C4
		while(f < c) {
			c=c/2;
			oct--;
		}

	}

	//find note below frequency
	//c=middle C
	r=c*root2;
	while(f > r) {
		c=c*root2;
		r=r*root2;
		note++;
	}

//	xil_printf("Note: %s\n", notes[note]);


	//determine which note frequency is closest to
	if((f-c) <= (r-f)) { //closer to left note
//		WriteString("N:");
//		WriteString(notes[note]);
//		WriteInt(oct);
//		WriteString(" D:+");
//		WriteInt((int)(f-c+.5));
//		WriteString("Hz");
		return (int)(c+.5);
	}
	else { //f closer to right note
		note++;
		if(note >=12) note=0;
//		WriteString("N:");
//		WriteString(notes[note]);
//		WriteInt(oct);
//		WriteString(" D:-");
//		WriteInt((int)(r-f+.5));
//		WriteString("Hz");
		return (int)(r+.5);
	}


}

int getCents(float f){
	int noteDif;
	int dif;
	float bin;
	int noteFreq = findNoteFreq(f);
	if(f<noteFreq){ //current freq is below ideal note
		int lowNote = noteFreq /root2;
		noteDif = noteFreq-lowNote;
		bin=((float)noteDif)*.01;
		dif = noteFreq-f;
		return -1.0*dif/bin;

	}
	else{ //current freq is above ideal note
		int highNote = noteFreq * root2;
		noteDif = highNote-noteFreq;
		bin=((float)noteDif)*.01;
		dif = f-noteFreq;
		return dif/bin;
	}
}
