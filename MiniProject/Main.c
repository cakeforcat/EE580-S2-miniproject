#include "Maincfg.h"
#include "framework.h"
#include "IIR_coeffs/bp.h"
#include "IIR_coeffs/lp.h"
#include "IIR_coeffs/hp.h"
#include "Main.h"
#include <clk.h>

uint32_t alldip;
uint8_t dip_1, dip_2, dip_6, dip_7, dip_8;
Bool dips[8];

Bool toggle_20[2];  // [0] -> LED1, [1] -> LED2
Bool toggle_6;      // only LED2
Bool toggle_2;      // only LED1

float buffer[32768];
//float obuffer[4096];
ulong_t ind = 0;
//ulong_t ind2 = 0;
int16_t cbuf = 0;
float x[N_IIR_BP];
float yb[N_IIR_BP];
float yl[N_IIR_LP];
float yh[N_IIR_HP];
int16_t s16;

LgUns start_time, end_time, duration;

void main(void){

    int i;
    for(i = 0;i<N_IIR_BP;i++){x[i]=0.0;yb[i]=0.0;yl[i]=0.0;yh[i]=0.0;}
    for(i = 0;i<32768;i++){buffer[i]=0.0;}
    //for(i = 0;i<4096;i++){obuffer[i]=0.0;}

    DIP_getAll(&alldip);
    for(i = 0;i<8;i++){
        dips[i]= 1;
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
    if (MCASP->RSLOT){
        ind = (ind+1) & (32767);
        //ind2 = (ind) & (4095);
        //MCASP->RSLOT;

        if(!dips[0]){
            //write_audio_sample(s16);
            start_time = CLK_gethtime();
            if(!dips[1]){
                cbuf = (cbuf+1) & (N_IIR_BP-1);
                x[cbuf] = buffer[ind];

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
                write_audio_sample((int16_t)result);
                //obuffer[ind2] = result;
                //printf("%d\n",ind<<4);
                //write_audio_sample(s16);

                end_time = CLK_gethtime();
                if (start_time>end_time) duration = start_time - end_time;
                else duration =  end_time-start_time;

                LOG_printf(&trace, "ms: %d --- ticks: %d", duration/CLK_countspms(), duration);
            }
            else{
                write_audio_sample(s16);
                buffer[ind] = (float)s16;
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
    else {
        write_audio_sample(0);
    }
}

void DIP_UPDATE(){
    DIP_getAll(&alldip);
    dips[0]= (alldip & ( 1 << 0 )) >> 0;
    dips[1]= (alldip & ( 1 << 1 )) >> 1;
    dips[5]= (alldip & ( 1 << 5 )) >> 5;
    dips[6]= (alldip & ( 1 << 6 )) >> 6;
    dips[7]= (alldip & ( 1 << 7 )) >> 7;
    if (!dips[0]){
        toggle_20[0]=1;
        toggle_20[1]=1;
        toggle_6=0;
        toggle_2=0;
        if(!dips[1]){
            toggle_20[0]=0;
            toggle_20[1]=1;
            toggle_6=0;
            toggle_2=1;
            if(!(dips[5]&dips[6]&dips[7])){
                toggle_20[0]=0;
                toggle_20[1]=0;
                toggle_6=1;
                toggle_2=1;
            }
        }
    } else {
        toggle_20[0]=0;
        toggle_20[1]=0;
        toggle_6=0;
        toggle_2=0;
        LED_turnOff(LED_2);
        LED_turnOff(LED_1);
    }
}

void LED_20HZ(){
    if (toggle_20[0]){LED_toggle(LED_1);}
    if (toggle_20[1]){LED_toggle(LED_2);}
}

void LED_6HZ(){
    if (toggle_6){LED_toggle(LED_2);}
}

void LED_2HZ(){
    if (toggle_2){LED_toggle(LED_1);}
}
float IIR(float a[], float y[],float b[]){
    int i;
    int16_t j;
    float out;
    out = 0.0;
    out += x[cbuf]*b[0];
    #pragma UNROLL(N_IIR_BP-1);
    for(i = 1; i<N_IIR_BP; i++){
        j = (cbuf - i) & (N_IIR_BP-1);
        out += x[j]*b[i];
        out -= y[j]*a[i];
    }
    y[cbuf] = out;
    return out;
}

