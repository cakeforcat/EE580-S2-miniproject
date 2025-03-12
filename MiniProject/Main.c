#include "Main.h"
#include "Maincfg.h"
#include "framework.h"

void main(void){
    initAll();
    return;
    // return to BIOS scheduler
}


//Possibly have to do HWI for switches
//lower priority?

void audioHWI(void){

    //update switches

    //if s2.1

    //if s2.2
    //filters function
    //output result

    //else
    //buffer store function
    //output input?
    //run IIR with zeros?

    //else
    //do nothing
    //fill buffer with zeros?
    //run IIR with zeros?
}

void buffer(){
    int16_t s16;
    s16 = read_audio_sample();
}

void filters(){
    //update x with new value

    //if 2.6
    //call IIR with LP filter
    //update y with new value

    //if 2.6
    //call IIR with BP filter
    //update y with new value

    //if 2.6
    //call IIR with HP filter
    //update y with new value

    //sum filter results
    //output result
}

float IIR(float x[],float b1[], float y[],float b2[]){
    int i;
    float out = 0.0;
    for(i = 0; i<100; i++){
        out += x[i]*b1[i];
    }
    for(i = 0;i<100; i++){
        out += y[i]*b2[i];
    }
    return out;
}

