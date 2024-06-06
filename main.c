#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define cpl_bit(y,bit) (y^=(1<<bit))
#define set_bit(Y,bit_x) (Y|=(1<<bit_x))
#define clr_bit(Y,bit_x) (Y&=~(1<<bit_x))
#define master_clock PB4
#define porta_start PB5
#define LED PB3

volatile int main_clock = 0;
volatile int led_turn_on_count = 18;
volatile int count_measure = 0;

volatile int measure_limit = 111;
volatile int inicial = 0;

volatile int start = 0;
volatile int reset_start = 0;
volatile int turn_led = 0;
volatile int reset_cycle = 0;
volatile int measure_cycle = 0;

//---------------------------------------------------------------------------------
ISR(TIMER2_COMPA_vect)
{
  //f = 31250Hz
  cpl_bit(PORTB, master_clock);
  main_clock = main_clock^1;
 
  if(inicial == 2){
    if(main_clock == 1){
		start = 1;
		reset_start = 1;
      if(reset_cycle == 1){
        start = 0;
        led_turn_on_count--;
      }
      if(led_turn_on_count == 0){
        turn_led = 1;
        measure_cycle = 1;
        reset_cycle = 0;
        start = 0;
        led_turn_on_count = 18;
      }
      if(turn_led == 1){
        set_bit(PORTB, LED);
      }
      if(measure_cycle == 1){
        start = 0;
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
  if((inicial < 2) && (main_clock == 1)){
  inicial = inicial + 1;
  }
}

int main(void){
	DDRB  = 0b00111000;
	PORTB = 0b11000111;

	// Configurando o Timer 2 no modo CTC
	TCCR2A = (1 << WGM21); // Configura o Timer 2 para o modo CTC (Clear Timer on Compare Match)
	TCCR2B = (1 << CS20);  // Prescaler de 1

	OCR2A = 255; // Valor de comparação para gerar uma interrupção a cada 16us (62.5ns * 256 = 16us)

	TIMSK2 = (1 << OCIE2A); // Habilitando a interrupção de comparação do Timer 2
	sei();


	while(1){
		ISR(TIMER2_COMPA_vect);
		if(start == 1){
			_delay_us(16);
			set_bit(PORTB, porta_start);
			reset_cycle = 1;
			reset_start = 0;
			start = 0;
		}
		else if(reset_start == 1){
			_delay_us(16);
			clr_bit(PORTB, porta_start);
		}

	}
}