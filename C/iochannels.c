/*
 * iochannels.c
 * 
 * Copyright 2019  <pi@raspberrypi>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <string.h>
#include <math.h>
#include <malloc.h>

#include "iochannels.h"

// Controller interface

controller* controller_open(controller_type type, int usegpio)
{
	controller *c;
	io_module *m;
	io_device *d;
	int i, j, err = CONTROLLER_SUCCESS;

	c = malloc(sizeof(controller));

	c->type = type;
	c->iomodules = NULL;
	c->bits = NULL;
	c->pulses = NULL;
	switch (type)
	{
		case V1:
			c->modules = 10;
			c->mindex = 0;

			c->iochannels = 8; // Q outputs of 74373

			c->obits = 2; // Q outputs of 7474
			c->bindex = 0;

			c->opulses = 2; // 6th and 7th output pins of 74138
			c->pindex = 0;

			c->iomodules = malloc(c->modules*sizeof(io_module));
			for(j=0;j<c->modules;j++)
			{
				m = &(c->iomodules[j]);
				m->parent = c;
				m->module = j;
				m->moduletype = M_VOID;
				strcpy(m->name, "");
				m->index = 0;
				m->pins_used = 0;

				m->gpiodevices = malloc((c->iochannels)*sizeof(io_device));
				for(i=0;i<c->iochannels;i++)
				{
					d = &(m->gpiodevices[i]);
					d->parent = c;
					d->module = j;
					d->channel = i;
					strcpy(d->name, "");
					d->type = D_VOID;
					d->numstates = 0;
					d->base_pin = 0;
					d->pins_used = 0;
				}
			}

			c->bits = malloc(c->obits*sizeof(io_device));
			for(i=0;i<c->obits;i++)
			{
				d = &(c->bits[i]);
				d->parent = c;
				strcpy(d->name, "");
				d->type = D_VOID;
				d->numstates = 0;
				d->channel = i;
				d->base_pin = 0;
				d->pins_used = 0;
			}

			c->pulses = malloc(c->opulses*sizeof(io_device));
			for(i=0;i<c->opulses;i++)
			{
				d = &(c->pulses[i]);
				d->parent = c;
				strcpy(d->name, "");
				d->type = D_VOID;
				d->numstates = 0;
				d->channel = i;
				d->base_pin = 0;
				d->pins_used = 0;
			}

			if (usegpio)
				err = init_state();
				//printf("init_state(%d) = %d\n", c->odatabits, ret);

			if( (err=pthread_mutex_init(&(c->iomutex), NULL))!=0 )
			{
				//printf("iomutex init failed, %d\n", ret);
				err = CONTROLLER_MUTEX_FAILED;
				break;
			}

			break;
		default:
			err = CONTROLLER_UNKNOWN;
			break;
	}

	c->err = err;

	return(c);
}

void controller_close(controller *c)
{
	int i, j;
	io_module *m;
	io_device *d;

	for(i=0;i<c->modules;i++)
	{
		m = &(c->iomodules[i]);
		for(j=0;j<m->index;j++)
		{
			d = &(m->gpiodevices[j]);
			pthread_mutex_destroy(&(d->chmutex));
		}
		free(m->gpiodevices);
	}
	free(c->iomodules);

	for(j=0;j<c->bindex;j++)
	{
		d = &(c->bits[j]);
		pthread_mutex_destroy(&(d->chmutex));
	}
	free(c->bits);

	for(j=0;j<c->pindex;j++)
	{
		d = &(c->pulses[j]);
		pthread_mutex_destroy(&(d->chmutex));
	}
	free(c->pulses);

	pthread_mutex_destroy(&(c->iomutex));
	free(c);
}

// iomodule interface

unsigned char controller_get_value(controller *c, unsigned int module, int locked)
{
	io_module *m = &(c->iomodules[module]);
	io_device *d;
	int i,j;

	for(i=0,j=0,m->databits=0x00;i<c->iochannels;i++)
	{
		d = &(m->gpiodevices[i]);
		if (d->type==D_VOID)
			break;
		if (locked) pthread_mutex_lock(&(d->chmutex));
		switch (d->type)
		{
			case A_LEVEL:
			case S_LEVEL:
				m->databits = m->databits << d->pins_used;
				m->databits |= d->value;
				break;
			case A_BUTTON:
				m->databits = m->databits << 1;
				m->databits |= (d->value & 0x80) >> 7;
				break;
			case A_SWITCH:
			case S_SWITCH:
			case A_PULSE:
			default:
				m->databits = m->databits << 1;
				m->databits |= d->value;
				break;
		}
		if (locked) pthread_mutex_unlock(&(d->chmutex));
		j+=d->pins_used;
		//printf("channel %d, value %x, databits %x\n", i, a[i].value, c->odatabits);
	}
	m->databits = m->databits << (c->iochannels-j);

	return(m->databits);
}

void controller_set_value(controller *c, unsigned int module, unsigned char value)
{
	io_module *m = &(c->iomodules[module]);
	io_device *d;
	int i,j;
	unsigned char valuemask;

	m->databits = value;

	for(i=0,j=c->iochannels;i<c->iochannels;i++)
	{
		d = &(m->gpiodevices[i]);
		if (d->type==D_VOID)
			break;
		pthread_mutex_lock(&(d->chmutex));
		j -= d->pins_used;
		d->value = value;
		d->value = d->value >> j;
		valuemask = pow(2, d->pins_used) - 1;
		d->value &= valuemask;
		pthread_mutex_unlock(&(d->chmutex));
		//printf("channel %d, value %x\n", i, s[i].value);
	}
}

unsigned int module_add(controller *c, char *name, module_type mtype)
{
	io_module *m;
	int err;

	pthread_mutex_lock(&(c->iomutex));
	if (c->mindex<c->modules)
	{
		m = &(c->iomodules[c->mindex]);
		strcpy(m->name, name);
		m->moduletype = mtype;
		err = c->mindex;
		c->mindex++;
	}
	else
		err = CONTROLLER_FULL;
	pthread_mutex_unlock(&(c->iomutex));

	return(err);
}

// ıodevice interface

int iochannel_add(controller *c, unsigned int module, char *name, device_type type, int numstates)
{
	io_module *m = &(c->iomodules[module]);
	io_device *d = &(m->gpiodevices[m->index]);
	int pins_used, err;

	pthread_mutex_lock(&(c->iomutex));
	switch (type)
	{
		case A_LEVEL:
		case S_LEVEL:
			if (!numstates) return(CONTROLLER_INVALID_STATES);
			pins_used = ceil(log(numstates) / log(2)); // s = 2**n; n = ceil(ln(s)/ln(2))
			break;
		case A_SWITCH:
		case S_SWITCH:
		case A_BUTTON:
			pins_used = 1;
			break;
		case D_VOID:
		default:
			pins_used = 0;
			break;
	}

	if (type == D_VOID)
	{
		err = CONTROLLER_INVALID_DEVICE;
	}
	else if (m->pins_used + pins_used > c->iochannels)
	{
		err = CONTROLLER_MODULE_FULL;
	}
	else
	{
		if( (err=pthread_mutex_init(&(d->chmutex), NULL))!=0 )
		{
			//printf("chmutex init failed, %d\n", ret);
			err = CHANNEL_MUTEX_FAILED;
		}
		else
		{
			d->module = module;
			strcpy(d->name, name);
			d->type = type;
			d->numstates = numstates;
			d->base_pin = m->pins_used;
			d->pins_used = pins_used;
			d->channel = m->index;
			switch (type)
			{
				case A_LEVEL:
				case S_LEVEL:
					d->value = 0; // Default value
					break;
				case A_SWITCH:
				case S_SWITCH:
				case A_BUTTON:
				default:
					d->value = SWITCH_OFF; // Default value
					break;
			}

			m->pins_used+=pins_used;
			err = m->index;
			m->index++;
			//controller_get_ovalue(c, module); // updates c->odatabits;
		}
	}
	pthread_mutex_unlock(&(c->iomutex));

	return(err);
}

int obit_add(controller *c, unsigned int module, char *name)
{
	io_device *d = &(c->bits[c->bindex]);
	int err;

	pthread_mutex_lock(&(c->iomutex));
	if (c->bindex >= c->obits)
	{
		err = CONTROLLER_FULL;
	}
	else
	{
		if( (err=pthread_mutex_init(&(d->chmutex), NULL))!=0 )
		{
			//printf("chmutex init failed, %d\n", ret);
			err = CHANNEL_MUTEX_FAILED;
		}
		else
		{
			d->module = module;
			strcpy(d->name, name);
			d->type = A_BIT;
			d->numstates = 2;
			d->base_pin = 4 + c->bindex;
			d->pins_used = 1;
			d->channel = c->bindex;
			d->value = SWITCH_OFF; // Default value
			err = c->bindex;
			c->bindex++;
		}
	}
	pthread_mutex_unlock(&(c->iomutex));

	return(err);
}

int opulse_add(controller *c, unsigned int module, char *name)
{
	io_device *d = &(c->pulses[c->pindex]);
	int err;

	pthread_mutex_lock(&(c->iomutex));
	if (c->pindex >= c->opulses)
	{
		err = CONTROLLER_FULL;
	}
	else
	{
		if( (err=pthread_mutex_init(&(d->chmutex), NULL))!=0 )
		{
			//printf("chmutex init failed, %d\n", ret);
			err = CHANNEL_MUTEX_FAILED;
		}
		else
		{
			d->module = module;
			strcpy(d->name, name);
			d->type = A_PULSE;
			d->numstates = 2;
			d->base_pin = 6 + c->pindex;
			d->pins_used = 1;
			d->channel = c->pindex;
			err = c->pindex;
			c->pindex++;
		}
	}
	pthread_mutex_unlock(&(c->iomutex));

	return(err);
}

unsigned char iochannel_get_value(controller *c, unsigned int module, unsigned int channel)
{
	io_module *m;
	io_device *d;
	int err;

	if (!c->mindex) // no modules added
	{
		err = 0;
	}
	else
	{
		if (module >= c->mindex) // module outside bounds
		{
			err = 0;
		}
		else
		{
			m = &(c->iomodules[module]);
			if (!m->index) // no channels added
			{
				err = 0;
			}
			else 
			{
				if (channel >= m->index) // channel outside bounds
				{
					err = 0;
				}
				else
				{
					d = &(m->gpiodevices[channel]);
					pthread_mutex_lock(&(d->chmutex));
					err = d->value;
					pthread_mutex_unlock(&(d->chmutex));
				}
			}
		}
	}

	return(err);
}

int iochannel_set_value(controller *c, unsigned int module, unsigned int channel, unsigned char value)
{
	io_module *m;
	io_device *d;
	int err;
	unsigned char valuemask;

	if (!c->mindex) // no modules added
	{
		err = NO_MODULES_ADDED;
	}
	else
	{
		if (module >= c->mindex) // module outside bounds
		{
			err = MODULE_OUTSIDE_BOUNDS;
		}
		else
		{
			m = &(c->iomodules[module]);
			if (!m->index) // no channels added
			{
				err = NO_CHANNELS_ADDED;
			}
			else 
			{
				if (channel >= m->index) // channel outside bounds
				{
					err = CHANNEL_OUTSIDE_BOUNDS;
				}
				else
				{
					d = &(m->gpiodevices[channel]);
					switch (d->type)
					{
						case A_LEVEL:
						case S_LEVEL:
							valuemask = pow(2, d->pins_used) - 1;
							value &= valuemask;
							break;
						case A_BUTTON:
							valuemask = 0x7F;
							value &= valuemask;
							break;
						case A_SWITCH:
						case S_SWITCH:
						default:
							if (value)
								value = 0x01;
							else
								value = 0x00;
							break;
					}
					pthread_mutex_lock(&(d->chmutex));
					d->value = value;
					pthread_mutex_unlock(&(d->chmutex));
					err = CONTROLLER_SUCCESS;
				}
			}
		}
	}

	return(err);
}

void* iochannel_pulse_thread(void* args)
{
	io_device *d = (io_device *)args;
	unsigned int duration;

	pthread_detach(pthread_self());

	pthread_mutex_lock(&(d->chmutex));
	duration = d->value;
	d->value |= 0x80; // set highest bit
//printf("%s module %d channel %d value %d\n", d->name, d->module, d->channel, d->value);
	ochannel_write(d->parent, FALSE);
	usleep(duration*100000); // pulse

	d->value &= 0x7F; // reset highest bit
//printf("%s module %d channel %d value %d\n", d->name, d->module, d->channel, d->value);
	ochannel_write(d->parent, FALSE);
	usleep(500000); // avoid spike
	pthread_mutex_unlock(&(d->chmutex));

	pthread_exit(NULL);
}

int iochannel_pulse(controller *c, unsigned int module, unsigned int channel)
{
	io_module *m;
	io_device *d;
	int err;
	pthread_t tid;

	if (!c->mindex) // no modules added
	{
		err = NO_MODULES_ADDED;
	}
	else
	{
		if (module >= c->mindex) // module outside bounds
		{
			err = MODULE_OUTSIDE_BOUNDS;
		}
		else
		{
			m = &(c->iomodules[module]);
			if (!m->index) // no channels added
			{
				err = NO_CHANNELS_ADDED;
			}
			else 
			{
				if (channel >= m->index) // channel outside bounds
				{
					err = CHANNEL_OUTSIDE_BOUNDS;
				}
				else
				{
					d = &(m->gpiodevices[channel]);
					switch (d->type)
					{
						case A_BUTTON:
							err = pthread_create(&tid, NULL, &iochannel_pulse_thread, (void *)d);
							if (err)
							{}
							break;
						default:
							break;
					}
					err = CONTROLLER_SUCCESS;
				}
			}
		}
	}
	
	return(err);
}

void ochannel_write(controller *c, int locked)
{
	io_module *m;
	unsigned int i;

	pthread_mutex_lock(&(c->iomutex));
	for(i=c->mindex;i;)
	{
		i--;
		m = &(c->iomodules[i]);
		if (m->moduletype!=ACTUATOR) continue;
		write_data_s(controller_get_value(c, i, locked));
	}
	write_data_e();
	pthread_mutex_unlock(&(c->iomutex));
}

unsigned char obit_get_value(controller *c, unsigned int bit)
{
	io_device *d;
	int err;

	if (!c->bindex) // no bits added
	{
		err = 0;
	}
	else if (bit >= c->bindex) // bit outside bounds
	{
		err = 0;
	}
	else
	{
		d = &(c->bits[bit]);
		pthread_mutex_lock(&(d->chmutex));
		err = d->value;
		pthread_mutex_unlock(&(d->chmutex));
	}

	return(err);
}

void obit_set_value(controller *c, unsigned int bit, unsigned char value)
{
	io_device *d;

	if (!c->bindex) // no bits added
	{
	}
	else if (bit >= c->bindex) // bit outside bounds
	{
	}
	else
	{
		d = &(c->bits[bit]);
		pthread_mutex_lock(&(d->chmutex));
		if (value)
			c->bits[bit].value = 0x01;
		else
			c->bits[bit].value = 0x00;
		pthread_mutex_lock(&(c->iomutex));
		write_bit(c->bits[bit].base_pin, c->bits[bit].value);
		pthread_mutex_unlock(&(c->iomutex));
		pthread_mutex_unlock(&(d->chmutex));
	}
}

unsigned char opulse_get_value(controller *c, unsigned int pulse)
{
	io_device *d;
	int err;

	if (!c->pindex) // no pulses added
	{
		err = 0;
	}
	else if (pulse >= c->pindex) // pulse outside bounds
	{
		err = 0;
	}
	else
	{
		d = &(c->pulses[pulse]);
		pthread_mutex_lock(&(d->chmutex));
		err = d->value;
		pthread_mutex_unlock(&(d->chmutex));
	}

	return(err);
}

void opulse_set_value(controller *c, unsigned int pulse, unsigned char value)
{
	io_device *d;

	if (!c->pindex) // no pulses added
	{
	}
	else if (pulse >= c->pindex) // pulse outside bounds
	{
	}
	else
	{
		d = &(c->pulses[pulse]);
		pthread_mutex_lock(&(d->chmutex));
		d->value = value;
		pthread_mutex_unlock(&(d->chmutex));
	}
}

void opulse_out(controller *c, unsigned int pulse, unsigned int usecs)
{
	if (!c->pindex) // no pulses added
	{
	}
	else if (pulse >= c->pindex) // pulse outside bounds
	{
	}
	else
	{
		pthread_mutex_lock(&(c->iomutex));
		write_pulse(c->pulses[pulse].base_pin, usecs);
		pthread_mutex_unlock(&(c->iomutex));
	}
}

unsigned char ichannel_read(controller *c)
{
	io_module *m;
	unsigned char value;
	unsigned int i;

	pthread_mutex_lock(&(c->iomutex));
	read_data_b();
	for(i=0;i<c->mindex;i++)
	{
		m = &(c->iomodules[i]);
		if (m->moduletype!=SENSOR) continue;
		value = read_data_s();
		controller_set_value(c, i, value);
	}
	pthread_mutex_unlock(&(c->iomutex));

	return(value);
}

void printbinary(char* s, unsigned char v)
{
	char mask = 0x80;
	int i;

	for(i=0;i<8;i++)
	{
		s[i] = (v&mask?'1':'0');
		mask = mask>>1;
	}
	s[i] = '\0';
}

void dump_controller(controller *c)
{
	io_module *m;
	io_device *d;
	int i, j;
	char bin[9];

	for(i=0;i<c->mindex;i++)
	{
		m = &(c->iomodules[i]);
		printbinary(bin, controller_get_value(c, i, FALSE));
		printf("%d) %s, type:%d index:%d value:%s\n", i, m->name, m->moduletype, m->index, bin);
		for(j=0;j<m->index;j++)
		{
			d=&(m->gpiodevices[j]);
			printf("\t%d) %s, module %d, channel %d, value %d\n", j, d->name, d->module, d->channel, d->value);
		}
	}
}
