#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp32/rom/uart.h"
#include "string.h"
#include "lcd.h"
#include "i2c-lcd1602.h"
#include "smbus.h"
#include "time.h"
#include "i2c-lcd1602.h"
#include "lcd.h"
#include "smbus.h"
#include "esp_sntp.h"

#define I2C_MASTER_NUM           I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN    0                    
#define I2C_MASTER_RX_BUF_LEN    0                     
#define I2C_MASTER_FREQ_HZ       100000
#define I2C_MASTER_SDA_IO        CONFIG_I2C_MASTER_SDA
#define I2C_MASTER_SCL_IO        CONFIG_I2C_MASTER_SCL
#define LCD_NUM_ROWS			 4
#define LCD_NUM_COLUMNS			 40
#define LCD_NUM_VIS_COLUMNS		 20

i2c_lcd1602_info_t *lcd_info;

char currentQuestion[20] = "0 + 0 = ?";
int currentAnswerA = 0,currentAnswerB = 0,currentAnswerC = 0;

//Initialize i2c master
static void i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE; 
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;  
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_LEN,
                       I2C_MASTER_TX_BUF_LEN, 0);
}

//Initialize LCD
void setup_lcd()
{
    i2c_master_init();
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t address = CONFIG_LCD1602_I2C_ADDRESS;

    smbus_info_t * smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

    lcd_info = i2c_lcd1602_malloc();
    i2c_lcd1602_init(lcd_info, smbus_info, true, LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VIS_COLUMNS);
    i2c_lcd1602_set_backlight(lcd_info, true);
}

//Write string at a certain position
void write_string_position(char text[], int x,int y) {
    i2c_lcd1602_home(lcd_info);
    i2c_lcd1602_move_cursor(lcd_info, x, y);
    i2c_lcd1602_write_string(lcd_info,text);
}

//Method to write question on LCD
void writeQuestion(char question[], int answerA, int answerB, int answerC){
     i2c_lcd1602_clear(lcd_info);

     //Change current question and answers
     strcpy(currentQuestion,question);
     currentAnswerA = answerA;
     currentAnswerB = answerB;
     currentAnswerC = answerC;

    //Create string to write on LCD
     char answers[15];
     sprintf(answers,"A: %d B: %d C: %d",currentAnswerA,currentAnswerB,currentAnswerC);

    //Write question and options on screen
     write_string_position(currentQuestion,4,1);
     write_string_position(answers,2,3);
}

//Method to clear the LCD
void clear_lcd(){
        i2c_lcd1602_clear(lcd_info); 
}

//Method to switch to question screen
void switch_to_question_screen(){
    i2c_lcd1602_clear(lcd_info); 
    writeQuestion(currentQuestion,currentAnswerA,currentAnswerB,currentAnswerC); 
}

//EXTENDED CODE STARTS HERE

//Method to set the full alphabet to 'alphabet' string
void set_alphabet_string(){
    //Check if it's not set already
    if(!(alphabet[0] == 'a')){
         strcpy(alphabet,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    }
}

//Method to show alphabet 1 by 1 on the LCD
void show_alphabet(){
    clear_lcd();
    int counter = 0;        //Counter to keep track of which letter is next
    set_alphabet_string();

    //Loop through the letters of the alphabet
    while(counter < 26){
        char print_string[25];   //String to print on lcd

        //Create and write string on LCD
        sprintf(print_string,"Letter %d: %c",counter+1,alphabet[counter]);
        write_string_position(print_string,3,1);

        vTaskDelay(1500 / portTICK_RATE_MS);    //Wait 1,5 seconds
        counter++;
    }
    clear_lcd();
    write_string_position("That's all!",4,1);
}

//Method to show the table of a given value
void show_table_of_value(int value){
    clear_lcd();
    int counter = 1;    //Counter to keep track of what table value is next

    //Loop for every table of the given value
    while(counter <= 10){
        char print_string[25];  //String to print on lcd

        //Create and write string on LCD
        sprintf(print_string,"%d x %d = %d",counter,value,(counter*value));
        write_string_position(print_string,4,1);

        vTaskDelay(1500 / portTICK_RATE_MS);    //Wait 1,5 seconds
        counter++;
    }
    clear_lcd();
    write_string_position("That's all!",4,1);
}

//Method to show the order of operations
void show_order_of_operations(){
    clear_lcd();

    //Show order of operations one by one on LCD
    write_string_position("1. Parentheses ( )",0,0);
    vTaskDelay(1500 / portTICK_RATE_MS);    //Wait 1,5 seconds

    write_string_position("2. Expoonents e^",0,1);
    vTaskDelay(1500 / portTICK_RATE_MS);    //Wait 1,5 seconds

    write_string_position("3. Multiplication x/",0,2);
    vTaskDelay(1500 / portTICK_RATE_MS);    //Wait 1,5 seconds
    
    write_string_position("4. Addition + -",0,3);    
    vTaskDelay(1500 / portTICK_RATE_MS);    //Wait 1,5 seconds

}

//Method to turn on timer with the given amount of hours, minutes and seconds
void set_timer(int hours, int minutes, int seconds){
    clear_lcd();
    int amount_of_seconds = (hours * 3600) + (minutes * 60) + seconds;  //Set the total amount of seconds for the timer

    //Set time left values
    hours_left = hours;
    minutes_left = minutes;
    seconds_left = seconds;

    int counter = 0;    //Counter to keep track of how many seconds have passed
    while(counter < amount_of_seconds){
        char print_string[25];  //String to print on lcd

        //Create and write string on LCD
        sprintf(print_string,"%d:%d:%d",hours_left,minutes_left,seconds_left);
        write_string_position(print_string,3,1);

        vTaskDelay(800 / portTICK_RATE_MS);    //Wait little less than 1 second to make up for lcd writing time

        update_time_for_timer();
        counter++;
    }
}

//Update time for timer
void update_time_for_timer(){
    seconds_left--;

    //Check seconds
    if(seconds_left <= 0){
        seconds_left = 59;
        minutes_left--;
    }

    //Check minutes
    if(minutes_left < 0){
        minutes_left = 59;
        hours_left--;
    }
}

//Method to start a time counter
void start_time_counter(){
    clear_lcd();

    hours = 0;
    minutes = 0;
    seconds = 0;
    time_counter_running = true;    

    while (time_counter_running)
    {
        char print_string[25];  //String to print on lcd

        update_time_counter_time();

        //Create and write string on LCD
        sprintf(print_string,"%d:%d:%d",hours,minutes,seconds);
        write_string_position(print_string,3,1);

        vTaskDelay(800 / portTICK_RATE_MS);    //Wait little less than 1 second to make up for lcd writing time
    }
    
}

//Method to update time for the time-counter
void update_time_counter_time(){
    seconds++;

    //Check seconds
    if(seconds >= 60){
        seconds = 0;
        minutes++;

        //Check minutes
        if(minutes >= 60){
            minutes = 0;
            hours++;
        }
    }
}

//Create welcome screen
void create_welcome_screen(){
    clear_lcd();

    write_string_position("==============",0,0);      //Decoration
    write_string_position("Welcome!",0,1);
    write_string_position("==============",0,3);      //Decoration

    vTaskDelay(3000 / portTICK_RATE_MS);    //Wait 3 seconds
}

//Create goodbye screen
void create_goodbye_screen(){
    clear_lcd();

    write_string_position("==============",0,0);      //Decoration
    write_string_position("Goodbye!",0,1);
    write_string_position("==============",0,3);      //Decoration

    vTaskDelay(3000 / portTICK_RATE_MS);    //Wait 3 seconds
}

//Method to print the names of all the planets in the solar system
void show_all_planets(){
    clear_lcd();

    int counter = 0;    //Counter to keep track of which planet is next

    //Loop to go through the list of planets
    while(counter < 9){
        char print_string[20];  //String to print on lcd

        //Create and write string on LCD
        sprintf(print_string,"%s",planet_list[counter].name);
        write_string_position(print_string,3,1);

        vTaskDelay(1000 / portTICK_RATE_MS);    //Wait 1 second

        clear_lcd();
        counter++;
    }
    write_string_position("That's all!",4,1);
}

//Method to write random numbers at random positions on the LCD
void write_random_numbers(){
    writing_random_numbers = true;

    //Infinite loop to keep writing random numbers
    while (writing_random_numbers)
    {
        int random_number = rand() % 10;    //Generate random number from 0-10
        int random_row = rand() % 15;       //Generate random row number from 0-15
        int random_column = rand() % 3;     //Generate random column number from 0-3

        char print_string[3];       //String to print

        //Create and write string on LCD
        sprintf(print_string,"%d",random_number);
        write_string_position(print_string,random_row,random_column);

        vTaskDelay(1000 / portTICK_RATE_MS);    //Wait 1 second
    }
    
}




