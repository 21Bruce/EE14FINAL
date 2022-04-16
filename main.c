#include "stm32l476xx.h"
#include "LCD.h"
#include "SysTick.h"
#include <stdlib.h>

void display_time(int time_to_display);
void display_direction(int index);
void joystick_config(void);
void joystick_test(void);

void movingString(uint8_t* str, uint8_t len);
void start_screen(void);
void count_down(void);
void game_loop(void);
int random_direction_gen(void);


int main(void){
	LCD_Initialization(); //initialize the LCD display
	SysTick_Initialize(1000);
	joystick_config();
	
	start_screen();
	count_down();

	game_loop();
}

int random_direction_gen(void){
	int rand_num = rand(); // gen random number
	rand_num %= 10; // take the ones place, this gives us a random number in the range [0, 10)
	rand_num /= 2.5; // divide by 2.5, this shortens the range to [0,4).
	return rand_num; // return an int, this floors the range, giving use the desired [0,3]
}
void game_loop(void){
	char num[1];
	while(1) {
		*num = '0' + random_direction_gen();
		LCD_DisplayString(num);
		delay(100);
	}
}
void count_down(void){
	int i;
	char num[4];
	num[0] = ' ';
	num[1] = ' ';
	num[2] = ' ';
	for(i = 5; i > 0; i--){
		num[3] = '0' + i;
		LCD_DisplayString(num);
		delay(500);
	}
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
	
	switch (index) {
		case 0:
			LCD_DisplayString(up);
			break;
		case 1:
			LCD_DisplayString(down);
			break;
		case 2:
			LCD_DisplayString(right);
			break;
		case 3:
			LCD_DisplayString(left);
			break;
		case 4:
			LCD_DisplayString(push);
			break;
		default:
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
	
	//Add pulldown resistors to the lateral buttons
	GPIOA->PUPDR &= ~(0x00000CFF);
	GPIOA->PUPDR |= (0x000008AA);
	
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
//depending on direction, different increment
//TESTED: WORKS
void joystick_test(void){
	int a = 0;
	int b = 0;
	int c = 0;
	int d = 0;
	int e = 0;
	int a_prev = 0;
	int b_prev = 0;
	int c_prev = 0;
	int d_prev = 0;
	int e_prev = 0;
	
	int count = 0;
	
	while(1){
		a = ((GPIOA->IDR) & 0x00000001); //0th bit
		b = ((GPIOA->IDR) & 0x00000002) >> 1; //1nd bit
		c = ((GPIOA->IDR) & 0x00000004) >> 2; //2rd bit
		d = ((GPIOA->IDR) & 0x00000008) >> 3; //3rd bit
		e = ((GPIOA->IDR) & 0x00000020) >> 5; //4rd bit
		
		if((a == 1)&&(a_prev==0)){
			count = count + 60000;
		}
		
		if((b == 1)&&(b_prev==0)){
			count = count + 10000;
		}
		
		if((c == 1)&&(c_prev==0)){
			count = count + 1000;
		}
		
		if((d == 1)&&(d_prev==0)){
			count = count + 100;
		}
		
		if((e == 1)&&(e_prev==0)){
			count = count + 10;
		}
		
		display_time(count);
		
		
		
		
		
		a_prev = a;
		b_prev = b;
		c_prev = c;
		d_prev = d;
		e_prev = e;
		
	}
}



void movingString(uint8_t* str, uint8_t len){
	int a = 0;
	int position = 0;
	int j = 0;
	int i = 0;
	//copy of the input with more space added for space padding
	int length = len;
	char* str_pad; //string to be padded with spaces
	while(length%5 != 0){
		length = length + 1;
	}
	length = length + 4;  //add some more space in order to reverse scroll later
	
	str_pad = (char *) malloc(length); //allocate more space
	
	
	while(1){
		//copy the contents of str into str_pad. 
		
		for(i = 0; i <= len; i++){
			str_pad[i] = str[i];
		}
		//add the spaces
		i = len;
		while(i != length){
			str_pad[i] = ' ';
			i = i + 1;
		}
		
		
		//shift the ensemble of elements display to the right to give the illusion of scrolling to the left. 
		for(i = 0; i <= length; i++){
			LCD_DisplayString(str_pad+i);		
			
			for(j = 0; j < 100000; j++){
		    if(((GPIOA->IDR) & 0x00000001) == 1) return;
	    }
		}
		//shift back in opposite direction
		//put spaces at the front:
		for(i = 0; i <= 5; i++){
			str_pad[i] = ' ';
			position = i;
		}
		//copy the contents of str into str_pad.
		for(i = 0; i <= length-1; i++){
			str_pad[i+position+1] = str[i];
		}
		for(i = 0; i <= 5; i++){
			LCD_DisplayString(str_pad+i);			
			
			for(j = 0; j < 100000; j++){
		    if(((GPIOA->IDR) & 0x00000001) == 1) return;
	    }
		}
		
		
	}
}




void start_screen(void){
	
	char *start_message = "PRESS BUTTON TO START THE GAME";
	movingString((uint8_t *)start_message, strlen(start_message));
	
}




