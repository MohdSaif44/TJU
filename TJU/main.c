#include "STC89xxx.h"
#include <intrins.h>
#include "math.h"
#include "stdio.h"
#define STRAIGHT_COND !P20 && !P21 && !P22 && !P23
#define STRAIGHT_COND_2 P21 && P22
#define STOP_COND P20 && P21 && P22 && P23
#define LEFT_COND (P20) && !P22 && !P23
#define RIGHT_COND (P23) && !P20 && !P21
#define SMALL_LEFT_COND  P21 && !P20 && !P22 && !P23
#define SMALL_RIGHT_COND  !P21 && !P21 && P22 && !P23

sbit center_left_sensor = P2^1; 		// If I see blackline
sbit right_sensor = P2^3; 					// If I see blackline
sbit center_right_sensor = P2^2; 		// If I see blackline
sbit left_sensor = P2^0; 						// If I see blackline
sbit TEST_LED = P1^0;
sbit pwm_pin_one = P1^6;
sbit pwm_pin_two= P1^7;
sbit start = P3^5;
sbit restart = P3^4;
sbit trig =P2^6;
sbit echo =P2^7;
sbit buzzer = P2^5;

int pwm_count;
int count = 1000;
int speedL = 3;
int speedR = 3;
int start_flag = 1;
int obstacle_flag = 1;
int boost_flag = 1;
int restart_flag = 0;
float sum;
float distance_measurement;
float distance_compare;


void Delay100ms();
void Delay1ms(unsigned int ms);
void Timer0_Init(void);
void PWM_generation(void);
void straight(void);
void left(void);
void right(void);
void stop(void);
void ultra_sonic_handler(void);
void Delay20us(void);	
void reverse(void);
void Timer1_Init(void);
void Delay10us(void);
void Auto_Car(void);



void main(){
	
	while(1){ 
		if(!restart){
			restart_flag = 1;
			Timer0_Init();
			Timer1_Init();
			TEST_LED = 0;
			boost_flag = 0;
			while(1){
				Auto_Car();
			}
		}
		if(!start){
			Timer0_Init();
			Timer1_Init();
			TEST_LED = 0;
			while(1){
				Auto_Car();
	 }
	}
 }
}

/*void extern_0_INT(void){	
	EX0 = 1; // Enable INT0
  IT0 = 1; // Set Falling Edge Trigger for INT0
}

void ISR_extern_0(void) interrupt 0 // ISR for external interrupt INT0
{
	 ultra_sonic_handler();
	 if(distance_compare >= 4.0 && distance_compare <= 5.5 && obstical_flag){
			right();
			Delay1ms(500);
			left();
			Delay1ms(500);
			straight();
			obstical_flag = 0;
	}
}*/

void Auto_Car()
{
	PWM_generation();
	 if(obstacle_flag){
			//	ultra_sonic_handler();
	 }
	 if(distance_compare >= 5.0  && distance_compare <= 22.5 && obstacle_flag){
				P15 = !P15;
				right();
				Delay1ms(240);
				straight();
				Delay1ms(390);
				left();
				Delay1ms(430);
				straight();
				obstacle_flag = 0;
		 }
		 if(STRAIGHT_COND){
		    straight();
		}
		 if(STRAIGHT_COND_2){
				straight();
			 while(STRAIGHT_COND_2);
		 }
		 if(SMALL_LEFT_COND){
			  left();
			  while(!STRAIGHT_COND);
			  //Delay1ms(20);
		 }
		 if(SMALL_RIGHT_COND){
			  right();
			 while(!STRAIGHT_COND);
			 //Delay1ms(20);
		 }
		if(LEFT_COND){
				left();
				Delay1ms(30);	
		    while(!center_left_sensor);
			
		 }
		 if(RIGHT_COND){
			  right();
			 	Delay1ms(30);
			  while(!center_right_sensor);
		}
}

void timer0_update() interrupt 1
{
		TR0 = 0;
		TL0 = 0xAE;				//Initial timer value
		TH0 = 0xFB;		  //reload timer0 high byte
	
		count++;
	
		if(count >= 6400 && boost_flag){
			buzzer = 0;
		}
		if(count >= 6500){
		TEST_LED = 1;
		boost_flag = 0; 
		buzzer = 1;      
	 }
		if(count >= 19100 && !restart_flag){
		TEST_LED = 0;
		//boost_flag = 1; 
		buzzer = 0;      
	 }
		if(count >= 19200){
			buzzer = 1;
		}
		if(count >= 25200){
			buzzer = 1;
			boost_flag = 0;
		}
	  pwm_count++;
	  PWM_generation();
	if (pwm_count >= 10){
		pwm_count = 0;
	}
		TR0 = 1;  
}


void Delay100ms()		//@12MHz0
{
	unsigned char i, j;

	i = 195;
	j = 138;
	do
	{
		while (--j);
	} while (--i);
}

void Timer0_Init(void)		//100us @ 12MHz
{
	AUXR |= 0x80;			//imer clock is 1T mode
	TMOD &= 0xF0;			//Set timer work mode
	TMOD |= 0x01;			//Set timer work mode
	TL0 = 0xAE;				//Initial timer value
	TH0 = 0xFB;				//Initial timer value
	TF0 = 0;				  //Clear TF0 flag
	ET0 = 1;        
	EA  = 1;         
	TR0 = 1;				  //Timer0 start run
}

void PWM_generation(void)
{
	if (pwm_count < speedL){
		pwm_pin_one = 1;
	}else{
	  pwm_pin_one = 0;
	}
	if (pwm_count < speedR){
		pwm_pin_two = 1;
	}else{
		pwm_pin_two = 0;
	}
}
void reverse(void){
  P11 = 0x00;
	P12 = 0x01;
	P13 = 0x00;
	P14 = 0x01;
	speedL = 5;
	speedR = 5;
}
void straight(void){
  P11 = 0x01;
	P12 = 0x00;
	P13 = 0x01;
	P14 = 0x00;
	speedL = 6;
	speedR = 6;
	if(boost_flag){
		speedL++;
		speedR++;
		speedL++;
		speedR++;
	}
}
void right(void){
	P11 = 0x01;
	P12 = 0x00;
	P13 = 0x00;
	P14 = 0x01;
	speedR = 7;
	speedL = 6;
	if(boost_flag){
		speedL++;
		speedR++;
		speedL++;
		speedR++;
	}
}
void left(void){
	P11 = 0x00;
	P12 = 0x01;
	P13 = 0x01;
	P14 = 0x00;
	speedR = 6;
	speedL = 7;
	if(boost_flag){
		speedL++;
		speedR++;
		speedL++;
		speedR++;
	}
}
void stop(void){
	speedL = 0;
	speedR = 0;
}

void Delay1ms(unsigned int ms)		//@12.000MHz
{
	while(ms){
	unsigned char i, j;
		i = 2;
		j = 239;
		do
		{
			while (--j);
		} while (--i);
		ms--;
	}
}

void Delay300ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 13;
	j = 156;
	k = 83;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void Delay20us()		//@11.0592MHz
{
	unsigned char i;

	i = 6;
	while (--i);
}
void Timer1_Init(void){
	AUXR &= 0xBF;	
	TMOD &= 0X0F;
	TMOD |= 0X10;
	TL1 = 0x00;				//Initial timer value
	TH1 = 0xFF;	
}
void ultra_sonic_handler(void)
{
	
	trig=1;
	Delay1ms(10);
	trig=0;
	TL1 = 0;
	TH1 = 0;
	while(!echo);
	TR1=1;
	while(echo);
	TR1=0;
	
	distance_measurement = (TL1+(TH1<<8));	// read timer for time count 
	distance_compare = (distance_measurement*0.00034*100)/2.0;  // find distance(in cm) 
	
}

void Delay10us()		//@11.0592MHz
{
	unsigned char i;

	i = 2;
	while (--i);
}