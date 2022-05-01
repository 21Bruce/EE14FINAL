#include "stm32l476xx.h"
#include "LCD.h"
#include "SysTick.h"
#include <stdio.h>
#include <stdlib.h>


int display_milliseconds(int time_to_display);
void joystick_config(void);
void count_down(void);
int movingString(uint8_t* str, uint8_t len);
int start_screen(void);

void missed_display(void);
int count_until_click(void);

void display_operation(int digit1, int digit2, int op_index);
extern int keyboard(void);   //ASSEMBLY FILE keyboard.s

int do_calculation(int a, int b, int op_index);
int get_input_keyboard(int expected_digits);
int digit_input();

int right_answer(int user_input, int correct_result, int time_waited, int score);
void display_good_message(void);
void display_wrong_message(void);
int amount_digits(int a);
void display_player_score(int score, int player_number);

void display_player1_win(void);
void display_player2_win(void);
void display_tie(void); //tie game
void display_endgame(void);
void display_player_1_go(void);
void display_player_2_go(void);
void display_pause(void);
void display_choose_level(void);
void RNG_Initialization(void);
int RNG_Gen(void);

void winner_announcer(int score1, int score2);

int get_level(void);

int main(void){
	int input = 0;
	int test = 0;
	int time_waited = 0;
	int oper1 = 9;
	int oper2 = 4;
	int op_test = 2;
	int amount_dig = 0;
	int score_player1 = 0;
	int score_player2 = 0;
	char score_player1_char[1];
	int player_number = 0;
	int seed;
	LCD_Initialization(); //initialize the LCD display
	SysTick_Initialize(1000);
	
	//add the scrolling press to start game.
	joystick_config();
	seed = start_screen();
	srand((unsigned) seed);
	char num[1];
	while (1){
		*num = '0' + (rand() % 10);
		LCD_DisplayString(num);
		delay(1000);
	}
	display_choose_level();
	op_test = get_level();
	//FIRST PLAYER'S TURN
	int i = 0;
	player_number = 1;
	display_player_1_go();
	for(i=0; i<2; i++){  //2 turns
		count_down();
		test = do_calculation(oper1, oper2, op_test);
		display_operation(oper1, oper2, op_test);
		amount_dig = amount_digits(test);
		
		time_waited = count_until_click(); 
		if(time_waited == 777){
			missed_display();
			delay(500);
			continue; //when add the loop
		}

		input = get_input_keyboard(amount_dig);
		score_player1 = score_player1 + right_answer(input, test, time_waited, score_player1); 
		delay(500); 
  }
	display_player_score(score_player1, player_number);
	delay(1000);
	
	display_pause();
	//SECOND PLAYER'S TURN
	player_number = 2;
	display_player_2_go();
	for(i=0; i<2; i++){  //2 turns
		count_down();
		test = do_calculation(oper1, oper2, op_test);
		display_operation(oper1, oper2, op_test);
		amount_dig = amount_digits(test);
		
		time_waited = count_until_click(); 
		if(time_waited == 777){
			missed_display();
			delay(500);
			continue; //when add the loop
		}

		input = get_input_keyboard(amount_dig);
		score_player2 = score_player2 + right_answer(input, test, time_waited, score_player2); 
		delay(500);
  }
	display_player_score(score_player2, player_number);
	delay(1000);
	
	display_pause();
	//display both scores, then announce winner
	
	winner_announcer(score_player1, score_player2);
	delay(1000);
	//display_tie();
	//delay(1000);
	
	
	display_endgame();
	

}

void RNG_Initialization(void) {

	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
	RNG->CR |= RNG_CR_RNGEN;
}

int RNG_Gen(void) {
	while (!(RNG->SR & (RNG_SR_DRDY)));
	return (RNG->DR) % 10;
}


//Uses side buttons A, B, and C to determine which operators will be used
//outputs operator code.
int get_level(void){
	int output_operation = 0; //the level will determine the operations used
	int user_input = 0;
	
	user_input = digit_input();
	
	if(user_input == 0x400){ // the A button is pressed
		output_operation = 1; //addition
	}
	if(user_input == 0x700){ // the B button is pressed
		output_operation = 2; //subtraction
	}
	if(user_input == 0x43300){ // the C button is pressed
		output_operation = 3; //multiplication
	}
	
	return output_operation;

}



//Displays the operation the player needs to execute.
//Inputs: the two 2-digit operands, and the code for the operator
//Output: LCD dis
void display_operation(int digit1, int digit2, op_index){
	char digit1char1st[1]; //1st digit of the 1st 2-digit input number
	char digit1char2nd[1];
	char digit2char1st[1];
	char digit2char2nd[1];
	int digit11st = 0;
	int digit12nd = 0;
	int digit21st = 0;
	int digit22nd = 0;
	
	digit11st = digit1/10; //get 1st digit
	digit12nd = digit1%10; //get 2nd digit
	
	digit21st = digit2/10; //get 1st digit
	digit22nd = digit2%10; //get 2nd digit
	
	*digit1char1st = digit11st + '0'; //convert to char
	*digit1char2nd = digit12nd + '0';
	
	*digit2char1st = digit21st + '0';
	*digit2char2nd = digit22nd + '0';
	
	//screen aesthetic adjustments
	if(digit11st == 0){
		*digit1char1st = ' ';
	}
	if(digit21st == 0){
		*digit2char1st = digit22nd + '0';
		*digit2char2nd = ' ';
	}
	
	char plus[1];
	char minus[1];
	char mult[1];
	char equal[1];
	
	*plus = '+';
	*minus = '-';
	*mult = 'x';
	*equal = '=';
	//if statement checking if 2 digits using mod, then accomodate
	LCD_WriteChar(digit1char1st, 0 , 0, 0);
	LCD_WriteChar(digit1char2nd, 0 , 0, 1);
	LCD_WriteChar(digit2char1st, 0 , 0, 3);
	LCD_WriteChar(digit2char2nd, 0 , 0, 4);
	LCD_WriteChar(equal, 0 , 0, 5);
	
	//do the correct operation according to the operation code inputted
	if(op_index == 1){
		LCD_WriteChar(plus, 0 , 0, 2);
	}else if(op_index == 2){
		LCD_WriteChar(minus, 0 , 0, 2);
	}else if(op_index == 3){
		LCD_WriteChar(mult, 0 , 0, 2);
	}
}


//do the correct calculation. This will later be compared to what 
//the user inputs 
int do_calculation(int a, int b, int op_index){
	
	if(op_index == 1){
		return (a+b);
	}else if(op_index == 2){
		return (a-b);
	}else if(op_index == 3){
		return (a*b);
	}
	
}


//Get the input from the user through the keypad
//The keypad function is wrote in ARM assembly in keyboard.s
int get_input_keyboard(int expected_digits){
	int output = 0;
	int a = 0;
	int power = 1;
	
	int array[expected_digits];
	int i = 0;
	//get inputs and putc in array
	for(i = 0; i<expected_digits; i++){
		array[i] = digit_input(); //put the pressed value in array
	}
	
	for(i = expected_digits-1; i>=0; i--){
		output = output + array[i]*power;
		power = power * 10; //multiply with a power of 10 to add them
	}
	return output;
}


int digit_input(){
	int output = 0;  
	//initialize the keypad registers
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	GPIOA->MODER &= ~(0xCFC);
	GPIOE->MODER &= ~(0xFF<<20);
	GPIOE->MODER |= (0x55<<20);
	GPIOE->ODR &= ~(0xF<<10);
	GPIOA->PUPDR &= ~(0xFFF);
	GPIOA->PUPDR |= (0x454);
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER &= ~(0x3<<4);
	GPIOE->MODER |= (0x1<<4);
	GPIOE->ODR &= ~(0x3<<16);
	GPIOE->ODR &= ~(0x1<<16);
	output = keyboard();
	return output;
}


//Purposes: - determine if the user input is correct
//		   		- update the score if the result is correct
//					- display messages about the correctness
int right_answer(int user_input, int correct_result, int time_waited, int score){
	int a = 0;
	int score_update = 0; //score increment
	if(user_input == correct_result){
		display_good_message();
		delay(500);
		display_milliseconds(time_waited);
		return 1; //add 1 to score
	}
	if(user_input != correct_result){
		display_wrong_message();
		return 0;
	}
}

void display_good_message(){
	char *good = " GOOD";
	LCD_DisplayString(good);
	delay(300);
}

void display_wrong_message(){
	char *wrong = " WRONG";
	LCD_DisplayString(wrong);
	delay(300);
}

void missed_display(void){
	char *missed_message = "MISSED";
	LCD_DisplayString(missed_message);
}

void display_player1_win(){
	char *message = "P1 WIN";
	LCD_DisplayString(message);
}

void display_player2_win(){
	char *message = "P2 WIN";
	LCD_DisplayString(message);
}

void display_tie(){
	char *message = "  TIE ";
	LCD_DisplayString(message);
}

void display_endgame(){
	char *message = "++++++";
	LCD_DisplayString(message);
}


void display_player_1_go(void){
	char *message = "P1 go!";
	LCD_DisplayString(message);
	delay(700);
}


void display_player_2_go(void){
	char *message = "P2 go!";
	LCD_DisplayString(message);
	delay(700);
}

void display_pause(void){
	char *message = "------";
	LCD_DisplayString(message);
	delay(1500);
}

void display_choose_level(void){
	char *message = " LEVEL";
	LCD_DisplayString(message);
}




//Output the number of digits expeted from the user
//to form the answer to the operation.
int amount_digits(int a){
	if((a/10) == 0){
		return 1;
	}
	if((a/100) == 0){
		return 2;
	}
	if((a/1000) == 0){
		return 3;
	}
	if((a/10000) == 0){
		return 4;
	}
	if((a/100000) == 0){
		return 5;
	}
	if((a/1000000) == 0){
		return 6;
	}
}


//Count the time between the display of the operation to carry out, 
//and the moment the user inputs their answer
int count_until_click(void){
	int missed_exit_code = 777; //if the player exceeds 500 ms, output 777.
	int a = 0;
	int counter = 0;
	int time_elapsed = 0;
	int time_divider = 349; //dividing constant to go from counter to time elapsed
	
	//initialize the keypad stuff
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	GPIOA->MODER &= ~(0xCFC);
	GPIOE->MODER &= ~(0xFF<<20);
	GPIOE->MODER |= (0x55<<20);
	GPIOE->ODR &= ~(0xF<<10);
	GPIOA->PUPDR &= ~(0xFFF);
	GPIOA->PUPDR |= (0x454);
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER &= ~(0x3<<4);
	GPIOE->MODER |= (0x1<<4);
	GPIOE->ODR &= ~(0x3<<16);
	GPIOE->ODR &= ~(0x1<<16);
	
	//set the row outputs to 0
	GPIOE->ODR &= ~(0xF<<10);
	
	a = (GPIOA->IDR)&(0x2E);
	//check if a button is clicked
	while(a == 0x2E){
		counter = counter + 1;
		a = (GPIOA->IDR)&(0x2E);
		if((counter/time_divider) >= 9999){
			return missed_exit_code;
		}
	}
	
	//calculation to go from counter to time elapsed.
	time_elapsed = counter/time_divider;
	
	return time_elapsed;
}


int display_milliseconds(int time_to_display){
	//variable declarations
	char units[1];
	char tens[1];
	char hundreds[1]; 
  char thousands[1];	
	int units_int = 0;
	int tens_int = 0;
	int hundreds_int = 0;
	int thousands_int = 0;
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
	thousands_int = (time_to_display/1000);
	hundreds_int = (time_to_display/100)%10;
	tens_int = (time_to_display/10)%10;
	units_int = time_to_display % 10;
	//convert to character
	*thousands = thousands_int + '0';
	*hundreds = hundreds_int + '0';
	*tens = tens_int + '0';
	*units = units_int + '0';
	//print to LCD
	LCD_WriteChar(space, 0 , 0, 0);
	LCD_WriteChar(thousands, 1 , 0, 1); 
	LCD_WriteChar(hundreds, 0 , 0, 2); 
	LCD_WriteChar(tens, 0 , 0, 3);
	LCD_WriteChar(space, 0 , 0, 4);
	LCD_WriteChar(s, 0 , 0, 5);
	
	return 1;
}



//Display the score of a player
void display_player_score(int score, int player_number){
	char score_char[1];
	char p[1];
	char t[1];
	char player_number_char[1];
	char space[1];
	*score_char = score + '0';
	*p = 'p';
	*t = 't';
	*space = ' ';
	*player_number_char = player_number + '0';
	
	LCD_WriteChar(p, 0 , 0, 0);
	LCD_WriteChar(player_number_char, 0 , 0, 1); 
	LCD_WriteChar(space, 0 , 0, 2); 
	LCD_WriteChar(score_char, 0 , 0, 3);
	LCD_WriteChar(p, 0 , 0, 4);
	LCD_WriteChar(t, 0 , 0, 5);
}



//displays both players' scores then the winner of the game.
void winner_announcer(int score1, int score2){
	display_player_score(score1, 1);
	delay(700);
	display_player_score(score2, 2);
	delay(700);
	if(score1 > score2){
		display_player1_win(); //player 1 wins
	}else if(score1 < score2){
		display_player2_win(); //player 2 wins
	}else{
		display_tie(); //tie 
	}
	delay(5000);
}



//Starting countdown that is displayed before the direction is displayed
void count_down(void){
	int i;
	
	
	
	char one[1];
	char two[1];
	char three[1];
	char space[1];
	*one = '1';
	*two = '2';
	*three = '3';
	*space = ' ';
	
	LCD_WriteChar(space, 0 , 0, 0);
	LCD_WriteChar(space, 0 , 0, 1);
	LCD_WriteChar(space, 0 , 0, 2);
	LCD_WriteChar(space, 0 , 0, 3);
	LCD_WriteChar(space, 0 , 0, 4);
	LCD_WriteChar(space, 0 , 0, 5);
	
	LCD_WriteChar(three, 0 , 0, 3);
	delay(300);
	LCD_WriteChar(two, 0 , 0, 3);
	delay(300);
	LCD_WriteChar(one, 0 , 0, 3);
	delay(300);
}



//Configure all joystick buttons. clock, GPIO mode, etc
void joystick_config(void){
	//Enable clock for GPIO port A
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	//Add pulldown resistors to the lateral buttons
  //	GPIOA->PUPDR &= ~(0x00000CFF);
  //	GPIOA->PUPDR |= (0x000008AA);
	
	//Configure the GPIO port A as input mode 00. port PA0
	GPIOA->MODER &= ~(0x00000003);
	//PA1 mode vf    
	GPIOA->MODER &= ~(0x0000000C);
	//PA2 mode
	GPIOA->MODER &= ~(0x00000030);
	//PA3 mode
	GPIOA->MODER &= ~(0x000000C0);
	//PA5 mode
	GPIOA->MODER &= ~(0x00000C00);
}

//Display the game starting message. Game starts when the central button is pressed
int start_screen(void){
	char *start_message = "PRESS BUTTON TO START THE GAME";
	return movingString((uint8_t *)start_message, strlen(start_message));
}


//Scrolls the starting message accross the LCD. Stops when the central button is pressed.
int movingString(uint8_t* str, uint8_t len){
	int a = 0;
	int position = 0;
	int j = 0;
	
	int count = 0;
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
			count++;
			str_pad[i] = str[i];
		}
		//add the spaces
		i = len;
		while(i != length){
			count++;
			str_pad[i] = ' ';
			i = i + 1;
		}
		//shift the ensemble of elements display to the right to give the illusion of scrolling to the left. 
		for(i = 0; i <= length; i++){
			LCD_DisplayString(str_pad+i);		
			for(j = 0; j < 100000; j++){
				count++;
		    if(((GPIOA->IDR) & 0x00000001) == 1) return count;  //start game
	    }
		}
		//shift back in opposite direction
		//put spaces at the front:
		for(i = 0; i <= 5; i++){
			str_pad[i] = ' ';
			position = i;
			count++;
		}
		//copy the contents of str into str_pad.
		for(i = 0; i <= length-1; i++){
			str_pad[i+position+1] = str[i];
			count++;
		}
		for(i = 0; i <= 5; i++){
			LCD_DisplayString(str_pad+i);			
			count++;
			for(j = 0; j < 100000; j++){
		    if(((GPIOA->IDR) & 0x00000001) == 1) return count;  //start game
	    }
		}
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



//discussion issue: the player needs to push one button at a time.

