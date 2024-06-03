#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h> /*biblioteca para o uso das rotinas de
 _delay_ms() e _delay_us()*/

#define cpl_bit(y,bit) (y^=(1<<bit))
#define set_bit(Y,bit_x) (Y|=(1<<bit_x)) 
#define clr_bit(Y,bit_x) (Y&=~(1<<bit_x))
#define master_clock PB5
#define porta_start PB4
#define LED PB3

int main_clock = 0;
int led_turn_on_count = 18;
int count_measure = 0;

int measure_limit = 111;
int init = 0;

int start = 0;
int reset_start = 0;
int turn_led = 0;
int reset_cycle = 0;
int measure_cycle = 0;


//----------------------------------------------------------------------------------
ISR(TIMER0_OVF_vect)
{
	//count == 2 f = 31250Hz
	cpl_bit(PORTB, master_clock);
	main_clock = ~main_clock;
	
	if(init == 2){
		reset_start = 1;
		if(main_clock == 1){
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
				start = 1;
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
	if((init < 2) && (main_clock == 1)){
	init = init + 1;
	start = 1;
	}
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
		if(start == 1){
			_delay_us(16);
			set_bit(PORTB, porta_start);
			reset_start = 0;
			start = 0;
		}
		if(reset_start == 1){
			_delay_us(16);
			clr_bit(PORTB, porta_start);
		}
		
		ISR(TIMER0_OVF_vect);
	}
}