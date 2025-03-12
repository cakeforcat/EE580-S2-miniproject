#include "Main.h"
#include "Maincfg.h"
#include "framework.h"

uint32_t dip_all;
uint8_t dip_1, dip_2, dip_3, dip_4, dip_5, dip_6, dip_7, dip_8;

void main(void){
    initAll();
    DIP_get(DIP_1,&dip_1);
    printf("%d\n",dip_1);

    return;
    // return to BIOS scheduler
}


//Possibly have to do HWI for switches
//lower priority?

void audioHWI(void){
    int16_t s16;
    s16 = read_audio_sample();
    printf("%d\n",s16);

    //printf("test2\n");
    DIP_get(DIP_1,&dip_1);
    if(dip_1){

        DIP_get(DIP_2,&dip_2);
        if(dip_2){
            //filters function
            //output result
            printf("filter\n");
        }
        else{
            //buffer store function
            //output input?
            //run IIR with zeros?
            printf("buffer\n");
        }
    }
    else{
        //do nothing
        //fill buffer with zeros?
        //run IIR with zeros?
        //printf("nothing\n");
    }
    write_audio_sample(s16);
}

void LED(){
    printf("test\n");
}

void buffer(){
    //int16_t s16;
    //s16 = read_audio_sample();
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

