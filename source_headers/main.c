#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#include <xc.h>
#include "breakpoints.h"
#define _XTAL_FREQ   40000000

int timer0_flag = 0;
int half_sec_flag = 0;
int adcon_flag = 0;
int rb_flag = 0;
int rb_stable_flag = 0;
int timer0_counter;
int timer1_counter;
int convertedDecimal;
int current_guess;
int isRb4High = 0;
int wasRb4HighLastInterrupt = 0;
int s5_flag = 0;
int end_game_counter;

int mapADC(){

    if (convertedDecimal <= 102)
        return 0;
    else if (convertedDecimal <= 204)
        return 1;
	else if (convertedDecimal <= 306)
        return 2;
  	else if (convertedDecimal <= 408)
        return 3;
	else if (convertedDecimal <= 510)
        return 4;
	else if (convertedDecimal <= 612)
        return 5;
	else if (convertedDecimal <= 714)
        return 6;
	else if (convertedDecimal <= 816)
        return 7;
    else if (convertedDecimal <= 918)
        return 8;
    else if (convertedDecimal <= 1023)
        return 9;

}

void __interrupt() ISR(){
    // interrupt service routine: checks timer0, timer1, adcon and rb interrupt
    // bits and set the relevant flags
    if(ADIF == 1) {
        /* For ADC interrupt */
        adcon_flag = 1 ;
        ADIF = 0 ; // interrupt flag is reset
    }
    if(TMR0IF == 1){
        //every 5ms
        if(isRb4High)
        {
            if(wasRb4HighLastInterrupt)//rb4 was pressed for at least 5ms
            {
                rb_flag = 1;
                wasRb4HighLastInterrupt = 0;
                isRb4High = 0;
            }
            else //this is the first time rb4 was high in timer 0 interrupts
            {
                wasRb4HighLastInterrupt = 1;
            }
        }
        // Timer 0 interrupt
        timer0_counter--;
        if(timer0_counter == 0){
            timer0_flag = 1;
            timer0_counter = 10;
        }
        TMR0L = 61;
        TMR0IF = 0;
        PEIE = 1;
    }
    if(TMR1IF == 1){
        // Timer 1 interrupt
        timer1_counter--;
        if(timer1_counter == 0){
            hs_passed(); //call every 500ms
            half_sec_flag = 1;
            timer1_counter = 10;
        } 
        TMR1 = 3036; 
        TMR1IF = 0;
    }
    if(RBIF == 1){
        //  rb port change interrupt
        if(PORTBbits.RB4 == 0)
        {
            //cancel rb button pressed actions
            rb_stable_flag = 0;
            isRb4High = 0;
            wasRb4HighLastInterrupt = 0;
        }
        else //button might be pressed
        {
            isRb4High = 1;            
        }
        PORTB; // read should update RBIF
        RBIF = 0;
    }
}

void Init(){
    // initialization function: configures timer0, timer1, adcon and rb
    INTCON = 0; //Interrupts disabled for now
    // Configure tmr0
    TMR0 = 0;
    T0CON = 0b11010111; // enable timer, 8-bit operation, ; falling edge, select prescaler ; with 1:256, internal source
    TMR0L = 61; //10MHZ clock -> 10^7 cycles per second -> 10^-4 ms per cycle;
    // counter can count x*256 cycles -> x*256*10^-4 ms -> x=195 for 4,992 ms -> 256-195 = 61 = '0x3d'
    timer0_counter = 10; //10*4,992= 49,92 ms is passed to the counter to count ~50ms for adcon
    TMR0IE = 1;
    
    // Configure tmr1
    TMR1 = 0;
    T1CON = 0b11110001; // enable timer, 16-bit operation, ; select prescaler ; with 1:8, internal source (Fosc/4)
    TMR1 = 3036; 
    timer1_counter = 10; //125*40= 5000 ms is passed to the counter to count 5s for endgame
    TMR1IE = 1;
    end_game_counter = 10;
            
    TRISH = 0b00010000;
    ADCON0 = 0x30; // channel 12 will be used
    ADCON1 = 0;   //input pins are analog
    ADCON2 = 0xAA; // b'10101010 12 Tad will be used

    ADON=1; // ADC module is active
    
    // set input output ports
    TRISJ = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    TRISB = 0b11111111;
    RBIE = 1;
    
    // enable interrupts
    GIE = 1;
    
    init_complete();
}

void Update7Segment(int value_to_display){
    // updates 7 segment display with value_to_display
    LATH0=1;
    switch (value_to_display){
        case -1:
            LATJ = 0; 
            break;
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
    UpdateLeds(2);
    Update7Segment(special_number());
    // Restart tmr1 for 500 ms
    TMR1 = 7000; 
    timer1_counter = 10; //125*40= 5000 ms is passed to the counter to count 5s for endgame
    half_sec_flag = 0;
    while(!half_sec_flag);
    Update7Segment(-1);
    while(!half_sec_flag);
    Update7Segment(special_number());
    while(!half_sec_flag);
    Update7Segment(-1);
    while(!half_sec_flag);
    Init();
    restart();
}

void main(void) {
    Init();
    while(1){
        // main loop: checks flags and does necessary ops
        if (timer0_flag){
            timer0_flag = 0;
            // every 50 ms
            ADIE = 1; // AD interrupt is enabled
            GODONE = 1; // AD conversion starts
        }
        if (half_sec_flag){
            // every 500 ms
            half_sec_flag = 0;
            end_game_counter--;
            if(end_game_counter == 0){
                // 5 seconds passed, game ends
                game_over();//needs to be called when no correct guess is made
                EndGame();
            }
        }
        if (adcon_flag){
            adcon_flag = 0;
            // sample and update value on 7 segment display
            convertedDecimal = ((ADRESH & 2) / 2) * 512 + (ADRESH & 1) * 256 + ADRESL; // get AD conversion result
            adc_value = convertedDecimal;
            adc_complete();
            current_guess = mapADC();
            Update7Segment(current_guess);
        }
        if (rb_flag){
            rb_flag = 0;
            rb4_handled(); // needs to be called before correct_guess() function)
            // get the guessed value, check if it is less than or grater than
            // special number, update leds accordingly
            if (current_guess < special_number())
                UpdateLeds (0);
            else if (current_guess > special_number())
                UpdateLeds (1);
            else
            {
                correct_guess(); //needs to be called after rb4_handled()
                EndGame(); // guess is correct game ends
            }
        }
    }
    return;
}
