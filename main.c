/*
 * Caroline Le cle027@ucr.edu
 * Lab Section: 21
 * Assignment: Final Project - Jukebox
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */ 

#include <avr/io.h>
#include "io.c"
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0;
// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		TCNT3 = 0;
		current_frequency = frequency;
	}
}//set_PWM end

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

unsigned char tempD = 0x00;
void Party(const double LED_note){
	//Octave 4
	const double C4 = 261.63; //do
	const double D4 = 293.66; //re
	const double E4 = 329.63; //mi
	const double F4 = 349.23; //fa
	const double G4 = 392.00; //so
	const double A4 = 440.00; //la
	const double B4 = 493.88; //ti
	
	//Octave 5
	const double C5 = 523.25; //do
	const double D5 = 587.30; //re
	const double E5 = 659.30; //mi
	const double F5 = 698.50; //fa
	const double G5 = 784.00; //so
	const double A5 = 880.00; //la
	
	tempD = PORTD & 0xC0;
	
	if(LED_note == C4 || LED_note == C5){
		PORTD = tempD | 0x01;
	}
	else if(LED_note == D4 || LED_note == D5){
		PORTD = tempD | 0x02;
	}
	else if(LED_note == E4 || LED_note == E5){
		PORTD = tempD | 0x04;
	}
	else if(LED_note == F4 || LED_note == F5){
		PORTD = tempD | 0x08;
	}
	else if(LED_note == G4 || LED_note == G5){
		PORTD = tempD | 0x10;
	}
	else if(LED_note == A4 || LED_note == A5){
		PORTD = tempD | 0x20;
	}
	//return;
}


unsigned char i = 0x00;  //maybe define this in init? - song selection (A=0, B=1, C=2
unsigned char j = 0x00;  //note count
unsigned char tempA = 0x00; //button press to Jukebox
unsigned char flag = 0x00;
enum states{welcome, initial, wait, up, down, select, pause, songA, songB, songC, songD} state;
void jukebox(){
	tempA = ~PINA & 0x0F;
	//Octave 4
	const double note_C4 = 261.63; //do
	const double note_D4 = 293.66; //re
	const double note_E4 = 329.63; //mi
	const double note_F4 = 349.23; //fa
	const double note_G4 = 392.00; //so
	const double note_A4 = 440.00; //la
	const double note_B4 = 493.88; //ti
	
	//Octave 5
	const double note_C5 = 523.25; //do
	const double note_D5 = 587.30; //re
	const double note_E5 = 659.30; //mi
	const double note_F5 = 698.50; //fa
	const double note_G5 = 784.00; //so
	const double note_A5 = 880.00; //la
	
	
	const double rest    = 0.00;   //shhhhh rest well my sweet child
	
	unsigned char songA_size = 0x1D; //Mary Had a Little Lamb [29]
	unsigned char songB_size = 0x35; //Song of Storms [53]
	unsigned char songC_size = 0x16; //Zelda's Lullaby [22]
	unsigned char songD_size = 0x3E; //Song of Healing [62]
	//define arrays for songs
	const double songA_notes [29] = {note_E4, note_D4, note_C4, note_D4, note_E4, note_E4, note_E4, rest, note_D4, note_D4, note_D4, rest, note_E4, note_E4, note_E4, rest, note_E4, note_D4, note_C4, note_D4, note_E4, note_E4, note_E4, note_E4, note_D4, note_D4, note_E4, note_D4, note_C4}; 
	const double songB_notes [53] = {note_D4, note_F4, note_D5, rest, note_D4, note_F4, note_D5, rest, note_E5, note_F5, note_E5, note_F5, note_E5, note_C5, note_A4, rest, note_A4, note_D4, note_F4, note_G4, note_A4, rest, note_A4, note_D4, note_F4, note_G4, note_E4, rest, note_D4, note_F4, note_D5, rest, note_D4, note_F4, note_D5, rest, note_E5, note_F5, note_E5, note_F5, note_E5, note_C5, note_A4, rest, note_A4, note_D4, note_F4, note_G4, note_A4, rest, note_A4, note_D4};
	const double songC_notes [22] = {note_B4, note_D5, note_A4, rest, note_G4, note_A4, note_B4, rest, note_D5, note_A4, rest, note_G4, note_A4, note_B4, rest, note_D5, note_A5, note_G5, note_D5, note_C5, note_B4, note_A4}; 
	const double songD_notes [62] = {note_B4, note_A4, note_F4, rest, note_B4, note_A4, note_F4, rest, note_B4, note_A4, note_E4, note_D4, note_E4, rest, note_F4, note_C4, note_B4, rest, note_F4, note_C4, note_B4, rest, note_F4, note_C4, note_B4, note_A4, note_B4, rest, note_B4, note_A4, note_F4, rest, note_B4, note_A4, note_F4, rest, note_B4, note_A4, note_E4, note_D4, note_E4, rest, note_A4, note_A4, note_A4, note_D4, note_G4, note_G4, note_G4, note_C4, rest, note_G4, rest, note_F4, note_F4, note_F4, note_A4, note_F4, note_E4, note_D4, note_G4, note_E4};
	//note: set_PWM(0) produces a rest for that time period
	
	switch(state){
		case welcome:
			if(tempA == 0x00){
				state = welcome;
				flag = 0; //IT BLIP
			}
			else{
				state = initial;
				flag = 0;
			}
			break;
		
		case initial:
			state = wait;
			break;
			
		case wait:
			if(tempA == 0x01){
				state = up;
				flag = 0;
			}
			if(tempA == 0x02){
				state = select;
			}
			if(tempA == 0x04){
				state = down;
				flag = 0;
			}
			break;
			
		case up:
			if(tempA != 0x01){
				state = wait;
			}
			else if(tempA == 0x01){
				state = up;
			}
			break;
			
		case down:
		if(tempA != 0x04){
			state = wait;
		}
		else if(tempA == 0x04){
			state = down;
		}
		break;
		
		case select:
			if(i == 0x00){
				state = songA;
				flag = 0;
			}
			else if(i == 0x01){
				state = songB;
				flag = 0;
			}
			else if(i == 0x02){
				state = songC;
				flag = 0;
			}
			else if(i == 0x03){
				state = songD;
				flag = 0;
			}
			else{
				state = initial;
			}
			break;
			
		case pause:
			if(tempA == 0x04 && i == 0x00){
				state = songA;
				flag = 0;
			}
			else if(tempA == 0x04 && i == 0x01){
				state = songB;
				flag = 0;
			}
			else if(tempA == 0x04 && i == 0x02){
				state = songC;
				flag = 0;
			}
			else if(tempA == 0x04 && i == 0x03){
				state = songD;
				flag = 0;
			}
			else if(tempA == 0x08){
				state = initial;
				flag = 0;
			}
			else{
				state = pause;
				flag = 0;
			}
			break;
			
		case songA:
			if((j < songA_size) && tempA != 0x08 && tempA != 0x04){
				state = songA;
			}
			else if(tempA == 0x04 && (j<songA_size) && tempA != 0x08){
				state = pause;
			}
			else{
				state = initial;
				flag = 0;
			}
		break;
		
		case songB:
		if((j < songB_size) && tempA != 0x08 && tempA != 0x04){
			state = songB;
		}
		else if(tempA == 0x04 && (j<songB_size) && tempA != 0x08){
			state = pause;
		}
		else{
			state = initial;
			flag = 0;
		}
		break;
		
		case songC:
		if((j < songC_size) && tempA != 0x08 && tempA != 0x04){
			state = songC;
		}
		else if(tempA == 0x04 && (j<songC_size) && tempA != 0x08){
			state = pause;
		}
		else{
			state = initial;
			flag = 0;
		}
		break;
		
		case songD:
		if((j < songD_size) && tempA != 0x08 && tempA != 0x04){
			state = songD;
		}
		else if(tempA == 0x04 && (j<songC_size) && tempA != 0x08){
			state = pause;
		}
		else{
			state = initial;
			flag = 0;
		}
		break;
			
	}//state-directory switch end
	
	switch(state){
		case welcome:
			set_PWM(0);
			if (flag == 0x00){
				LCD_DisplayString(1, " Press a button     to start    ");
				flag = 0x01;
			}
			break;
		
		case initial:
			i = 0x00;
			j = 0x00;
			PORTD = 0x00;
			set_PWM(0);
			
			break;
		
		case wait:
			if(i == 0){
				if (flag == 0x00){
					LCD_DisplayString(1, "Select song pls      Song A     ");
					flag = 0x01;
				}
			}
			
			else if(i == 1){
				if (flag == 0x00){
					LCD_DisplayString(1, "Select song pls      Song B     ");
					flag = 0x01;
				}
			}
			else if(i == 2){
				if (flag == 0x00){
					LCD_DisplayString(1, "Select song pls      Song C     ");
					flag = 0x01;
				}
			}
			else if(i == 3){
				if (flag == 0x00){
					LCD_DisplayString(1, "Select song pls      Song D     ");
					flag = 0x01;
				}
			}
			break;
			
		case up:
			if(i == 3){
				i = 0;
			}
			else{
				i++;
			}
			break;
			
		case down:
			if(i == 0){
				i = 3;
			}
			else{
				i--;
			}
			break;
		
		case select:
			break;
			
		case pause:
			if (flag == 0x00){
				LCD_DisplayString(1, "     PAUSED     ");
				set_PWM(0);
				flag = 0x01;
			}
			break;
		
		case songA:
			set_PWM(songA_notes[j]);
			Party(songA_notes[j]);
			j++;
			if (flag == 0x00){
				LCD_DisplayString(1, "Now playing     Song A");
				flag = 0x01;
			}
			break;
		
		case songB:
			set_PWM(songB_notes[j]);
			Party(songB_notes[j]);
			j++;
			if (flag == 0x00){
				LCD_DisplayString(1, "Now playing     Song B");
				flag = 0x01;
			}
			break;
		
		case songC:
			set_PWM(songC_notes[j]);
			Party(songC_notes[j]);
			j++;
			if (flag == 0x00){
				LCD_DisplayString(1, "Now playing     Song C");
				flag = 0x01;
			}
			break;
		
		case songD:
		set_PWM(songD_notes[j]);
		Party(songD_notes[j]);
		j++;
		if (flag == 0x00){
			LCD_DisplayString(1, "Now playing     Song D");
			flag = 0x01;
		}
		break;
		
	}//action switch end
	
}//void jukebox end

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; //Button presses
	DDRB = 0xFF; PORTB = 0x00; //Speaker
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines and LEDs
	
	const unsigned long timerPeriod = 250;
	state = welcome;
	TimerSet(timerPeriod);
	TimerOn();
	
	// Initializes the LCD display
	LCD_init(); 
	PWM_on();
	
	while(1) {
		jukebox();
		while(!TimerFlag);
		TimerFlag = 0;
		
		}//while(1) end
}//main end
