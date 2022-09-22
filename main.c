#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "LCD16x2_4bit.h"
#include "DHT.h"
#include<avr/interrupt.h>

#define DHT11_PIN 6

//uint8_t a=0,I_RH,F_RH,I_Temp,F_Temp,CheckDig;


char array_mo[10];
int val_adc;
float moist;

char data[5];


int main(void){
	DDRC=0xFF;
	//PORTC|=1<<PINC0;
	_delay_ms(10);
    
    GICR |= 1<<INT0 | 1<<INT1;   // external inturrupt at PIND2 & PIND3
	MCUCR |= 1<<ISC01| 1<<ISC11; // choosing what to do when interrupts happened. in falling edge. 
	sei();


    ADC_Init();		/* initialize the ADC */

	lcdinit();					/* initialize LCD */
	lcd_clear();				/* clear LCD */
	lcd_gotoxy(0,0);
	lcd_print("Compost");
	lcd_gotoxy(0,1);
	lcd_print("   Indicator");
	
	
	val_adc = ADC_Read();	
	moist = 100-(val_adc*100.00)/1023.00; /* Calculate moisture in % */
	

    while(1){
	if(moist<40|moist>60){
		PORTC |= 1<<PINC0;
		}
		else{
		PORTC &= ~(1<<PINC0);
		
		}
	}	
}



ISR(INT0_vect){                            //Soil moisture button
		//val_adc = ADC_Read();	
		//moist = 100-(val_adc*100.00)/1023.00; /* Calculate moisture in % */
		lcd_clear();
		lcd_gotoxy(0,0);
		lcd_print("Soil Moisture: ");
		dtostrf(moist,3,2,array_mo); /* Converting floating point to string*/
		strcat(array_mo,"%   ");	/* Concatenate unit of % */
		lcd_gotoxy(0,1);		
		lcd_print(array_mo);	
		memset(array_mo,0,10);
		_delay_ms(500);
	
}



ISR(INT1_vect){                            //Humidity sensor button
		lcd_clear();
		lcd_gotoxy(0,0);			/* enter column and row position */
		lcd_print("Humidity =");
		lcd_gotoxy(0,1);
		lcd_print("Temp = ");
			
		Request();				/* send start pulse */
		Response();				/* receive response */
		I_RH=Receive_data();	/* store first eight bit in I_RH */
		F_RH=Receive_data();	/* store next eight bit in F_RH */
		I_Temp=Receive_data();	/* store next eight bit in I_Temp */
		F_Temp=Receive_data();	/* store next eight bit in F_Temp */
		CheckDig=Receive_data();/* store next eight bit in CheckSum */
		
		if ((I_RH + F_RH + I_Temp + F_Temp) != CheckDig){

			lcd_gotoxy(0,0);
			lcd_print("Error");
		}
		
		else{

			itoa(I_RH,data,10);
			lcd_gotoxy(11,0);
			lcd_print(data);
			lcd_print(".");
			
			itoa(F_RH,data,10);
			lcd_print(data);
			lcd_print("%");

			itoa(I_Temp,data,10);
			lcd_gotoxy(6,1);
			lcd_print(data);
			lcd_print(".");
			
			itoa(F_Temp,data,10);
			lcd_print(data);
			lcddata(0xDF);
			lcd_print("C ");
		}
				
	_delay_ms(500);	
}




void ADC_Init(){

	DDRA=0x0;		/* Make ADC port as input */
	ADCSRA = 0x87;	/* Enable ADC, fr/128  */	
}

int ADC_Read(){

	ADMUX = 0x40;					/* Vref: Avcc, ADC channel: 0 */
	ADCSRA |= (1<<ADSC);			/* start conversion */
	while ((ADCSRA &(1<<ADIF))==0);	/*monitor end of conversion interrupt flag */
	ADCSRA |=(1<<ADIF);				/* set the ADIF bit of ADCSRA register */
	return(ADCW);					/* return the ADCW */
}



