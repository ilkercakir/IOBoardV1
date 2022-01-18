#ifndef iochannelsH
#define iochannelsH

#include <pthread.h>
#include "gpio.h"

#define CONTROLLER_SUCCESS 0
#define CONTROLLER_UNKNOWN -2
#define CONTROLLER_FULL -3
#define CONTROLLER_INVALID_DEVICE -4
#define CONTROLLER_INVALID_STATES -5
#define CONTROLLER_MUTEX_FAILED -6
#define CONTROLLER_MODULE_FULL -7
#define NO_MODULES_ADDED -8
#define MODULE_OUTSIDE_BOUNDS -9
#define NO_CHANNELS_ADDED -10
#define CHANNEL_OUTSIDE_BOUNDS -11
#define CHANNEL_MUTEX_FAILED -12

typedef enum { V0, V1 } controller_type;

typedef enum { D_VOID, A_SWITCH, A_LEVEL, A_PULSE, A_BUTTON, S_SWITCH, S_LEVEL, A_BIT} device_type; // Must be inline with database table IOBoard.devicetypes

typedef enum { SWITCH_OFF, SWITCH_ON } switch_values;
typedef enum { PULSE_LOW, PULSE_HIGH } pulse_values;

typedef enum { M_VOID, ACTUATOR, SENSOR, CONTROL } module_type; // Must be inline with database table IOBoard.devcat

typedef struct modulecontroller controller; // forward

typedef struct
{
	controller *parent;
	unsigned int module;
	char name[50];
	device_type type;
	unsigned int numstates;
	unsigned int base_pin;
	unsigned int pins_used;
	unsigned int channel;
	unsigned char value;
	pthread_mutex_t chmutex;
}io_device;

typedef struct
{
	controller *parent;
	unsigned int module;
	module_type moduletype;
	char name[50];
	unsigned int index;
	unsigned int pins_used;
	unsigned char databits;
	io_device *gpiodevices;
}io_module;

struct modulecontroller
{
	int err;
	void *parent;
	controller_type type;
	unsigned int mindex;
	unsigned int modules; // max modules
	io_module *iomodules;
	unsigned int iochannels; // max channels
	unsigned int obits; // max output bits
	unsigned int opulses; // max output pulses
	unsigned int bindex;
	unsigned int pindex;
	io_device *bits;
	io_device *pulses;
	pthread_mutex_t iomutex;
};

controller* controller_open(controller_type type, int usegpio);
void controller_close(controller *c);
unsigned char controller_get_value(controller *c, unsigned int module, int locked);
void controller_set_value(controller *c, unsigned int module, unsigned char value);
unsigned int module_add(controller *c, char *name, module_type mtype);
int iochannel_add(controller *c, unsigned int module, char *name, device_type type, int numstates);
int obit_add(controller *c, unsigned int module, char *name);
int opulse_add(controller *c, unsigned int module, char *name);
unsigned char iochannel_get_value(controller *c, unsigned int module, unsigned int channel);
int iochannel_set_value(controller *c, unsigned int module, unsigned int channel, unsigned char value);
int iochannel_pulse(controller *c, unsigned int module, unsigned int channel);
void ochannel_write(controller *c, int locked);
unsigned char obit_get_value(controller *c, unsigned int bit);
void obit_set_value(controller *c, unsigned int bit, unsigned char value);
unsigned char opulse_get_value(controller *c, unsigned int pulse);
void opulse_set_value(controller *c, unsigned int pulse, unsigned char value);
void opulse_out(controller *c, unsigned int pulse, unsigned int usecs);
unsigned char ichannel_read(controller *c);
void dump_controller(controller *c);
#endif
