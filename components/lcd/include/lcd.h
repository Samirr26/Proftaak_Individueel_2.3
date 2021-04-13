#include "i2c-lcd1602.h"
#include "smbus.h"

#ifndef LCD_H
#define LCD_H
void setup_lcd();
void write_string_position(char text[], int x,int y);
void switch_to_question_screen();
void clearLCD();

//Extended code starts here
void show_alphabet();
void show_table_of_value(int value);
void show_order_of_operations();
void set_timer(int hours, int minutes, int seconds);
void update_time_for_timer();
void start_time_counter();
void update_time_counter_time();
void create_welcome_screen();
void create_goodbye_screen();
void show_all_planets();
void write_random_numbers();

char alphabet[27];
int hours_left;
int minutes_left;
int seconds_left;
bool time_counter_running;
bool writing_random_numbers;
int seconds;
int minutes;
int hours;

typedef struct PLANET{
    char name[20];
}PLANET;

//List of all planets
static PLANET planet_list[9] = {
    {"Mercury"},
    {"Venus"},
    {"Earth"},
    {"Mars"},
    {"Jupiter"},
    {"Saturn"},
    {"Uranus"},
    {"Neptune"},
    {"Pluto"}
};
#endif