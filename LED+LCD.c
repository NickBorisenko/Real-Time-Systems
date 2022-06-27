#include <stdio.h>
#include <stdlib.h>
#include <LPC17xx.h>
#include "GLCD.h"
#include "uart.h"
#include "stdio.h"
#include <time.h>
#include <cmsis_os2.h>

void LED_setup(unsigned int n)
{
	int i = 0;
	int bin[8] = {0};
	unsigned int all_ones = 255; // 255 is 1111 1111 in binary.
	int eight_bit = all_ones&n; // Saves only the last 8 bits of integer n.
	
	for(i=7; i>=0; i--){
		bin[i]=eight_bit%2;
		eight_bit = eight_bit/2;  //converts the number from deciaml to binary.
	}
	
	//Initailizes pins on the board.
	LPC_GPIO1->FIODIR |= 1<<28;
	LPC_GPIO1->FIODIR |= 1<<29;
	LPC_GPIO1->FIODIR |= 1<<31;
	
	LPC_GPIO2->FIODIR |= 1<<1;
	LPC_GPIO2->FIODIR |= 1<<2;
	LPC_GPIO2->FIODIR |= 1<<3;
	LPC_GPIO2->FIODIR |= 1<<4;
  	LPC_GPIO2->FIODIR |= 1<<5;
	
	//Lights up the LED if the respective bit is turned.
	if(bin[0]==1)
		LPC_GPIO1->FIOSET |= 1<<28;
	if(bin[1]==1)
		LPC_GPIO1->FIOSET |= 1<<29;
	if(bin[2]==1)
		LPC_GPIO1->FIOSET |= 1<<31;
	if(bin[3]==1)
		LPC_GPIO2->FIOSET |= 1<<2;
	if(bin[4]==1)
		LPC_GPIO2->FIOSET |= 1<<3;
	if(bin[5]==1)
		LPC_GPIO2->FIOSET |= 1<<4;
	if(bin[6]==1)
		LPC_GPIO2->FIOSET |= 1<<5;
	if(bin[7]==1)
		LPC_GPIO2->FIOSET |= 1<<6;
}

char* jp_button(){ //Reads input from the joystick and pushbutton and prints to console.
	while(1){
		if(!(LPC_GPIO1->FIOPIN &=(1<<20)))
			return "JOYSTICK_PUSHED";
		
		if(!(LPC_GPIO1->FIOPIN &=(1<<23)))
			return "LEFT";
		
		if(!(LPC_GPIO1->FIOPIN &=(1<<24)))
			return"UP";
		
		if(!(LPC_GPIO1->FIOPIN &=(1<<25)))
			return"RIGHT";
		
		if(!(LPC_GPIO1->FIOPIN &=(1<<26)))
			return"DOWN";
		
		if(!(LPC_GPIO2->FIOPIN &=(1<<10)))
			return "PUSHBUTTON_PUSHED";
	}
}

void analogIn(){ //Reads the analog input and prints out the voltage value.
	unsigned int tmp2 = (0xFFF<<4);
	unsigned int tmp3;
	LPC_PINCON->PINSEL1 &= ~(1 << 18);
	LPC_PINCON->PINSEL1 &= ~(1 << 19);
	LPC_PINCON->PINSEL1 |= 1<<18;
	LPC_SC->PCONP |= 1<<12;
	LPC_ADC->ADCR = (1<<2)|(4<<8)|(1<<21);
	LPC_ADC->ADCR |= 1<<24;
	
	while(1){
		LPC_ADC->ADCR &= ~(1 << 24);
		LPC_ADC->ADCR |= 1<<24;
		tmp2 = (0xFFF<<4); //Moves and sequence of 1's to occupy bits 4-15.
		tmp2 &= LPC_ADC->ADGDR; //Extracts the value of bits 4-15.
		tmp3 = tmp2 >>4; //Moves the bits so it occupy's 0-11.
		printf("%d\n",tmp3);
		osThreadYield();
	}
}

void LCD(){
	GLCD_Clear( Blue );
	GLCD_SetTextColor ( White );
	GLCD_SetBackColor ( Blue );
	GLCD_DisplayString( 1, 1, 1, "Hello world!" );

}

char* pushedDown(){
	while(1){
		if(!(LPC_GPIO1->FIOPIN &=(1<<20)))
			return "PRESSED"; //Checks to see if Joystick is Pushed and prints result.
		else
			return "NOT PRESSED";
	}
}

char* joyDirection(){
	while(1){ //Prints the direction of the Joystick.
		if(!(LPC_GPIO1->FIOPIN &=(1<<23)))
			return "LEFT";
		else if(!(LPC_GPIO1->FIOPIN &=(1<<24)))
			return"UP";
		else if(!(LPC_GPIO1->FIOPIN &=(1<<25)))
			return"RIGHT";
		else if(!(LPC_GPIO1->FIOPIN &=(1<<26)))
			return"DOWN";
		else
			return"NO DIR";
	}
}

void jp_LCD(){
	while(1){ //Printing "Hello World" on the LCD screen.
		GLCD_Clear( Blue );
		GLCD_SetTextColor ( White );
		GLCD_SetBackColor ( Blue );

		GLCD_DisplayString( 1, 1, 1, joyDirection());
		GLCD_DisplayString( 5, 1, 1, pushedDown());
	}
}

void buttonLED(){
	int toggle;
	LPC_GPIO1->FIODIR |= 1<<28;
	while(1){
		
		if(!(LPC_GPIO2->FIOPIN &=(1<<10))&& toggle == 1){ //checks the previous state of the LED
			toggle = 0;
		}
		else if(!(LPC_GPIO2->FIOPIN &=(1<<10)) && toggle == 0){
			toggle = 1;
		}
		while(!(LPC_GPIO2->FIOPIN &=(1<<10)));


		if(toggle == 1)
		{
			LPC_GPIO1->FIOSET |= 1<<28; //Turns ON LED if button is clicked and LED was OFF
		}
		else if(toggle == 0)
		{
			LPC_GPIO1->FIOCLR |= 1<<28; //Turns OFF LED if button is clicked and LED was ON
		}
	}
}

int main (void)
{
	SystemInit();
	GLCD_Init();

	//LED_setup(123);
	//jp_button());
	//analogIn();
	//LCD();
	
	osKernelInitialize();
	osThreadNew(analogIn, NULL, NULL);
	osThreadNew(jp_LCD, NULL, NULL);
	osThreadNew(buttonLED, NULL, NULL);
	printf("-------\n");
	osKernelStart();
	while(1);
}