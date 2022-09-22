#define DHT11_PIN 6
uint8_t a=0,I_RH,F_RH,I_Temp,F_Temp,CheckDig;

void Request(){                                     /* Microcontroller send start pulse or request */
    						
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);		/* set to low pin */
	_delay_ms(20);					/* wait for 20ms */
	PORTD |= (1<<DHT11_PIN);		/* set to high pin */
}

void Response(){                                                        /* receive response from DHT11 */
    						
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
}

uint8_t Receive_data(){                                             /* receive data */
    								
	for (int i=0; i<8;i++){

		while((PIND & (1<<DHT11_PIN)) == 0);	/* check received bit 0 or 1 */
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN))				/* if high pulse is greater than 30ms */
		a = (a<<1)|(0x01);						/* then its logic HIGH */
		else									/* otherwise its logic LOW */
		a = (a<<1);
		while(PIND & (1<<DHT11_PIN));
	}
	return a;
}