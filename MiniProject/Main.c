#include "Maincfg.h"
#include "framework.h"
#include "IIR_coeffs/bp.h"
#include "IIR_coeffs/lp.h"
#include "IIR_coeffs/hp.h"
#include "Main.h"

uint32_t alldip;
uint8_t dip_1, dip_2, dip_6, dip_7, dip_8;
Bool dips[8];

float buffer[32768];
//float obuffer[2000];
ulong_t ind = 0;
float x[N_IIR_BP] = {0.0};
float yb[N_IIR_BP];
float yl[N_IIR_LP];
float yh[N_IIR_HP];
int16_t s16;


void main(void){

    int i;
    for(i = 0;i<N_IIR_BP;i++){x[i]=0.0;yb[i]=0.0;yl[i]=0.0;yh[i]=0.0;}
    for(i = 0;i<32000;i++){buffer[i]=0.0;}
    //for(i = 0;i<2000;i++){obuffer[i]=0.0;}

    DIP_getAll(&alldip);
    for(i = 0;i<8;i++){
        dips[i]= (alldip & ( 1 << i )) >> i;
    }

    initAll();
    //DO NOT PUT CODE HERE!!!
    return;
    // return to BIOS scheduler
}


//Possibly have to do HWI for switches
//lower priority?

void audioHWI(void){

    s16 = read_audio_sample();
    ind = (ind+1) & (32767);
    //MCASP->RSLOT;

    if(!dips[0]){
        //write_audio_sample(s16);

        if(!dips[1]){
            update_array(x,buffer[ind]);

            float result = 0.0;
            if(!dips[5]){
                result += IIR(a_iir_bp,yb,b_iir_bp);
            }

            if(!dips[6]){
                result += IIR(a_iir_lp,yl,b_iir_lp);
            }

            if(!dips[7]){
                result += IIR(a_iir_hp,yh,b_iir_hp);
            }

            //sum filter results
            //printf("%d\n\n",result);
            write_audio_sample(result);
            //obuffer[ind>>4] = result;
            //printf("%d\n",ind<<4);
            //write_audio_sample(s16);
        }
        else{
            buffer[ind] = (float)s16;
            write_audio_sample(s16);
            //output input?
            //run IIR with zeros?
        }
        //write_audio_sample(s16);
    }
    else{
        write_audio_sample(0);
        //do nothing
        //fill buffer with zeros?
        //run IIR with zeros?
    }

}

void LED(){
    LED_toggle(LED_2);
    int i;
    DIP_getAll(&alldip);
    for(i = 0;i<8;i++){
        dips[i]= (alldip & ( 1 << i )) >> i;
    }
}


//TODO unroll
float IIR(float b1[], float y[],float b2[]){
    int i;
    float out = 0.0;

    //#pragma UNROLL(N_IIR_BP);
    for(i = 0; i<N_IIR_BP; i++){
        out += x[i]*b1[i];
        out += y[i]*b2[i];
    }
    update_array(y,out);
    return out;
}

void update_array(float arr[], float new){
    int i = 0;
    for(i = N_IIR_LP-1; i>0; i--){
        arr[i] = arr[i-1];
    }
    arr[0] = new;
}


