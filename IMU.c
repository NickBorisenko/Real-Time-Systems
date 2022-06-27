#include <stdio.h>
#include <stdlib.h>
#include <LPC17xx.h>
#include "stdio.h"
#include <cmsis_os2.h>

#include "MPU9250.h"
#include "sensor_fusion.h"



unsigned int ledCount=0;
float gx,gy,gz,ax,ay,az,mx,my,mz;
osMutexId_t mut;

const osMutexAttr_t Thread_Mutex_attr = {
	"myButtonMutex",
	0,
	NULL,
	0U
};


void ledMutex() //Prints Binary represnentation on LED's from ledCount
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

void agmStore(){
	while(1){
		osMutexAcquire(mut,osWaitForever);
		while(1){
			MPU9250_read_gyro(); //Updating the IMU device values
			MPU9250_read_acc();
			MPU9250_read_mag();
			gx = MPU9250_gyro_data[0]; //Saving each measurement value from IMU
			gy = MPU9250_gyro_data[1];
			gz = MPU9250_gyro_data[2];
			ax = MPU9250_accel_data[0];
			ay = MPU9250_accel_data[1];
			az = MPU9250_accel_data[2];
			mx = MPU9250_mag_data[0];
			my = MPU9250_mag_data[1];
			mz = MPU9250_mag_data[2];
		}
		osMutexRelease(mut);
	}
	
}

void senseFusion(){
	while(1){
		osMutexAcquire(mut,osWaitForever);
		while(1){
			sensor_fusion_update(gx,gy,gz,ax,ay,az,mx,my,mz); 

		}
		osMutexRelease(mut);
	}
}
void printVal(){
		while(1){
			osMutexAcquire(mut,osWaitForever);
			while(1){
				printf("%f,%f,%f\n",sensor_fusion_getRoll(),
				sensor_fusion_getPitch(),sensor_fusion_getYaw());
			}
			osMutexRelease(mut);
		}
}
int main(void){
	
	MPU9250_init(1,1);  //Setting up the IMU and Sensor Fusion
	MPU9250_st_value|= 1<<4;
	sensor_fusion_init();
	sensor_fusion_begin(8192.0f);
	osKernelInitialize();

	osMutexAcquire(mut, osWaitForever);
	osMutexRelease(mut);
	osThreadNew(ledMutex,NULL,NULL); //Threads
	osThreadNew(agmStore,NULL,NULL);
	osThreadNew(senseFusion,NULL,NULL);
	osThreadNew(printVal,NULL,NULL);

	osKernelStart();
	while(1);
}