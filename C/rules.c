#include "rules.h"

int init_actuators_of_rule_callback(void *data, int argc, char **argv, char **azColName) 
{
	rule *r = (rule *)data;

	r->adevs[r->adevcount].modul = atoi(argv[0]);
	r->adevs[r->adevcount].chnnl = atoi(argv[1]);
	r->adevs[r->adevcount].devid = atoi(argv[2]);
	r->adevs[r->adevcount].dtype = atoi(argv[3]);
	r->adevs[r->adevcount].value = atoi(argv[4]);  
//printf("init_actuators_of_rule_callback %d %d %d %d\n", r->adevs[r->adevcount].value, r->adevs[r->adevcount].devid, r->adevs[r->adevcount].chnnl, r->adevcount);
	r->adevcount++;

	return(0);
}

void init_actuators_of_rule(rule *r)
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
		sprintf(sql, "select devices.modul, devices.chnnl, devices.devid, devices.dtype, agdev.value from agdev inner join devices on devices.modul = agdev.modul and devices.devid = agdev.devid where agdev.devgr = %d;", r->agrid);
		if((rc = sqlite3_exec(db, sql, init_actuators_of_rule_callback, (void*)r, &err_msg)) != SQLITE_OK)
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

int init_count_actuators_of_rule_callback(void *data, int argc, char **argv, char **azColName) 
{
	rule *r = (rule *)data;
  
	r->adevcount = atoi(argv[0]);
	r->adevs = malloc(r->adevcount*sizeof(adev));
//printf("init_count_actuators_of_rule_callback %d\n", r->adevcount);
	r->adevcount = 0;

	return(0);
}

void init_count_actuators_of_rule(rule *r)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[100];
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sprintf(sql, "select count(*) as actuatorcount from agdev where devgr = %d;", r->agrid);
		if((rc = sqlite3_exec(db, sql, init_count_actuators_of_rule_callback, (void*)r, &err_msg)) != SQLITE_OK)
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

int init_sensors_of_rule_callback(void *data, int argc, char **argv, char **azColName) 
{
	rule *r = (rule *)data;

	r->sdevs[r->sdevcount].modul = atoi(argv[0]);
	r->sdevs[r->sdevcount].chnnl = atoi(argv[1]);
	r->sdevs[r->sdevcount].devid = atoi(argv[2]);
	r->sdevs[r->sdevcount].dtype = atoi(argv[3]);
	r->sdevs[r->sdevcount].fromval = atoi(argv[4]);  
	r->sdevs[r->sdevcount].toval = atoi(argv[5]); 
//printf("init_sensors_of_rule_callback %d %d %d %d %d\n", r->sdevcount, r->sdevs[r->sdevcount].chnnl, r->sdevs[r->sdevcount].devid, r->sdevs[r->sdevcount].fromval, r->sdevs[r->sdevcount].toval);
	r->sdevcount++;

	return(0);
}

void init_sensors_of_rule(rule *r)
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
		sprintf(sql, "select devices.modul, devices.chnnl, devices.devid, devices.dtype, sgdev.fromval, sgdev.toval from sgdev inner join devices on devices.modul = sgdev.modul and devices.devid = sgdev.devid where sgdev.devgr = %d;", r->sgrid);
		if((rc = sqlite3_exec(db, sql, init_sensors_of_rule_callback, (void*)r, &err_msg)) != SQLITE_OK)
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

int init_count_sensors_of_rule_callback(void *data, int argc, char **argv, char **azColName) 
{
	rule *r = (rule *)data;
  
	r->sdevcount = atoi(argv[0]);
	r->sdevs = malloc(r->sdevcount*sizeof(sdev));
//printf("init_count_sensors_of_rule_callback %d\n", r->sdevcount);
	r->sdevcount = 0;

	return(0);
}

void init_count_sensors_of_rule(rule *r)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[100];
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sprintf(sql, "select count(*) as sensorcount from sgdev where devgr = %d;", r->sgrid);
		if((rc = sqlite3_exec(db, sql, init_count_sensors_of_rule_callback, (void*)r, &err_msg)) != SQLITE_OK)
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

int init_rules_of_interval_callback(void *data, int argc, char **argv, char **azColName) 
{
	ruleinterval *i = (ruleinterval *)data;

	i->rules[i->rulecount].rulid = atoi(argv[0]);
	i->rules[i->rulecount].sgrid = atoi(argv[1]);
	i->rules[i->rulecount].agrid = atoi(argv[2]);  
//printf("init_rules_of_interval_callback %d %d %d %d\n", i->rulecount, i->rules[i->rulecount].rulid, i->rules[i->rulecount].sgrid, i->rules[i->rulecount].agrid);
	init_count_sensors_of_rule(&(i->rules[i->rulecount]));
	init_sensors_of_rule(&(i->rules[i->rulecount]));
	init_count_actuators_of_rule(&(i->rules[i->rulecount]));
	init_actuators_of_rule(&(i->rules[i->rulecount]));

	i->rulecount++;

	return(0);
}

void init_rules_of_interval(ruleinterval *i)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[200];
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sprintf(sql, "select ruledef.rulid, ruledef.sgrid, ruledef.agrid from rules inner join ruledef on rules.rulid = ruledef.rulid where rules.intid = %d;", i->intid);
		if((rc = sqlite3_exec(db, sql, init_rules_of_interval_callback, (void*)i, &err_msg)) != SQLITE_OK)
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

int init_count_rules_of_interval_callback(void *data, int argc, char **argv, char **azColName) 
{
	ruleinterval *i = (ruleinterval *)data;
  
	i->rulecount = atoi(argv[0]);
	i->rules = malloc(i->rulecount*sizeof(rule));
//printf("init_count_rules_of_interval_callback %d\n", i->rulecount);

	i->rulecount = 0;

	return(0);
}

void init_count_rules_of_interval(ruleinterval *i)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char sql[100];
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sprintf(sql, "select count(*) as rulecount from rules where intid = %d;", i->intid);
		if((rc = sqlite3_exec(db, sql, init_count_rules_of_interval_callback, (void*)i, &err_msg)) != SQLITE_OK)
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

int init_rule_intervals_callback(void *data, int argc, char **argv, char **azColName) 
{
	rulescheduler *s = (rulescheduler *)data;
  
	s->intervals[s->intervalcount].intid = atoi(argv[0]);
	s->intervals[s->intervalcount].seconds = atoi(argv[2]);
	s->intervals[s->intervalcount].c = s->c;
//printf("init_intervals_callback %d %d %d\n", s->intervalcount, s->intervals[s->intervalcount].intid, s->intervals[s->intervalcount].seconds);
	init_count_rules_of_interval(&(s->intervals[s->intervalcount]));
	init_rules_of_interval(&(s->intervals[s->intervalcount]));

	s->intervalcount++;

	return(0);
}

void init_rule_intervals(rulescheduler *s)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char *sql = NULL;
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sql = "select * from intervals where itype = 'R';";
		if((rc = sqlite3_exec(db, sql, init_rule_intervals_callback, (void*)s, &err_msg)) != SQLITE_OK)
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

int init_count_rule_intervals_callback(void *data, int argc, char **argv, char **azColName) 
{
	rulescheduler *s = (rulescheduler *)data;

	s->intervalcount = atoi(argv[0]);
	s->intervals = malloc(s->intervalcount*sizeof(ruleinterval));
//printf("init_count_intervals_callback %d\n", s->intervalcount);
	s->intervalcount = 0;

	return(0);
}
  
void init_count_rule_intervals(rulescheduler *s)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char *sql = NULL;
	int rc;

	if ((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sql = "select count(*) as schedules from intervals where itype = 'R';";
		if ((rc = sqlite3_exec(db, sql, init_count_rule_intervals_callback, (void*)s, &err_msg)) != SQLITE_OK)
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

void* rule_interval_thread(void *args)
{
	ruleinterval *ival = (ruleinterval *)args;
	controller *c = ival->c;
	rule *r;
	sdev *s;
	adev *a;
	int i, j;
	unsigned char value;
	int sensorval;
	httpclient h;

	int ctype = PTHREAD_CANCEL_ASYNCHRONOUS;
	int ctype_old;
	pthread_setcanceltype(ctype, &ctype_old);

	ival->threadrunning = 1;
	do
	{
//printf("interval task %d\n", ival->intid);
		if (c)
			ichannel_read(c);

		for(i=0;i<ival->rulecount;i++)
		{
			sensorval = 1;
			r = &(ival->rules[i]);
			for(j=0;(j<r->sdevcount) && (sensorval);j++)
			{
				s = &(r->sdevs[j]);
				if (c)
				{
					switch (s->dtype)
					{
						case A_SWITCH:
						case A_LEVEL:
						case S_SWITCH:
						case S_LEVEL:
							value = iochannel_get_value(c, 0, s->chnnl);
							break;
						case A_BIT:
							value = obit_get_value(c, s->chnnl);
							break;
						default:
							value = 0;
							break;
					}
				}
				else
				{
					switch (s->dtype)
					{
						case S_SWITCH:
						case S_LEVEL:
							value = jsonReadChannel(&h, s->modul, s->chnnl, 0);
							break;
						case A_SWITCH:
						case A_LEVEL:
							value = jsonChannelGetValue(&h, s->modul, s->chnnl, 0);
							break;
						case A_BIT:
							value = jsonBitGetValue(&h, s->modul, s->chnnl, 0);
							break;
						default:
							value = 0;
							break;
					}
				}
//printf("input %d %d %d %d %d value %d\n", s->modul, s->chnnl, s->devid, s->fromval, s->toval, value);
				sensorval &= ((value >= s->fromval) && (value <= s->toval));
			}
			if (sensorval)
			{
				for(j=0;j<r->adevcount;j++)
				{
					a = &(r->adevs[j]);
//printf("output %d %d %d value %d\n", a->modul, a->chnnl, a->devid, a->value);
					if (c)
					{
						switch (a->dtype)
						{
							case A_SWITCH:
							case A_LEVEL:
							case S_SWITCH:
							case S_LEVEL:
								iochannel_set_value(c, a->modul, a->chnnl, (unsigned char)a->value);
								ochannel_write(c, TRUE);
								break;
							case A_PULSE:
								opulse_out(c, a->chnnl, (unsigned char)a->value);
								break;
							case A_BUTTON:
								iochannel_pulse(c, a->modul, a->chnnl);
								break;
							case A_BIT:
								obit_set_value(c, a->chnnl, (unsigned char)a->value);
								break;
							default :
								break;
						}
					}
					else
					{
						switch (a->dtype)
						{
							case A_SWITCH:
							case A_LEVEL:
							case S_SWITCH:
							case S_LEVEL:
								jsonWriteChannel(&h, a->modul, a->chnnl, 0, (unsigned char)a->value);
								break;
							case A_PULSE:
								jsonWritePulse(&h, a->modul, a->chnnl, 0, (unsigned char)a->value);
								break;
							case A_BUTTON:
								jsonWriteChannelPulse(&h, a->modul, a->chnnl, 0);
								break;
							case A_BIT:
								jsonWriteBit(&h, a->modul, a->chnnl, 0, (unsigned char)a->value);
							default :
								break;
						}
					}
				}
			}
		}
//printf("\n");
		sleep(ival->seconds);
	}while (ival->threadrunning);

	ival->retval = 0;
	pthread_exit(&(ival->retval));
}

void init_rule_interval_threads(rulescheduler *s)
{
	int i, err;

	for(i=0;i<s->intervalcount;i++)
	{
		err = pthread_create(&(s->intervals[i].tid), NULL, &rule_interval_thread, (void *)&(s->intervals[i]));
		if (err)
			printf("pthread_create error, interval %d\n", i);
	}
}

void close_rule_interval_threads(rulescheduler *s)
{
	int i, err;

	for(i=0;i<s->intervalcount;i++)
	{
		s->intervals[i].threadrunning = 0;
	}

	for(i=0;i<s->intervalcount;i++)
	{
		err = pthread_join(s->intervals[i].tid, NULL);
		if (err)
			printf("pthread_join error, interval %d\n", i);
	}
}

void init_rule_scheduler(rulescheduler *s, controller *c)
{
	s->c = c;
	init_count_rule_intervals(s);
	init_rule_intervals(s);
//printf("intervals %d\n", s->intervalcount);

	init_rule_interval_threads(s);
}

void close_rule_scheduler(rulescheduler *s)
{
	int i, j;

	close_rule_interval_threads(s);

	for(i=0;i<s->intervalcount;i++)
	{
		for(j=0;j<s->intervals[i].rulecount;j++)
		{
			free(s->intervals[i].rules[j].sdevs);
			free(s->intervals[i].rules[j].adevs);
		}
		free(s->intervals[i].rules);
	}
	free(s->intervals);
}
