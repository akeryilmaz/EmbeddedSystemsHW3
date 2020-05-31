#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#include <xc.h>
#include "breakpoints.h"

int timer0_flag = 0; 
int timer1_flag = 0;
int adcon_flag = 0;
int rb_flag = 0;

void __interrupt() ISR(){
    // interrupt service routine: checks timer0, timer1, adcon and rb interrupt
    if(ADIF == 1) {
		/* For ADC interrupt */
		adcon_flag = 1 ;
		ADIF = 0 ; // interrupt flag is reset
        
	}
    // bits and set the relevant flags
}

void Init(){
    // initialization function: configures timer0, timer1, adcon and rb 
    
    
    ADCON0 = 0x30; // channel 12 will be used
	ADCON1 = 0;   //input pins are analog
	ADCON2 = 0x82; // ? not sure for t_ad which will change those 3 bits:10---010
    
    ADON=1; // ADC module is active
    // interrupts
}

void Update7Segment(int value_to_display){
    // updates 7 segment display with value_to_display
    LATH0=1;
    switch (value_to_display){
        case 0:    
            LATJ = 63; // = '00111111'
            break;
        case 1:
            LATJ = 6; // = '00000110'
            break;
        case 2:   
            LATJ = 91; // = '01011011'
            break;
        case 3:    
            LATJ = 79; // = '01001111'
            break;
        case 4:    
            LATJ = 102; // = '01100110'
            break;
        case 5:    
            LATJ = 109; // = '01101101'
            break;
        case 6:   
            LATJ = 125; // = '01111101'
            break;
        case 7:    
            LATJ = 7; // = '00000111'
            break;
        case 8:    
            LATJ = 127; // = '01111111'
            break;
        case 9:    
            LATJ = 111; // = '01101111'
            break;
    }
    LATH0 = 0;
    latjh_update_complete();
}

void UpdateLeds(int down_up){
    // updates leds with down or up arrow according to down_up,
    // 0 means down, 1 means up, 2 means show no led
    if (down_up==0){
        //show down arrow
        LATD0=1;
        LATD1=1;
        LATD2=1;
        LATD3=1;
        
        LATC1=0;
        LATE1=0;
        LATC2=1;
        LATE2=1;
    }
    else if(down_up==1){
        //show up arrow
        LATD0=1;
        LATD1=1;
        LATD2=1;
        LATD3=1;
        
        LATC1=1;
        LATE1=1;
        LATC2=0;
        LATE2=0;
    }
    else { // down_up == 2
        // show no arrow
        LATD0=0;
        LATD1=0;
        LATD2=0;
        LATD3=0;
        
        LATC1=0;
        LATE1=0;
        LATC2=0;
        LATE2=0;
    }
    latcde_update_complete();
}

void EndGame(){
    // Game ends,display the special number on 7-segment display for 2 seconds.
}

void Restart(){
    // game restarts, resets the configurations
}

void main(void) {
    Init();
    while(1){
        // main loop: checks flags and does necessary ops
        if (timer0_flag){
            // adcon starts
        }
        if (timer1_flag){
            // 5 seconds passed, game ends
            EndGame();
            Restart();
        }
        if (adcon_flag){
            // sample and update value on 7 segment display
            int current_value;
            Update7Segment(current_value);
            adcon_flag = 0;
        }
        if (rb_flag){
            // get the guessed value, check if it is less than or grater than 
            // special number, update leds accordingly
            int guess;
            if (guess < special_number())
                UpdateLeds (0);
            else if (guess > special_number())
                UpdateLeds (1);
            else
                continue; //not implemented
                // guess is correct game ends
        }
    }
    return;
}
