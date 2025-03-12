#include "Maincfg.h"
#include "framework.h"
#include "IIR_coeffs/bp.h"
#include "IIR_coeffs/lp.h"
#include "IIR_coeffs/hp.h"
#include "Main.h"

uint32_t dip_all;
uint8_t dip_1, dip_2, dip_6, dip_7, dip_8;

float buffer[32000];
int ind = 0;
float x[N_IIR_BP] = {0};
float yb[N_IIR_BP] = {0};
float yl[N_IIR_LP] = {0};
float yh[N_IIR_HP] = {0};

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
    //printf("%d\n",s16);

    //printf("test2\n");

    DIP_get(DIP_1,&dip_1);
    if(dip_1){

        DIP_get(DIP_2,&dip_2);
        if(dip_2){
            filters();
            printf("filter\n");
        }
        else{
            buffer[ind] = (float)s16;
            //output input?
            //run IIR with zeros?
            printf("buffer\n");
        }
        ind = (ind+1)%32000;
    }
    else{
        //do nothing
        //fill buffer with zeros?
        //run IIR with zeros?
        //printf("nothing\n");
    }
}

void LED(){
    printf("test\n");
}

void filters(){
    update_array(x,buffer[ind]);

    float result = 0.0;
    DIP_get(DIP_6,&dip_6);
    if(dip_6){
        result += IIR(x,b_iir_bp,yb,a_iir_bp);
    }

    DIP_get(DIP_7,&dip_7);
    if(dip_7){
        result += IIR(x,b_iir_lp,yl,a_iir_lp);
    }

    DIP_get(DIP_8,&dip_8);
    if(dip_8){
        result += IIR(x,b_iir_hp,yh,a_iir_hp);
    }

    //sum filter results
    write_audio_sample(result);
}


//TODO unroll
float IIR(float x[],float b1[], float y[],float b2[]){
    int i;
    float out = 0.0;
    for(i = 0; i<N_IIR_BP; i++){
        out += x[i]*b1[i];
    }
    for(i = 0;i<N_IIR_BP; i++){
        out += y[i]*b2[i];
    }
    update_array(y,out);
    return out;
}

void update_array(float arr[], float new){
    int i = 0;
    for(i = N_IIR_LP; i>1; i--){
        arr[i] = arr[i-1];
    }
    arr[0] = new;
}


