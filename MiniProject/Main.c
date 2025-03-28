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

float buffer[32000];
//float obuffer[2000];
ulong_t ind = 0;
float x[N_IIR_BP] = {0.0};
float yb[N_IIR_BP];
float yl[N_IIR_LP];
float yh[N_IIR_HP];
int16_t s16;

LgUns start_time, end_time, duration;

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
    if (MCASP->RSLOT){
        ind = (ind+1) % (32000);

        if(!dips[0]){
            //write_audio_sample(s16);

        if(!dips[1]){
            start_time = CLK_gethtime();

            update_array(x,buffer[ind]);

                float result = 0.0;
                if(!dips[5]){
                    result += IIR(a_iir_lp,yb,b_iir_lp);
                }

                if(!dips[6]){
                    result += IIR(a_iir_bp,yl,b_iir_bp);
                }

                if(!dips[7]){
                    result += IIR(a_iir_hp,yh,b_iir_hp);
                }

                //sum filter results
                //printf("%d\n\n",result);
                write_audio_sample((int16_t)result);
                //obuffer[ind>>4] = result;
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
    int i = 0;
    for(i = 0;i<8;i++){
        dips[i]= (alldip & ( 1 << i )) >> i;
    }
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
    float out = 0.0;
    out += x[0]*b[0];
    for(i = 1; i<N_IIR_BP; i++){
        out += x[i]*b[i];
        out -= y[i-1]*a[i];
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


