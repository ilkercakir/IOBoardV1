#include "devices.h"

/*
int get_io_devices_callback(void *data, int argc, char **argv, char **azColName) 
{
	iodevices *d = (iodevices *)data;

	d->devices[d->count].devid = atoi(argv[0]);
	strcpy(d->devices[d->count].dtext, argv[1]);
	d->devices[d->count].chnnl= atoi(argv[2]);
	d->devices[d->count].dtype = atoi(argv[3]);
	strcpy(d->devices[d->count].dttext, argv[4]);
	d->devices[d->count].numstates = atoi(argv[5]);
	d->devices[d->count].initval = atoi(argv[6]);
	d->devices[d->count].dstat = atoi(argv[7]);
	d->devices[d->count].categ = argv[8][0];
	strcpy(d->devices[d->count].catxt, argv[9]);
	strcpy(d->devices[d->count].dicon, argv[10]);
	strcpy(d->devices[d->count].dticon, argv[11]); 
//printf("get_io_devices_callback %d\n", d->devices[d->count].devid);
	d->count++;

	return(0);
}

void get_io_devices(iodevices *d, char categ, int fromChannel, int toChannel)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[500];
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sprintf(sql, "select devices.devid, devices.dtext, devices.chnnl, devices.dtype, devicetypes.dtext as dttext, devices.numstates, devices.initval, devicetypes.dstat, devicetypes.categ, devcat.catxt, devices.dicon, devicetypes.dicon as dticon from devices inner join devicetypes on devicetypes.dtype = devices.dtype inner join devcat on devcat.categ = devicetypes.categ where devicetypes.categ = '%c' and devices.chnnl >= %d and devices.chnnl <= %d;", categ, fromChannel, toChannel);
		if((rc = sqlite3_exec(db, sql, get_io_devices_callback, (void*)d, &err_msg)) != SQLITE_OK)
		{
		printf("Failed to select data, %s\n", err_msg);
		sqlite3_free(err_msg);
		}
		else // success
		{
		}
	}
	sqlite3_close(db);
}

int get_count_iodevices_callback(void *data, int argc, char **argv, char **azColName) 
{
	iodevices *d = (iodevices *)data;
  
	d->count = atoi(argv[0]);
	d->devices = malloc(d->count*sizeof(device));
//printf("get_count_iodevices_callback %d\n", d->count);
	d->count = 0;

	return(0);
}

void get_count_iodevices(iodevices *d, char categ, int fromChannel, int toChannel)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[300];
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sprintf(sql, "select count(*) as devicecount from devices inner join devicetypes on devicetypes.dtype = devices.dtype inner join devcat on devcat.categ = devicetypes.categ where devicetypes.categ = '%c' and devices.chnnl >= %d and devices.chnnl <= %d;", categ, fromChannel, toChannel);
		if((rc = sqlite3_exec(db, sql, get_count_iodevices_callback, (void*)d, &err_msg)) != SQLITE_OK)
		{
		printf("Failed to select data, %s\n", err_msg);
		sqlite3_free(err_msg);
		}
		else // success
		{
		}
	}
	sqlite3_close(db);
}

void get_devices(iodevices *d, char categ, int fromChannel, int toChannel)
{
	get_count_iodevices(d, categ, fromChannel, toChannel);
	get_io_devices(d, categ, fromChannel, toChannel);
}

void free_devices(iodevices *d)
{
	free(d->devices);
}
*/

int get_io_devices_of_module_callback(void *data, int argc, char **argv, char **azColName) 
{
	iodevices *d = (iodevices *)data;

	d->devices[d->count].modul = atoi(argv[0]);
	d->devices[d->count].devid = atoi(argv[1]);
	strcpy(d->devices[d->count].dtext, argv[2]);
	d->devices[d->count].chnnl= atoi(argv[3]);
	d->devices[d->count].dtype = atoi(argv[4]);
	strcpy(d->devices[d->count].dttext, argv[5]);
	d->devices[d->count].numstates = atoi(argv[6]);
	d->devices[d->count].initval = atoi(argv[7]);
	d->devices[d->count].dstat = atoi(argv[8]);
	d->devices[d->count].categ = argv[9][0];
	strcpy(d->devices[d->count].catxt, argv[10]);
	strcpy(d->devices[d->count].dicon, argv[11]);
	strcpy(d->devices[d->count].dticon, argv[12]); 
//printf("get_io_devices_callback %d\n", d->devices[d->count].devid);
	d->count++;

	return(0);
}

void get_io_devices_of_module(iomodules *m, unsigned int modul)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[500];
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sprintf(sql, "select devices.modul, devices.devid, devices.dtext, devices.chnnl, devices.dtype, devicetypes.dtext as dttext, devices.numstates, devices.initval, devicetypes.dstat, devicetypes.categ, devcat.catxt, devices.dicon, devicetypes.dicon as dticon from devices inner join devicetypes on devicetypes.dtype = devices.dtype inner join devcat on devcat.categ = devicetypes.categ where devices.modul = %d;", modul);
		if((rc = sqlite3_exec(db, sql, get_io_devices_of_module_callback, (void*)&(m->modules[modul].devs), &err_msg)) != SQLITE_OK)
		{
		printf("Failed to select data, %s\n", err_msg);
		sqlite3_free(err_msg);
		}
		else // success
		{
		}
	}
	sqlite3_close(db);
}

int get_count_iodevices_of_module_callback(void *data, int argc, char **argv, char **azColName) 
{
	iodevices *d = (iodevices *)data;
  
	d->count = atoi(argv[0]);
	d->devices = malloc(d->count*sizeof(device));
//printf("get_count_iodevices_callback %d\n", d->count);
	d->count = 0;

	return(0);
}

void get_count_iodevices_of_module(iomodules *m, unsigned int modul)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[300];
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sprintf(sql, "select count(*) as devicecount from devices inner join devicetypes on devicetypes.dtype = devices.dtype inner join devcat on devcat.categ = devicetypes.categ where devices.modul = %d;", modul);
		if((rc = sqlite3_exec(db, sql, get_count_iodevices_of_module_callback, (void*)&(m->modules[modul].devs), &err_msg)) != SQLITE_OK)
		{
		printf("Failed to select data, %s\n", err_msg);
		sqlite3_free(err_msg);
		}
		else // success
		{
		}
	}
	sqlite3_close(db);
}

int get_iomodules_callback(void *data, int argc, char **argv, char **azColName) 
{
	iomodules *m = (iomodules *)data;

	m->modules[m->count].modul = atoi(argv[0]);
	strcpy(m->modules[m->count].modtx, argv[1]);
	m->modules[m->count].categ = atoi(argv[2]);
	m->modules[m->count].mtype = atoi(argv[3]);

	get_count_iodevices_of_module(m, m->count);
	get_io_devices_of_module(m, m->count);
//printf("get_modules_callback %d\n", m->modules[m->count].modul);
	m->count++;

	return(0);
}

void get_iomodules(iomodules *m)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[500];
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sprintf(sql, "select modules.modul, modules.modtx, modules.categ, devcat.mtype from modules inner join devcat on devcat.categ = modules.categ;");
		if((rc = sqlite3_exec(db, sql, get_iomodules_callback, (void*)m, &err_msg)) != SQLITE_OK)
		{
		printf("Failed to select data, %s\n", err_msg);
		sqlite3_free(err_msg);
		}
		else // success
		{
		}
	}
	sqlite3_close(db);
}

int get_count_iomodules_callback(void *data, int argc, char **argv, char **azColName) 
{
	iomodules *m = (iomodules *)data;
  
	m->count = atoi(argv[0]);
	m->modules = malloc(m->count*sizeof(module));
//printf("get_count_iomodules_callback %d\n", d->count);
	m->count = 0;

	return(0);
}

void get_count_iomodules(iomodules *m)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[300];
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sprintf(sql, "select count(*) as modulecount from modules;");
		if((rc = sqlite3_exec(db, sql, get_count_iomodules_callback, (void*)m, &err_msg)) != SQLITE_OK)
		{
		printf("Failed to select data, %s\n", err_msg);
		sqlite3_free(err_msg);
		}
		else // success
		{
		}
	}
	sqlite3_close(db);
}

void get_modules(iomodules *m)
{
	get_count_iomodules(m);
	get_iomodules(m);
}

void free_modules(iomodules *m)
{
	int i;

	for(i=0;i<m->count;i++)
		free(m->modules[i].devs.devices);
	free(m->modules);
}
