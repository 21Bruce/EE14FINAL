#include "stm32l476xx.h"
#include "LCD.h"
#include "SysTick.h"
#include <stdio.h>
#include <stdlib.h>

int display_milliseconds(int time_to_display);
void display_direction(int index);
void display_score(int score);
void joystick_config(void);
void count_down(void);
void movingString(uint8_t* str, uint8_t len);
void start_screen(void);
int random_direction_gen(void);
int count_until_click(int index);

int main(void){
	int index = 0;
	int time_elapsed = 0;
	int dir_index = 0;
	int millis_elapsed = 0;
	int score_increment = 0;
	int score = 0;
	
	LCD_Initialization(); //initialize the LCD display
	SysTick_Initialize(1000);
	joystick_config();
	
	start_screen();
	while(1){
		count_down();
		dir_index = random_direction_gen();
		display_direction(dir_index);
		millis_elapsed = count_until_click(dir_index);
		score_increment = display_milliseconds(millis_elapsed);
		delay(1000);
		score = score + score_increment;
		display_score(score);
		delay(1000);
	}
}


//Displays a direction: up, down, right, left, or pushdown
//Each direction has a corresponding index input
//Input: index of the direction to display to the LCD
//Output: Direction string on the LCD
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


//Configure all joystick buttons. clock, GPIO mode, etc
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


//Scrolls the starting message accross the LCD. Stops when the central button is pressed.
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
		    if(((GPIOA->IDR) & 0x00000001) == 1) return;  //start game
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
		    if(((GPIOA->IDR) & 0x00000001) == 1) return;  //start game
	    }
		}
	}
}

//Display the game starting message. Game starts when the central button is pressed
void start_screen(void){
	char *start_message = "PRESS BUTTON TO START THE GAME";
	movingString((uint8_t *)start_message, strlen(start_message));
}


//Returns time in milliseconds between center press and given side press
//input: index of the side button we want to count for.
//output: Time (in milliseconds) elapsed
int count_until_click(int index){
	int count = 0; //counter
	int time_divider = 305;  //adjust the time by dividing
	int time_elapsed = 0;
	int button_status = 0;
	
	int missed_exit_code = 777; //if the player exceeds 500 ms, output 777.
															//The printing function will recognize this 
															//code and will output a "losing" message
	if(index == 0){
		button_status = ((GPIOA->IDR) & 0x00000008) >> 3; //up PA3
	}else if(index == 1){
		button_status = ((GPIOA->IDR) & 0x00000020) >> 5; //down PA5
	}else if(index == 2){
		button_status = ((GPIOA->IDR) & 0x00000004) >> 2; //right PA2
	}else if(index == 3){
		button_status = ((GPIOA->IDR) & 0x00000002) >> 1; //left PA1
	}
	
	while (button_status == 0){
		count = count + 1;
		
		//update status ints
		if(index == 0){
			button_status = ((GPIOA->IDR) & 0x00000008) >> 3; //up PA3
		}else if(index == 1){
			button_status = ((GPIOA->IDR) & 0x00000020) >> 5; //down PA5
		}else if(index == 2){
			button_status = ((GPIOA->IDR) & 0x00000004) >> 2; //right PA2
		}else if(index == 3){
			button_status = ((GPIOA->IDR) & 0x00000002) >> 1; //left PA1
		}
		
		time_elapsed = count/time_divider;
		
		//if the player exceeds 500 ms, output 777. The printing function will 
		//recognize this code and will output a "losing" message
		if(time_elapsed > 600){
			return missed_exit_code;
		}
	}
	return time_elapsed;
}


//Displays time inputted with a "ms" appended
//assumption that the input is 3 digits long (or less)
int display_milliseconds(int time_to_display){
	//variable declarations
	char units[1];
	char tens[1];
	char hundreds[1];  
	int units_int = 0;
	int tens_int = 0;
	int hundreds_int = 0;
	char m[1];
	char s[1];
	char space[1];
	
	//if the player exceeds 500 ms, output a "losing" message
	int missed_exit_code = 777;
	char *missed_message = "MISSED";
	if(time_to_display == missed_exit_code){
		LCD_DisplayString(missed_message);
		return 0;
	}
	
	*m = 'm';  
	*s = 's';
	*space = ' ';
	
	//get each digit individually
	hundreds_int = time_to_display/100;
	tens_int = (time_to_display/10)%10;
	units_int = time_to_display % 10;
	//convert to character
	*hundreds = hundreds_int + '0';
	*tens = tens_int + '0';
	*units = units_int + '0';
	//print to LCD
	LCD_WriteChar(hundreds, 0 , 0, 0); 
	LCD_WriteChar(tens, 0 , 0, 1);  
	LCD_WriteChar(units, 0 , 0, 2);
	LCD_WriteChar(space, 0 , 0, 3);
	LCD_WriteChar(m, 0 , 0, 4);  
	LCD_WriteChar(s, 0 , 0, 5);
	return 1;
}


//Display inputted number with "pts" appended
//assumption that the score is less than 99
void display_score(int score){
	//variable declarations
	char units[1];
	char tens[1];  
	int units_int = 0;
	int tens_int = 0;

	char p[1];
	char t[1];
	char s[1];
	char space[1];
	
	tens_int = score/10;
	units_int = score%10;
	*tens = tens_int + '0';
	*units = units_int + '0';
	*p = 'p';  
	*t = 't';
	*s = 's';
	*space = ' ';
	
	LCD_WriteChar(tens, 0 , 0, 0);  
	LCD_WriteChar(units, 0 , 0, 1);
	LCD_WriteChar(space, 0 , 0, 2);
	LCD_WriteChar(p, 0 , 0, 3);  
	LCD_WriteChar(t, 0 , 0, 4);
	LCD_WriteChar(s, 0 , 0, 5);
}


//Starting countdown that is displayed before the direction is displayed
void count_down(void){
	int i;
	char num[4];
	num[0] = ' ';
	num[1] = ' ';
	num[2] = ' ';
	for(i = 3; i > 0; i--){
		num[3] = '0' + i;
		LCD_DisplayString(num);
		delay(500);
	}
}


//Randomly generated a direction index
//PROBLEM: IT'S PSEUDORANDOM. EACH TIME RESTART GAME, SAME SUCCESSION OF DIRECTIONS.
int random_direction_gen(void){
	int rand_num = rand(); // gen random number
	rand_num %= 10; // take the ones place, this gives us a random number in the range [0, 10)
	rand_num /= 2.5; // divide by 2.5, this shortens the range to [0,4).
	return rand_num; // return an int, this floors the range, giving use the desired [0,3]
}
