#include "stm32l476xx.h"
#include "LCD.h"

int main(void){
	
	
	char minutes_1_char[1];
	char minutes_2_char[1];
	char seconds_ints_1_char[1];
	char seconds_ints_2_char[1];
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
	
	int elapsed = 130589;
	
	LCD_Initialization();
	
	minutes_1 = ((elapsed - (elapsed % 60000))/60000)/10;
	minutes_2 = ((elapsed - (elapsed % 60000))/60000)%10;
	seconds_ints_1 = (((elapsed % 60000) - (elapsed % 1000))/1000)/10; //middle 2 characters
	seconds_ints_2 = (((elapsed % 60000) - (elapsed % 1000))/1000)%10;
	seconds_decs_1 = ((elapsed % 1000)/10)/10;                         //last 2 characters
	seconds_decs_2 = ((elapsed % 1000)/10)%10; 
	
	*minutes_1_char = minutes_1 + '0';
	*minutes_2_char = minutes_2 + '0';
	*seconds_ints_1_char = seconds_ints_1 + '0';
	*seconds_ints_2_char = seconds_ints_2 + '0';
	*seconds_decs_1_char = seconds_decs_1 + '0';
	*seconds_decs_2_char = seconds_decs_2 + '0';
	
	
	
	LCD_WriteChar(minutes_1_char, 0 , 0, 0); 
	LCD_WriteChar(minutes_2_char, 0 , 1, 1);  
	LCD_WriteChar(seconds_ints_1_char, 0 , 0, 2);  
	LCD_WriteChar(seconds_ints_2_char, 1 , 0, 3); 
	LCD_WriteChar(seconds_decs_1_char, 0 , 0, 4); 
	LCD_WriteChar(seconds_decs_2_char, 0 , 0, 5);
	
	
	
	
	
	
	
}
