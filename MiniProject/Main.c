//bios and framework files
#include "Maincfg.h"
#include "framework.h"

//IIR filter weights
#include "IIR_coeffs/bp.h"
#include "IIR_coeffs/lp.h"
#include "IIR_coeffs/hp.h"

//function declarations
#include "Main.h"

//for profiling
#include <clk.h>

//current switch states
uint32_t alldip;
uint8_t dip_1, dip_2, dip_6, dip_7, dip_8;
Bool dips[8];

//which LEDs to toggle at what frequency
Bool toggle_20[2];  // [0] -> LED1, [1] -> LED2
Bool toggle_6;      // only LED2
Bool toggle_2;      // only LED1

//4s input circular buffer
float buffer[32000];

//smaller output buffer for testing
//commented out for effeciency
//float obuffer[2000];

//circular buffer index initialized to 0
ulong_t ind = 0;

//last few input samples and last few output samples from each filter
//can share the input array since they are all the same size
float x[N_IIR_BP] = {0.0};
float yb[N_IIR_BP];
float yl[N_IIR_LP];
float yh[N_IIR_HP];

//variable for current input sample
int16_t s16;

//stores times for profiling
LgUns start_time, end_time, duration;

void main(void){
    //initialize arrays to 0
    int i;
    for(i = 0;i<N_IIR_BP;i++){x[i]=0.0;yb[i]=0.0;yl[i]=0.0;yh[i]=0.0;}
    for(i = 0;i<32000;i++){buffer[i]=0.0;}
    //for(i = 0;i<2000;i++){obuffer[i]=0.0;}

    //initialize the switches
    DIP_getAll(&alldip);
    for(i = 0;i<8;i++){
        dips[i]= (alldip & ( 1 << i )) >> i;
    }

    //initialize all interupts
    initAll();
    //DO NOT PUT CODE HERE!!!
    return;
    // return to BIOS scheduler
}

//called whenever the hardware receives an audio sample
void audioHWI(void){
    //current input sample
    s16 = read_audio_sample();
    
    //left or right channel
    if (MCASP->RSLOT){
        //update index
        ind = (ind+1) % (32000);

        //switch S2:1
        if(!dips[0]){

            //switch S2:2
            if(!dips[1]){
                //time before filtering for profiling
                start_time = CLK_gethtime();
    
                //move value from 4s buffer to input array and move all other values along array
                update_array(x,buffer[ind]);

                float result = 0.0;

                //add outputs from IIR filters to output
                //filters used based on switch states
                //switch S2:6
                if(!dips[5]){
                    result += IIR(a_iir_lp,yb,b_iir_lp);
                }
                //switch S2:7
                if(!dips[6]){
                    result += IIR(a_iir_bp,yl,b_iir_bp);
                }
                //switch S2:8
                if(!dips[7]){
                    result += IIR(a_iir_hp,yh,b_iir_hp);
                }
                //write the resulting sample to output
                write_audio_sample((int16_t)result);

                //check how long the filters took and log it for profiling
                end_time = CLK_gethtime();
                if (start_time>end_time) duration = start_time - end_time;
                else duration =  end_time-start_time;

                LOG_printf(&trace, "ms: %d --- ticks: %d", duration/CLK_countspms(), duration);
            }
            else{
                //add input to 4s buffer and write to output
                write_audio_sample(s16);
                buffer[ind] = (float)s16;
            }
        }
        else{
            //do nothing
            //must write output sample every interrupt
            write_audio_sample(0);
        }
    }
    else {
        //do nothing
        //must write output sample every interrupt
        write_audio_sample(0);
    }


}

//called every 1s by prd
void DIP_UPDATE(){
    //get all switch states - active low
    DIP_getAll(&alldip);
    //change the current switch states to boolean
    int i = 0;
    for(i = 0;i<8;i++){
        dips[i]= (alldip & ( 1 << i )) >> i;
    }

    //work out which LEDs to toggle based on the switch states
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

//functions to toggle LEDs based on the switch states

//called every 50ms by prd
void LED_20HZ(){
    if (toggle_20[0]){LED_toggle(LED_1);}
    if (toggle_20[1]){LED_toggle(LED_2);}
}

//called every 167ms by prd
void LED_6HZ(){
    if (toggle_6){LED_toggle(LED_2);}
}

//called every 500ms by prd
void LED_2HZ(){
    if (toggle_2){LED_toggle(LED_1);}
}

//function to run an IIR filter on a single input sample from buffer
float IIR(float a[], float y[],float b[]){
    //position in filter weights
    int i;
    float out = 0.0;

    //seperate since first a value isnt used
    out += x[0]*b[0];

    //all filter lengths are the same so one loop can be used for all
    for(i = 1; i<N_IIR_BP; i++){
        //multiply the inputs by the filter weights and add to the output
        out += x[i]*b[i];
        out -= y[i-1]*a[i];
    }
    //add result to the output array and move other values one along the array
    update_array(y,out);
    return out;
}

//for updating the inputs/outputs for the IIR filters
void update_array(float arr[], float new){
    int i = 0;
    //one length loop can be used for all since filter lengths are all the same
    for(i = N_IIR_LP-1; i>0; i--){
        //move each value along the array
        arr[i] = arr[i-1];
    }
    //add the new value to the start of the array
    arr[0] = new;
