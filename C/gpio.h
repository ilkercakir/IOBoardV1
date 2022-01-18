#ifndef gpioH
#define gpioH

#include <wiringPi.h>
#include <unistd.h>

#define GPIO_SUCCESS 0
#define GPIO_ERROR -1

#define USECS 1000 // clock half pulse width

void set_address(unsigned int addr);
void clock_pulse(int duration);
void output_data_bit(unsigned char databit);
void write_data_s(unsigned char c);
void write_data_e();
void write_bit(unsigned char addr, unsigned char c);
void write_pulse(unsigned char addr, unsigned int usecs);
void read_data_b();
unsigned char read_data_s();
int init_state();

#endif
