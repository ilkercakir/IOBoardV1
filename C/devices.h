#ifndef devicesH
#define devicesH

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"

typedef struct
{
	int modul;
	int devid;
	char dtext[31];
	int chnnl;
	int dtype;
	char dttext[21];
	int numstates;
	int initval;
	int dstat;
	char categ;
	char catxt[21];
	char dicon[21];
	char dticon[21];
}device;

typedef struct
{
	int count;
	device *devices;
}iodevices;

typedef struct
{
	int modul;
	char modtx[31];
	char categ;
	int mtype;
	iodevices devs;
}module;

typedef struct
{
	int count;
	module *modules;
}iomodules;

//void get_devices(iodevices *d, char categ, int fromChannel, int toChannel);
//void free_devices(iodevices *d);
void get_modules(iomodules *m);
void free_modules(iomodules *m);
#endif
