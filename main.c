#include "stm32l476xx.h"
#include "LCD.h"
#include "SysTick.h"

void display_time(int time_to_display);
void display_direction(int index);
void joystick_config(void);
void joystick_test(void);

int main(void){
	int index = 0;
	
	LCD_Initialization(); //initialize the LCD display
	SysTick_Initialize(1000);
	joystick_config();
	joystick_test();
	
	//direction display test
//	while(1){
//		display_direction(index);
//		index = index+1;
//		index = index %5;
//		delay(500);
//	}
	
	
	//number display test
//	int elapsed = 130689;
//	display_time(elapsed);
	
	
	
}



//Displays a direction: up, down, right, left, or pushdown
//Each direction has a corresponding index input
//Input: index of the direction to display to the LCD
//Output: Direction string on the LCD
//TESTED: WORKS
void display_direction(int index){
	
	char *up = "  UP  ";    //0
	char *down = " DOWN ";  //1
	char *right = " RIGHT"; //2
	char *left = " LEFT ";  //3
	char *push = " PUSH ";	//4
	char *error = "ERROR!"; //if wrong input, output error
	
	if(index == 0){
		LCD_DisplayString(up);
	}else if(index == 1){
		LCD_DisplayString(down);
	}else if(index == 2){
		LCD_DisplayString(right);
	}else if(index == 3){
		LCD_DisplayString(left);
	}else if(index == 4){
		LCD_DisplayString(push);
	}else{
		LCD_DisplayString(error);
	}
	
}

//Displays an amount of milliseconds in mm:ss,ss format
//input: time in milliseconds to display
//output: display to LCD
//TESTED: WORKS
void display_time(int elapsed){
	
	//variable declarations
	char minutes_1_char[1];
	char minutes_2_char[1];
	char seconds_ints_1_char[1];   //integer seconds
	char seconds_ints_2_char[1];	 //decimal seconds
	char seconds_decs_1_char[1];
	char seconds_decs_2_char[1];
	int minutes;
	int seconds_ints;
	int seconds_decs;
	int minutes_1;
	int minutes_2;
	int seconds_ints_1;
	int seconds_ints_2;
	int seconds_decs_1;
	int seconds_decs_2;
	
	
	minutes_1 = ((elapsed - (elapsed % 60000))/60000)/10;              //first 2 characters
	minutes_2 = ((elapsed - (elapsed % 60000))/60000)%10;
	seconds_ints_1 = (((elapsed % 60000) - (elapsed % 1000))/1000)/10; //middle 2 characters
	seconds_ints_2 = (((elapsed % 60000) - (elapsed % 1000))/1000)%10;
	seconds_decs_1 = ((elapsed % 1000)/10)/10;                         //last 2 characters
	seconds_decs_2 = ((elapsed % 1000)/10)%10; 
	
	//convert to characters
	*minutes_1_char = minutes_1 + '0';				
	*minutes_2_char = minutes_2 + '0';
	*seconds_ints_1_char = seconds_ints_1 + '0';
	*seconds_ints_2_char = seconds_ints_2 + '0';
	*seconds_decs_1_char = seconds_decs_1 + '0';
	*seconds_decs_2_char = seconds_decs_2 + '0';
	
	//Output to LCD
	LCD_WriteChar(minutes_1_char, 0 , 0, 0); 
	LCD_WriteChar(minutes_2_char, 0 , 1, 1);  
	LCD_WriteChar(seconds_ints_1_char, 0 , 0, 2);  
	LCD_WriteChar(seconds_ints_2_char, 1 , 0, 3); 
	LCD_WriteChar(seconds_decs_1_char, 0 , 0, 4); 
	LCD_WriteChar(seconds_decs_2_char, 0 , 0, 5);
	
}


void joystick_config(void){
  //Enable clock for GPIO port A
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	//Configure the GPIO port A as input mode 00. port PA0
	GPIOA->MODER &= ~(0x00000003);
	//PA1 mode
	GPIOA->MODER &= ~(0x0000000C);
	//PA2 mode
	GPIOA->MODER &= ~(0x00000030);
	//PA3 mode
	GPIOA->MODER &= ~(0x000000C0);
	//PA5 mode
	GPIOA->MODER &= ~(0x00000C00);
}

//Test the joystick functioning
//Each time the user presses down on the button, a counter increments 
//by 10 on the LCD.
void joystick_test(void){
	int a = 0;
	int a_prev = 0;
	int run = 0;
	int count = 0;
	while(1){
		a = ((GPIOA->IDR) & 0x00000001);
		if((a == 1)&&(a_prev==0)){
			count = count + 60000;
		}
		display_time(count);
		
		a_prev = a;
	}
}