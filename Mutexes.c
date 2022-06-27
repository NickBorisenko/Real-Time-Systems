#include <stdio.h>
#include <stdlib.h>
#include <LPC17xx.h>
//#include "GLCD.h"
//#include "uart.h"
#include "stdio.h"
#include <time.h>
#include <cmsis_os2.h>
unsigned int ledCount=0;
unsigned int state = 1;
osMutexId_t mut;
//osWait_t wait;
const osMutexAttr_t Thread_Mutex_attr = {
	"myButtonMutex",
	0,
	NULL,
	0U
};

void countMutex()
{
	while(1){
		osMutexAcquire(mut,osWaitForever);
			
			//unsigned int state = 1;
		
			if(!(LPC_GPIO2->FIOPIN &=(1<<10))){
				if(state ==1){
					
				}
				else{
					ledCount++;
					state = 1;
					//if(ledCount > 255)
						//ledCount = 0;
				}
		}
		else{
			state = 0;
		}
		osMutexRelease(mut);
	}
}



void ledMutex()
{
	while(1){
		osMutexAcquire(mut,osWaitForever);
		int i = 0;
		int bin[8] = {0};
		unsigned int all_ones = 255;
		int eight_bit = all_ones&ledCount;
	
		for(i=7; i>=0; i--){
			bin[i]=eight_bit%2;
			eight_bit = eight_bit/2;
		}
	
	LPC_GPIO1->FIODIR |= 1<<28;
	LPC_GPIO1->FIODIR |= 1<<29;
	LPC_GPIO1->FIODIR |= 1<<31;
	
	LPC_GPIO2->FIODIR |= 1<<2;
	LPC_GPIO2->FIODIR |= 1<<3;
	LPC_GPIO2->FIODIR |= 1<<4;
	LPC_GPIO2->FIODIR |= 1<<5;
  LPC_GPIO2->FIODIR |= 1<<6;
	
	if(bin[0]==1)
		LPC_GPIO1->FIOSET |= 1<<28;
	else
		LPC_GPIO1->FIOCLR |= 1<<28;
	
	if(bin[1]==1)
		LPC_GPIO1->FIOSET |= 1<<29;
	else
		LPC_GPIO1->FIOCLR |= 1<<29;
	
	if(bin[2]==1)
		LPC_GPIO1->FIOSET |= 1<<31;
	else
		LPC_GPIO1->FIOCLR |= 1<<31;
	
	if(bin[3]==1)
		LPC_GPIO2->FIOSET |= 1<<2;
	else
		LPC_GPIO2->FIOCLR |= 1<<2;
	
	if(bin[4]==1)
		LPC_GPIO2->FIOSET |= 1<<3;
	else
		LPC_GPIO2->FIOCLR |= 1<<3;
	
	if(bin[5]==1)
		LPC_GPIO2->FIOSET |= 1<<4;
	else
		LPC_GPIO2->FIOCLR |= 1<<4;
	
	if(bin[6]==1)
		LPC_GPIO2->FIOSET |= 1<<5;
	else
		LPC_GPIO2->FIOCLR |= 1<<5;
	
	if(bin[7]==1)
		LPC_GPIO2->FIOSET |= 1<<6;
	else
		LPC_GPIO2->FIOCLR |= 1<<6;

		osMutexRelease(mut);
	}
}



void printMutex()
{
	while(1){
		osMutexAcquire(mut,osWaitForever);
		
		while(1){
			printf("%d\n", ledCount);
		}
		osMutexRelease(mut);
	}
}

int main (void)
{
	osKernelInitialize();

	
	osMutexAcquire(mut, osWaitForever);
	osMutexRelease(mut);
	
	osThreadNew(countMutex,NULL,NULL);
	osThreadNew(ledMutex,NULL,NULL);
	osThreadNew(printMutex,NULL, NULL);

	osKernelStart();

	//buttonPushed();
	//light_setup();
	while(1);
}
