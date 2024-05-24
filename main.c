#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#define cpl_bit(y,bit) (y^=(1<<bit))
#define set_bit(Y,bit_x) (Y|=(1<<bit_x)) 
#define clr_bit(Y,bit_x) (Y&=~(1<<bit_x))
#define master_clock PB5
#define porta_half_clock PB4
#define LED PB3

int count = 1;
int led_turn_on_count = 18;
int count_measure = 0;

int measure_limit = 111;
int init = 0;

int start = 0;
int turn_led = 0;
int reset_cycle = 0;
int measure_cycle = 0;


//----------------------------------------------------------------------------------
ISR(TIMER0_OVF_vect)
{
	//count == 2 f = 31250Hz
	if(count == 2){
		cpl_bit(PORTB, master_clock);
	}
	
	if(init == 1){
		if((start == 1) && (count == 1)){
			clr_bit(PORTB, porta_half_clock);	
		}
		if((count == 1) && (start == 0)){
			set_bit(PORTB, porta_half_clock);
			start = 1;
			reset_cycle = 1;
		}

		if(count == 2){
			if(reset_cycle == 1){
				led_turn_on_count--;
			}
			if(led_turn_on_count == 0){
				turn_led = 1;
				measure_cycle = 1;
				reset_cycle = 0;
				led_turn_on_count = 18;
			}
			if(turn_led == 1){
				set_bit(PORTB, LED);
			}
			if(measure_cycle == 1){
				count_measure++;
			}
			if(count_measure == measure_limit + 1){
				start = 0;
			}
			if(count_measure == measure_limit + 2){
				measure_cycle = 0;
				turn_led = 0;
			}
			if(turn_led == 0){
				clr_bit(PORTB, LED);
			}
		}
	}
	if((init == 0) && (count == 2)){
	init = 1;
	}
	if(count == 2){
		count = 0;
	}
	count++;
}



int main(void)
{
	DDRB  = 0b00111000;
	PORTB = 0b11000111;

	TCCR0B = 0b00000001;
	TCNT0 = 0;
	
	TIMSK0 = 1<<TOIE0;
	sei();

	while(1){
		
		ISR(TIMER0_OVF_vect);
	}
}
