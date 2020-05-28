# 1 "source_headers/breakpoints.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/opt/microchip/xc8/v2.10/pic/include/language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "source_headers/breakpoints.c" 2
# 1 "source_headers/breakpoints.h" 1



volatile int adc_value = 0;

void init_complete(void);
void adc_complete(void);
void rb4_handled(void);
void latjh_update_complete(void);
void latcde_update_complete(void);
void correct_guess(void);
void hs_passed(void);
void game_over(void);
void restart(void);
char special_number(void);
# 2 "source_headers/breakpoints.c" 2

volatile char special = 4;

void init_complete(){}
void adc_complete(){}
void rb4_handled(){}
void latjh_update_complete(){}
void latcde_update_complete(){}
void correct_guess(){}
void hs_passed(){}
void game_over(){}
void restart(){}
char special_number(){return special;}
