// C++ code
//
#define cpl_bit(reg, bit) (reg ^= (1 << bit))
int count = 0;
int start = 1;

int reset = 0;
int reset_count = 18;

int measurement_count = 111;
int measurement = 0;
int led = 0;

bool border = false;
bool high_frequency = false;

void setup() {
  // Configuração do Timer 1
  TCCR2A = 0;  // Desabilita a saída PWM
  TCCR2B = 0;  // Limpa o registro de controle do Timer 1
  TCNT2 = 0;   // Inicializa o contador do Timer 1 em 0
  
  DDRB = 0b00000111;
  PORTB = 0b11111000;

  // Configura o prescaler para 1
  TCCR2B |= 0b00000001;

  // Habilita a interrupção de estouro do Timer 1
  TIMSK2 |= (1 << TOIE2);

  // Habilita as interrupções globais
  sei();
}

void loop() {
  if(!high_frequency){
    if (Serial.available() > 0) {
        char incomingByte = Serial.read();
        
        if (incomingByte == '1') {
            Serial.println("Start = 1");
            start = 1;
        }
    }
  }
}

// Interrupção de estouro do Timer 1
ISR(TIMER2_OVF_vect) {
  if((count == 1 || count == 5) && border == true){
      cpl_bit(PORTB, PORTB0);
      border = !border;
    }
  if ((count == 4 || count == 8) && (border == false)) {
    cpl_bit(PORTB, PORTB0);
    border = !border;
    if(count == 8 && start == 0){
      cpl_bit(PORTB, PORTB1);
      }
    if(start != 1){
      count = 0;
    }
    if (reset == 1) {
        reset_count--;
    }
    if (reset_count == 0 && reset == 1) {
        reset = 0;
        reset_count = 18;
        measurement = 1;
    }
    if (measurement_count != 0 && measurement == 1) {
        led = 1;
        measurement_count--;
    } else if (measurement_count == 0 && measurement == 1) {
        measurement = 0;
        measurement_count = 111;
        start = 0; //
        led = 0;
    }
    if (led == 1) {
        PORTB |= (1 << PORTB2);
    } else {
        PORTB &= ~(1 << PORTB2);
    }
  }
  if (count == 6 && start == 1) {
    cpl_bit(PORTB, PORTB1);
    reset = 1;
    start = 0;
  }
  count++;
  if(high_frequency){
    TCNT2 = 255;
  }
  else{
    TCNT2 = 0;
  }
}