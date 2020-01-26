#include "lcd.h"

uint16 T1 = 0;
uint16 T2 = 0;
uint16 T3 = 0;

uint8 count = 0 ;

typedef enum {
	Rising=0x03,
	Falling=0x02
}Edge;

void APP_edgeProcessing(void);
void set_edge(Edge e);
void INT0_Init();
void Timer1_Init();
void stop_Interrupt ();

ISR(INT0_vect)
{
	APP_edgeProcessing();
}



int main()
{

	uint32 dutyCycle = 0;

	Timer1_Init();
	LCD_init();

	set_edge(Rising);
	INT0_Init();

	while(1)
	{
		if(count == 4)
		{
			stop_Interrupt();
			count = 0;
			LCD_displayString("Duty = ");
			dutyCycle = ((float)(T3 - T2) / (T3 - T1)) * 100;
			LCD_intgerToString(dutyCycle);
			LCD_displayCharacter('%');
		}
	}
}

void APP_edgeProcessing(void)
{
	count++;
	if(count == 1)
	{
		TCNT1 = 0;
		set_edge(Falling);
	}
	else if(count == 2)
	{
		T1 = TCNT1;
		set_edge(Rising);
	}
	else if(count == 3)
	{
		T2 = TCNT1;
		set_edge(Falling);
	}
	else if(count == 4)
	{
		T3 = TCNT1;
		TCNT1=0;
		set_edge(Rising);
	}
}

void set_edge(Edge e)
{
	MCUCR = (MCUCR & 0xFC) | e ;
}

void INT0_Init()
{
	GICR |= (1<<INT0);
	SREG |= (1<<7);
}

void Timer1_Init()
{
	DDRD &= ~(1<<PD2);
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	TCCR1B = (TCCR1B & 0xF8) | (0x01);
	TCNT1 = 0;
}

void stop_Interrupt ()
{
	SREG &= ~(1<<7);
	GICR &= ~(1<<INT0);
}
