#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <locale.h>

#include "rules.h"
#include "states.h"
#include "devices.h"
#include "httpget.h"

// compile with gcc -std=c99 -Wall -c "%f" -DIS_RPI -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -g -ftree-vectorize -pipe -DUSE_VCHIQ_ARM -Wno-psabi -mcpu=cortex-a53 -mfloat-abi=hard -mfpu=neon-fp-armv8 -mneon-for-64bits $(pkg-config --cflags gtk+-3.0) -Wno-deprecated-declarations
// link with gcc -std=c99 -Wall -o "%e" "%f" $(pkg-config --cflags gtk+-3.0) -Wl,--whole-archive -lrt -ldl -lm -Wl,--no-whole-archive -rdynamic $(pkg-config --libs gtk+-3.0) -lwiringPi

typedef struct
{
	GtkWidget *frame;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *control;
}devicewidgets;

typedef enum
{
	COL_MODUL,
	COL_DEVID,
	COL_DTEXT,
	COL_DTYPE,
	COL_NUMSTATES,
	COL_INITVAL,
	COL_DICON,
	COL_CHNNL,
	NUM_COLS
}devices_columns;

typedef struct
{
	int modul;
	int devid;
	char *dtext;
	int dtype;
	int numstates;
	int initval;
	char *dicon;
	int chnnl;
}deviceconfig;

typedef struct
{
	GtkWidget *configwindow;
	GtkWidget *configcontainer;
	GtkWidget *scrolled_window;
	GtkWidget *listview;
	GtkListStore *store;
	GtkTreeIter iter;
	GtkWidget *buttoncontainer;
	GtkWidget *add_mbutton;
	GtkWidget *modulewindow;
	GtkWidget *modulecontainer;
	GtkWidget *modulegrid;
	GtkWidget *label_modtx;
	GtkWidget *entry_modtx;
	GtkWidget *label_categ;
	GtkWidget *combo_categ;
	GtkWidget *add_button;
	GtkWidget *delete_button;
	GtkWidget *refresh_button;
	GtkWidget *editconfigwindow;
	GtkWidget *editconfigcontainer;
	GtkWidget *grid;
	GtkWidget *label_modul;
	GtkWidget *entry_modul;
	GtkWidget *label_devid;
	GtkWidget *entry_devid;
	GtkWidget *label_dtext;
	GtkWidget *entry_dtext;
	GtkWidget *label_dtype;
	GtkWidget *combo_dtype;
	GtkWidget *label_numstates;
	GtkWidget *entry_numstates;
	GtkWidget *label_initval;
	GtkWidget *entry_initval;
	GtkWidget *label_dicon;
	GtkWidget *entry_dicon;
	GtkWidget *label_chnnl;
	GtkWidget *entry_chnnl;
	GtkWidget *save_button;
	int mode;
	deviceconfig dc;
}configwidgets;

typedef struct
{
	GtkWidget *tablabel;
	GtkWidget *box;
	devicewidgets dw[8];
	GtkWidget *refresh_button;
}notebooktab;

typedef struct
{
	GtkWidget *window;
	GtkWidget *container;
	GtkWidget *notebook;
	notebooktab nt[10];
	configwidgets cw;
	controller *c;
	httpclient h;
	int usegpio;
	iomodules m; // from database
}controlpanel;

gboolean widget_state_set_gpio(GtkWidget *togglebutton, gboolean state, gpointer data)
{
	io_device *a = (io_device*)data;

	iochannel_set_value(a->parent, a->module, a->channel, (state?0x01:0x00));
	ochannel_write(a->parent, TRUE);

printf("%s module %d channel %d value %c\n", a->name, a->module, a->channel, (iochannel_get_value(a->parent, a->module, a->channel)?'1':'0'));

	return(TRUE);
}

gboolean widget_state_set_http(GtkWidget *togglebutton, gboolean state, gpointer data)
{
	io_device *a = (io_device*)data;

	httpclient h;
	jsonWriteChannel(&h, a->module, a->channel, 0, (state?0x01:0x00));

//printf("%s module %d channel %d value %c\n", a->name, a->module, a->channel, (iochannel_get_value(a->parent, a->module, a->channel)?'1':'0'));

	return(TRUE);
}

void combo_changed_gpio(GtkWidget *combo, gpointer data)
{
	io_device *a = (io_device*)data;
	gchar *strval;
	unsigned char value;

	g_object_get((gpointer)combo, "active-id", &strval, NULL);
	//printf("Selected id %s\n", strval);
	value = atoi((const char *)strval);
	g_free(strval);

	iochannel_set_value(a->parent, a->module, a->channel, value);
	ochannel_write(a->parent, TRUE);

printf("%s module %d channel %d value %c\n", a->name, a->module, a->channel, iochannel_get_value(a->parent, a->module, a->channel) + '0');
}

void combo_changed_http(GtkWidget *combo, gpointer data)
{
	io_device *a = (io_device*)data;
	gchar *strval;

	g_object_get((gpointer)combo, "active-id", &strval, NULL);
	//printf("Selected id %s\n", strval);
	a->value = atoi((const char *)strval);
	g_free(strval);

	httpclient h;
	jsonWriteChannel(&h, a->module, a->channel, 0, a->value);

//printf("%s module %d channel %d value %c\n", a->name, a->module, a->channel, iochannel_get_value(a->parent, a->module, a->channel) + '0');
}

gboolean widget_state_set_bit_gpio(GtkWidget *togglebutton, gboolean state, gpointer data)
{
	io_device *a = (io_device*)data;
	switch_values value = (state?SWITCH_ON:SWITCH_OFF);

	obit_set_value(a->parent, a->channel, value);

printf("%s module %d bit %d value %c\n", a->name, a->module, a->channel, (obit_get_value(a->parent, a->channel)?'1':'0'));

	return(TRUE);
}

gboolean widget_state_set_bit_http(GtkWidget *togglebutton, gboolean state, gpointer data)
{
	io_device *a = (io_device*)data;
	switch_values value = (state?SWITCH_ON:SWITCH_OFF);

	httpclient h;
	jsonWriteBit(&h, a->module, a->channel, 0, value);

//printf("%s module %d bit %d value %c\n", a->name, a->module, a->channel, (obit_get_value(a->parent, a->channel)?'1':'0'));

	return(TRUE);
}

static void button_clicked_pulse_gpio(GtkWidget *button, gpointer data)
{
	io_device *a = (io_device*)data;

	opulse_out(a->parent, a->channel, (int)a->value*100000);

printf("%s module %d bit %d value %d\n", a->name, a->module, a->channel, a->value);
}

static void button_clicked_pulse_http(GtkWidget *button, gpointer data)
{
	io_device *a = (io_device*)data;

	httpclient h;
	jsonWritePulse(&h, a->module, a->channel, 0, a->value);

//printf("%s module %d bit %d value %d\n", a->name, a->module, a->channel, a->value);
}

static void button_clicked_iochannel_gpio(GtkWidget *button, gpointer data)
{
	io_device *a = (io_device*)data;

	iochannel_pulse(a->parent, a->module, a->channel);

//printf("%s module %d channel %d value %d\n", a->name, a->module, a->channel, a->value);
}

static void button_clicked_iochannel_http(GtkWidget *button, gpointer data)
{
	io_device *a = (io_device*)data;

	httpclient h;
	jsonWriteChannelPulse(&h, a->module, a->channel, 0);

//printf("%s module %d channel %d value %d\n", a->name, a->module, a->channel, a->value);
}

// IO Module Interface

void add_moduletab(controlpanel *cp, controller *c, int modul)
{
	io_module *m = &(c->iomodules[modul]);

	cp->nt[modul].tablabel = gtk_label_new(m->name);
	cp->nt[modul].box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_notebook_append_page(GTK_NOTEBOOK(cp->notebook), cp->nt[modul].box, cp->nt[modul].tablabel);
}

int add_iomodule(controlpanel *cp, controller *c, int modul)
{
	int i;
	module *m = &(cp->m.modules[modul]);

	switch (i = module_add(c, m->modtx, m->mtype))
	{
		case CONTROLLER_FULL:
			printf("module_add() err=%d\n", i);
			break;
		default:
			add_moduletab(cp, c, modul);
			break;
	}
	return(i);
}

void add_iochannelframe(controlpanel *cp, controller *c, int modul, int channel)
{
// frame
	cp->nt[modul].dw[channel].frame = gtk_frame_new(c->iomodules[modul].gpiodevices[channel].name);
	//gtk_container_add(GTK_CONTAINER(ae->container), ae->frame);
	gtk_box_pack_start(GTK_BOX(cp->nt[modul].box), cp->nt[modul].dw[channel].frame, TRUE, TRUE, 0);

// horizontal box
	cp->nt[modul].dw[channel].hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel].frame), cp->nt[modul].dw[channel].hbox);
	//gtk_box_pack_start(GTK_BOX(ae->frame), ae->hbox, TRUE, TRUE, 0);

	GtkWidget *w;

	cp->nt[modul].dw[channel].label = gtk_label_new(c->iomodules[modul].gpiodevices[channel].name);
	//gtk_widget_set_size_request(cp->dw[channel].label, 100, 30);
	gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel].hbox), cp->nt[modul].dw[channel].label);

	switch (c->iomodules[modul].gpiodevices[channel].type)
	{
		case A_LEVEL:
		case S_LEVEL:
			cp->nt[modul].dw[channel].control = w = gtk_combo_box_text_new();

			int i;
			char s[10];
			for(i=0;i<c->iomodules[modul].gpiodevices[channel].numstates;i++)
			{
				sprintf(s, "%d", i);
				//printf("val = %s\n", s);
				gchar *gc = g_strnfill(10, '\0');
				g_stpcpy(gc, (const char *)s);
				gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w), gc, gc);
				g_free(gc);
			}
			sprintf(s, "%d", c->iomodules[modul].gpiodevices[channel].value);
			g_object_set((gpointer)w, "active-id", s, NULL);
			if (cp->usegpio)
				g_signal_connect(GTK_COMBO_BOX(w), "changed", G_CALLBACK(combo_changed_gpio), &(c->iomodules[modul].gpiodevices[channel]));
			else
				g_signal_connect(GTK_COMBO_BOX(w), "changed", G_CALLBACK(combo_changed_http), &(c->iomodules[modul].gpiodevices[channel]));
			gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel].hbox), w);
			//gtk_box_pack_start(GTK_BOX(ae->parameter[i].vbox), w, TRUE, TRUE, 0);
			break;
		case A_BUTTON:
			cp->nt[modul].dw[channel].control = w = gtk_button_new_with_label(c->iomodules[modul].gpiodevices[channel].name);
			if (cp->usegpio)
				g_signal_connect(GTK_BUTTON(w), "clicked", G_CALLBACK(button_clicked_iochannel_gpio), &(c->iomodules[modul].gpiodevices[channel]));
			else
				g_signal_connect(GTK_BUTTON(w), "clicked", G_CALLBACK(button_clicked_iochannel_http), &(c->iomodules[modul].gpiodevices[channel]));
			gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel].hbox), w);
			break;
		case A_SWITCH:
		case S_SWITCH:
		default:
			cp->nt[modul].dw[channel].control = w = gtk_switch_new();
			gtk_switch_set_active(GTK_SWITCH(w), c->iomodules[modul].gpiodevices[channel].value);
			if (cp->usegpio)
				g_signal_connect(GTK_SWITCH(w), "state-set", G_CALLBACK(widget_state_set_gpio), &(c->iomodules[modul].gpiodevices[channel]));
			else
				g_signal_connect(GTK_SWITCH(w), "state-set", G_CALLBACK(widget_state_set_http), &(c->iomodules[modul].gpiodevices[channel]));
			gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel].hbox), w);
			//gtk_box_pack_start(GTK_BOX(cp->dw[channel].hbox), w, TRUE, TRUE, 0);
			break;
	}
}

int add_iochanneldevice(controlpanel *cp, controller *c, int modul, char *name, device_type type, int nstates, unsigned char initval)
{
	int i;

	switch (i = iochannel_add(c, modul, name, type, nstates))
	{
		case CONTROLLER_INVALID_STATES:
			printf("iochannel_add() err=%d\n", i);
			break;
		case CONTROLLER_INVALID_DEVICE:
			printf("iochannel_add() err=%d\n", i);
			break;
		case CONTROLLER_FULL:
			printf("iochannel_add() err=%d\n", i);
			break;
		default:
			if (cp->usegpio)
				iochannel_set_value(c, modul, i, initval);
			else
				iochannel_set_value(c, modul, i, jsonChannelGetValue(&(cp->h), modul, i, 0));
			add_iochannelframe(cp, c, modul, i);
			break;
	}

	return(i);
}

void add_obitframe(controlpanel *cp, controller *c, int modul, int channel)
{
// frame
	cp->nt[modul].dw[channel].frame = gtk_frame_new(c->bits[channel].name);
	//gtk_container_add(GTK_CONTAINER(ae->container), ae->frame);
	gtk_box_pack_start(GTK_BOX(cp->nt[modul].box), cp->nt[modul].dw[channel].frame, TRUE, TRUE, 0);

// horizontal box
	cp->nt[modul].dw[channel].hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel].frame), cp->nt[modul].dw[channel].hbox);
	//gtk_box_pack_start(GTK_BOX(ae->frame), ae->hbox, TRUE, TRUE, 0);

	GtkWidget *w;

	cp->nt[modul].dw[channel].label = gtk_label_new(c->bits[channel].name);
	//gtk_widget_set_size_request(cp->dw[channel].label, 100, 30);
	gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel].hbox), cp->nt[modul].dw[channel].label);

	cp->nt[modul].dw[channel].control = w = gtk_switch_new();
	gtk_switch_set_active(GTK_SWITCH(w), c->bits[channel].value);
	if (cp->usegpio)
		g_signal_connect(GTK_SWITCH(w), "state-set", G_CALLBACK(widget_state_set_bit_gpio), &(c->bits[channel]));
	else
		g_signal_connect(GTK_SWITCH(w), "state-set", G_CALLBACK(widget_state_set_bit_http), &(c->bits[channel]));
	gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel].hbox), w);
	//gtk_box_pack_start(GTK_BOX(cp->dw[channel].hbox), w, TRUE, TRUE, 0);
}

int add_obitdevice(controlpanel *cp, controller *c, int modul, char *name, unsigned char initval)
{
	int i;

	switch (i = obit_add(c, modul, name))
	{
		case CONTROLLER_INVALID_STATES:
			printf("obit_add() err=%d\n", i);
			break;
		case CONTROLLER_INVALID_DEVICE:
			printf("obit_add() err=%d\n", i);
			break;
		case CONTROLLER_FULL:
			printf("obit_add() err=%d\n", i);
			break;
		default:
			if (cp->usegpio)
				obit_set_value(c, i, initval);
			else
				c->bits[i].value = jsonBitGetValue(&(cp->h), modul, i, 0);
			add_obitframe(cp, c, modul, i);
			break;
	}

	return(i);
}

void add_opulseframe(controlpanel *cp, controller *c, int modul, int channel)
{
// frame
	cp->nt[modul].dw[channel+c->obits].frame = gtk_frame_new(c->pulses[channel].name);
	//gtk_container_add(GTK_CONTAINER(ae->container), ae->frame);
	gtk_box_pack_start(GTK_BOX(cp->nt[modul].box), cp->nt[modul].dw[channel+c->obits].frame, TRUE, TRUE, 0);

// horizontal box
	cp->nt[modul].dw[channel+c->obits].hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel+c->obits].frame), cp->nt[modul].dw[channel+c->obits].hbox);
	//gtk_box_pack_start(GTK_BOX(ae->frame), ae->hbox, TRUE, TRUE, 0);

	GtkWidget *w;

	cp->nt[modul].dw[channel+c->obits].label = gtk_label_new(c->pulses[channel].name);
	//gtk_widget_set_size_request(cp->dw[channel].label, 100, 30);
	gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel+c->obits].hbox), cp->nt[modul].dw[channel+c->obits].label);

	cp->nt[modul].dw[channel+c->obits].control = w = gtk_button_new_with_label(c->pulses[channel].name);
	if (cp->usegpio)
		g_signal_connect(GTK_BUTTON(w), "clicked", G_CALLBACK(button_clicked_pulse_gpio), &(c->pulses[channel]));
	else
		g_signal_connect(GTK_BUTTON(w), "clicked", G_CALLBACK(button_clicked_pulse_http), &(c->pulses[channel]));
	gtk_container_add(GTK_CONTAINER(cp->nt[modul].dw[channel+c->obits].hbox), w);
	//gtk_box_pack_start(GTK_BOX(cp->dw[channel].hbox), w, TRUE, TRUE, 0);
}

int add_opulsedevice(controlpanel *cp, controller *c, int modul, char *name, unsigned char initval)
{
	int i;

	switch (i = opulse_add(c, modul, name))
	{
		case CONTROLLER_INVALID_STATES:
			printf("opulse_add() err=%d\n", i);
			break;
		case CONTROLLER_INVALID_DEVICE:
			printf("opulse_add() err=%d\n", i);
			break;
		case CONTROLLER_FULL:
			printf("opulse_add() err=%d\n", i);
			break;
		default:
			if (cp->usegpio)
				opulse_set_value(c, i, initval);
			else
				c->pulses[i].value = jsonPulseGetValue(&(cp->h), modul, i, 0);
			add_opulseframe(cp, c, modul, i);
			break;
	}

	return(i);
}

static void drbutton_clicked_gpio(GtkWidget *button, gpointer data)
{
	io_module *m = (io_module *)data;
	controller *c = m->parent;
	controlpanel* cp = (controlpanel *)c->parent;
	int i;
	GtkWidget *w;
	char s[10];
	unsigned char value;

	if (m->moduletype==CONTROL)
	{
		for(i=0;i<c->bindex;i++)
		{
			w = cp->nt[m->module].dw[i].control;
			value = obit_get_value(c, i);
			gtk_switch_set_active(GTK_SWITCH(w), value);
		}
	}

	for(i=0;i<m->index;i++)
	{
		w = cp->nt[m->module].dw[i].control;
		switch (m->gpiodevices[i].type)
		{
			case A_LEVEL:
			case S_LEVEL:
				value = iochannel_get_value(c, m->module, i);
				sprintf(s, "%d", value);
				g_object_set((gpointer)w, "active-id", s, NULL);
//printf("%d) %s value %d\n", i, m->gpiodevices[i].name, value);
				break;
			case A_BUTTON:
				break;
			case A_SWITCH:
			case S_SWITCH:
			default:
				value = iochannel_get_value(c, m->module, i);
				gtk_switch_set_active(GTK_SWITCH(w), value);
//printf("%d) %s value %d\n", i, m->gpiodevices[i].name, m->gpiodevices[i].value);
				break;
		}
	}
}

static void drbutton_clicked_http(GtkWidget *button, gpointer data)
{
	io_module *m = (io_module *)data;
	controller *c = m->parent;
	controlpanel* cp = (controlpanel *)c->parent;
	int i;
	GtkWidget *w;
	char s[10];
	unsigned char value;

	for(i=0;i<m->index;i++)
	{
		w = cp->nt[m->module].dw[i].control;
		switch (m->gpiodevices[i].type)
		{
			case A_LEVEL:
			case S_LEVEL:
				value = jsonChannelGetValue(&(cp->h), m->module, i, 0);
				sprintf(s, "%d", value);
				g_object_set((gpointer)w, "active-id", s, NULL);
printf("%d) %s value %d\n", i, m->gpiodevices[i].name, value);
				break;
			case A_BUTTON:
				break;
			case A_SWITCH:
			case S_SWITCH:
			default:
				value = jsonChannelGetValue(&(cp->h), m->module, i, 0);
				gtk_switch_set_active(GTK_SWITCH(w), value);
printf("%d) %s value %d\n", i, m->gpiodevices[i].name, m->gpiodevices[i].value);
				break;
		}
	}
}

/*
static void srbutton_clicked_gpio(GtkWidget *button, gpointer data)
{
	controlpanel* cp = (controlpanel *)data;
	controller *c = cp->c;
	int i;
	GtkWidget *w;
	char s[10];

	ichannel_read(c);

	for(i=0;c->iomodules[0].gpiodevices[i].type!=D_VOID;i++)
	{
		w = cp->idw[i].control;
		switch (c->iomodules[0].gpiodevices[i].type)
		{
			case S_LEVEL:
				sprintf(s, "%d", c->iomodules[0].gpiodevices[i].value);
				g_object_set((gpointer)w, "active-id", s, NULL);
				break;
			case S_SWITCH:
			default:
				gtk_switch_set_active(GTK_SWITCH(w), c->iomodules[0].gpiodevices[i].value);
				break;
		}
		//printf("%s = %c\n", c->sensors[i].name, ichannel_get_value(c, i) + '0');
	}
	
//printf("ichannel_get_value(0) = %d\n", ichannel_get_value(c, 0));
}

static void srbutton_clicked_http(GtkWidget *button, gpointer data)
{
	controlpanel* cp = (controlpanel *)data;
	controller *c = cp->c;
	iodevices *sen = cp->sen;
	int i;
	GtkWidget *w;
	char s[10];

	for(i=0;i<sen->count;i++)
	{
		w = cp->idw[i].control;
		c->iomodules[0].gpiodevices[i].value = jsonReadChannel(&(cp->h), i, 0);
		switch (c->iomodules[0].gpiodevices[i].type)
		{
			case S_LEVEL:
				sprintf(s, "%d", c->iomodules[0].gpiodevices[i].value);
				g_object_set((gpointer)w, "active-id", s, NULL);
				break;
			case S_SWITCH:
			default:
				gtk_switch_set_active(GTK_SWITCH(w), c->iomodules[0].gpiodevices[i].value);
				break;
		}
		//printf("%s = %c\n", c->sensors[i].name, ichannel_get_value(c, i) + '0');
	}
	
//printf("ichannel_get_value(0) = %d\n", ichannel_get_value(c, 0));
}
*/

// Configuration Interface

int fill_combo_dtype_callback(void *data, int argc, char **argv, char **azColName) 
{
	GtkWidget *w = (GtkWidget *)data;
	char dtext[30];

	strcpy(dtext, argv[1]);
	strcat(dtext, " - ");
	strcat(dtext, argv[2]);
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w), argv[0], dtext);

	return 0;
}

void fill_combo_dtype(GtkWidget *combo)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char *sql = NULL;
	int rc;

	if((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sql = "SELECT * FROM devicetypes where dtype <> 0;";
		if((rc = sqlite3_exec(db, sql, fill_combo_dtype_callback, (void*)combo, &err_msg)) != SQLITE_OK)
		{
			printf("Failed to select data, %s\n", err_msg);
			sqlite3_free(err_msg);
		}
		else
		{
// success
		}
	}
	sqlite3_close(db);
}

int fill_combo_categ_callback(void *data, int argc, char **argv, char **azColName) 
{
	GtkWidget *w = (GtkWidget *)data;

	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(w), argv[0], argv[1]);

	return 0;
}

void fill_combo_categ(GtkWidget *combo)
{
	sqlite3 *db;
	char *err_msg = NULL;
	char *sql = NULL;
	int rc;

	if((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sql = "SELECT * FROM devcat where allow = 1;";
		if((rc = sqlite3_exec(db, sql, fill_combo_categ_callback, (void*)combo, &err_msg)) != SQLITE_OK)
		{
			printf("Failed to select data, %s\n", err_msg);
			sqlite3_free(err_msg);
		}
		else
		{
// success
		}
	}
	sqlite3_close(db);
}

static gboolean delete_event_edit_config(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return FALSE; // return FALSE to emit destroy signal
}

static void destroy_edit_config(GtkWidget *widget, gpointer data)
{
}

static void realize_cb_edit_config(GtkWidget *widget, gpointer data)
{
}

int select_callback(void *data, int argc, char **argv, char **azColName) 
{
	controlpanel* cp = (controlpanel *)data;
	configwidgets *cw = &(cp->cw);

	gtk_list_store_append(cw->store, &(cw->iter));
	gtk_list_store_set(cw->store, &(cw->iter), COL_MODUL, atoi(argv[0]), COL_DEVID, atoi(argv[1]), COL_DTEXT, argv[2], COL_DTYPE, atoi(argv[3]), COL_NUMSTATES, atoi(argv[4]), COL_INITVAL, atoi(argv[5]), COL_DICON, argv[6], COL_CHNNL, atoi(argv[7]), -1);

//g_print("%s, %s\n", argv[0], argv[1]);
	return 0;
}

static GtkTreeModel* create_and_fill_model(controlpanel *cp)
{
	configwidgets *cw = &(cp->cw);
	sqlite3 *db;
	char *err_msg = NULL;
	char *sql = NULL;
	int rc;

	cw->store = gtk_list_store_new(NUM_COLS, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_UINT);

	if((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		sql = "SELECT devices.modul, devices.devid, devices.dtext, devices.dtype, devices.numstates, devices.initval, devices.dicon, devices.chnnl FROM devices order by modul, devid;";
		if((rc = sqlite3_exec(db, sql, select_callback, (void*)cp, &err_msg)) != SQLITE_OK)
		{
			printf("Failed to select data, %s\n", err_msg);
			sqlite3_free(err_msg);
		}
		else
		{
// success
		}
	}
	sqlite3_close(db);

	return GTK_TREE_MODEL(cw->store);
}

static void refresh_button_clicked(GtkWidget *button, gpointer data)
{
	controlpanel *cp = (controlpanel *)data;
	configwidgets *cw = &(cp->cw);

	GtkTreeModel *model;
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(cw->listview));

	gtk_tree_view_set_model(GTK_TREE_VIEW(cw->listview), NULL); /* Detach model from view */
	g_object_unref(model);
	model = create_and_fill_model(cp); // insert rows from db
	gtk_tree_view_set_model(GTK_TREE_VIEW(cw->listview), model); /* Re-attach model to view */
}

static void save_button_clicked(GtkWidget *button, gpointer data)
{
	controlpanel* cp = (controlpanel *)data;
	configwidgets *cw = &(cp->cw);
	deviceconfig *dc = &(cw->dc);

	sqlite3 *db;
	char *err_msg = NULL;
	char sql[250];
	int rc;

	if((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		switch (cw->mode)
		{
			case 0: // Create
					sprintf(sql, "insert into devices values(%d, %d, '%s', %d, %d, %d, '%s', %d);", atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_modul))), atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_devid))), gtk_entry_get_text(GTK_ENTRY(cw->entry_dtext)), gtk_combo_box_get_active(GTK_COMBO_BOX(cw->combo_dtype)), atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_numstates))), atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_initval))), gtk_entry_get_text(GTK_ENTRY(cw->entry_dicon)), atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_chnnl))));
					break;
			case 1: // Update
					sprintf(sql, "update devices set dtext='%s', dtype=%d, numstates=%d, initval=%d, dicon='%s', chnnl=%d where modul=%d and devid=%d;", gtk_entry_get_text(GTK_ENTRY(cw->entry_dtext)), gtk_combo_box_get_active(GTK_COMBO_BOX(cw->combo_dtype)), atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_numstates))), atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_initval))), gtk_entry_get_text(GTK_ENTRY(cw->entry_dicon)), atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_chnnl))), atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_modul))), atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_devid))));
					break;
			case 2: // Delete
					sprintf(sql, "delete from devices where modul=%d and devid=%d;", dc->modul, dc->devid);
					break;
			default:
					break;
		}
//printf("%s\n", sql);
		if((rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg)) != SQLITE_OK)
		{
			printf("Failed to update data, %s\n", err_msg);
			sqlite3_free(err_msg);
		}
		else
		{
// success
			switch (cw->mode)
			{
				case 0:
				case 1:
						gtk_window_close(GTK_WINDOW(cw->editconfigwindow));
						break;
				case 2:
				default:
						break;
			}
			refresh_button_clicked(cw->refresh_button, (void*)cp);
		}
	}
	sqlite3_close(db);
}

void edit_device_config(controlpanel *cp)
{
	configwidgets *cw = &(cp->cw);
	deviceconfig *dc = &(cw->dc);
	char s[50];

	/* create a new window */
	cw->editconfigwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(cw->editconfigwindow), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(cw->editconfigwindow), 2);
	gtk_widget_set_size_request(cw->editconfigwindow, 300, 400);
	gtk_window_set_title(GTK_WINDOW(cw->editconfigwindow), "Edit Configuration");
	gtk_window_set_resizable(GTK_WINDOW(cw->editconfigwindow), TRUE);
	gtk_window_set_modal(GTK_WINDOW(cw->editconfigwindow), TRUE);
	/* When the window is given the "delete-event" signal (this is given
	* by the window manager, usually by the "close" option, or on the
	* titlebar), we ask it to call the delete_event () function
	* as defined above. The data passed to the callback
	* function is NULL and is ignored in the callback function. */
	g_signal_connect(cw->editconfigwindow, "delete-event", G_CALLBACK(delete_event_edit_config), NULL);
	/* Here we connect the "destroy" event to a signal handler.  
	* This event occurs when we call gtk_widget_destroy() on the window,
	* or if we return FALSE in the "delete-event" callback. */
	g_signal_connect(cw->editconfigwindow, "destroy", G_CALLBACK(destroy_edit_config), NULL);
	g_signal_connect(cw->editconfigwindow, "realize", G_CALLBACK(realize_cb_edit_config), NULL);

	cw->editconfigcontainer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(cw->editconfigwindow), cw->editconfigcontainer);

	cw->grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(cw->editconfigcontainer), cw->grid);

	// MODUL
	cw->label_modul = gtk_label_new("Module ID");
	gtk_grid_attach(GTK_GRID(cw->grid), cw->label_modul, 0, 0, 1, 1);

	cw->entry_modul = gtk_entry_new();
	if (cw->mode==1)
	{
		sprintf(s, "%d", dc->modul);
		gtk_entry_set_text(GTK_ENTRY(cw->entry_modul), (const gchar *)s);
		gtk_widget_set_sensitive(cw->entry_modul, FALSE);
	}
	gtk_grid_attach(GTK_GRID(cw->grid), cw->entry_modul, 1, 0, 1, 1);

	// DEVID
	cw->label_devid = gtk_label_new("Device ID");
	gtk_grid_attach(GTK_GRID(cw->grid), cw->label_devid, 0, 1, 1, 1);

	cw->entry_devid = gtk_entry_new();
	if (cw->mode==1)
	{
		sprintf(s, "%d", dc->devid);
		gtk_entry_set_text(GTK_ENTRY(cw->entry_devid), (const gchar *)s);
		gtk_widget_set_sensitive(cw->entry_devid, FALSE);
	}
	gtk_grid_attach(GTK_GRID(cw->grid), cw->entry_devid, 1, 1, 1, 1);

	// DTEXT
	cw->label_dtext = gtk_label_new("Device Name");
	gtk_grid_attach(GTK_GRID(cw->grid), cw->label_dtext, 0, 2, 1, 1);

	cw->entry_dtext = gtk_entry_new();
	if (cw->mode==1)
	{
		gtk_entry_set_text(GTK_ENTRY(cw->entry_dtext), (const gchar *)dc->dtext);
	}
	gtk_grid_attach(GTK_GRID(cw->grid), cw->entry_dtext, 1, 2, 1, 1);

	// DTYPE
	cw->label_dtype = gtk_label_new("Device Type");
	gtk_grid_attach(GTK_GRID(cw->grid), cw->label_dtype, 0, 3, 1, 1);

	cw->combo_dtype = gtk_combo_box_text_new();
	fill_combo_dtype(cw->combo_dtype);
	if (!cw->mode)
	{
		dc->dtype = 0;
		sprintf(s, "%d", dc->dtype);
		g_object_set((gpointer)cw->combo_dtype, "active-id", s, NULL);
	}
	else if (cw->mode==1)
	{
		sprintf(s, "%d", dc->dtype);
		g_object_set((gpointer)cw->combo_dtype, "active-id", s, NULL);
	}
	gtk_grid_attach(GTK_GRID(cw->grid), cw->combo_dtype, 1, 3, 1, 1);

	// NUMSTATES
	cw->label_numstates = gtk_label_new("Device States");
	gtk_grid_attach(GTK_GRID(cw->grid), cw->label_numstates, 0, 4, 1, 1);

	cw->entry_numstates = gtk_entry_new();
	if (cw->mode==1)
	{
		sprintf(s, "%d", dc->numstates);
		gtk_entry_set_text(GTK_ENTRY(cw->entry_numstates), (const gchar *)s);
	}
	gtk_grid_attach(GTK_GRID(cw->grid), cw->entry_numstates, 1, 4, 1, 1);

	// INITVAL
	cw->label_initval = gtk_label_new("Initial Value");
	gtk_grid_attach(GTK_GRID(cw->grid), cw->label_initval, 0, 5, 1, 1);

	cw->entry_initval = gtk_entry_new();
	if (cw->mode==1)
	{
		sprintf(s, "%d", dc->initval);
		gtk_entry_set_text(GTK_ENTRY(cw->entry_initval), (const gchar *)s);
	}
	gtk_grid_attach(GTK_GRID(cw->grid), cw->entry_initval, 1, 5, 1, 1);

	// DICON
	cw->label_dicon = gtk_label_new("Device Icon");
	gtk_grid_attach(GTK_GRID(cw->grid), cw->label_dicon, 0, 6, 1, 1);

	cw->entry_dicon = gtk_entry_new();
	if (cw->mode==1)
	{
		gtk_entry_set_text(GTK_ENTRY(cw->entry_dicon), (const gchar *)dc->dicon);
	}
	gtk_grid_attach(GTK_GRID(cw->grid), cw->entry_dicon, 1, 6, 1, 1);

	// CHNNL
	cw->label_chnnl = gtk_label_new("Device Channel");
	gtk_grid_attach(GTK_GRID(cw->grid), cw->label_chnnl, 0, 7, 1, 1);

	cw->entry_chnnl = gtk_entry_new();
	if (cw->mode==1)
	{
		sprintf(s, "%d", dc->chnnl);
		gtk_entry_set_text(GTK_ENTRY(cw->entry_chnnl), (const gchar *)s);
	}
	gtk_grid_attach(GTK_GRID(cw->grid), cw->entry_chnnl, 1, 7, 1, 1);

	cw->save_button = gtk_button_new_with_label("Save");
	g_signal_connect(GTK_BUTTON(cw->save_button), "clicked", G_CALLBACK(save_button_clicked), (void*)cp);
	gtk_grid_attach(GTK_GRID(cw->grid), cw->save_button, 1, 8, 1, 1);

	gtk_widget_show_all(cw->editconfigwindow);
}

static GtkWidget* create_view_and_model(controlpanel* cp)
{
	GtkCellRenderer *renderer;
	GtkTreeModel *model;
	GtkWidget *view;

	view = gtk_tree_view_new();

	// Column 1
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Module ID", renderer, "text", COL_MODUL, NULL);

	// Column 2
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Device ID", renderer, "text", COL_DEVID, NULL);

	// Column 3
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(view), -1, "Device", renderer, "text", COL_DTEXT, NULL);

	// Column 4
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Device Type", renderer, "text", COL_DTYPE, NULL);

	// Column 5
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(view), -1, "Device States", renderer, "text", COL_NUMSTATES, NULL);

	// Column 6
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(view), -1, "Initial Value", renderer, "text", COL_INITVAL, NULL);

	// Column 7
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(view), -1, "Device Icon", renderer, "text", COL_DICON, NULL);

	// Column 8
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(view), -1, "Channel", renderer, "text", COL_CHNNL, NULL);

	model = create_and_fill_model(cp);

	gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

/* The tree view has acquired its own reference to the model, so we can drop ours. That way the model will
   be freed automatically when the tree view is destroyed */
//g_object_unref(model);

	return view;
}

void listview_onRowActivated(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata)
{
	controlpanel* cp = (controlpanel *)userdata;
	configwidgets *cw = &(cp->cw);
	deviceconfig *dc = &(cw->dc);
	GtkTreeModel *model;

//g_print("double-clicked\n");
	cw->mode = 1; // Edit
	model = gtk_tree_view_get_model(treeview);
	if (gtk_tree_model_get_iter(model, &(cw->iter), path))
	{
		gtk_tree_model_get(model, &(cw->iter), COL_MODUL, &(dc->modul), COL_DEVID, &(dc->devid), COL_DTEXT, &(dc->dtext), COL_DTYPE, &(dc->dtype), COL_NUMSTATES, &(dc->numstates), COL_INITVAL, &(dc->initval), COL_DICON, &(dc->dicon), COL_CHNNL, &(dc->chnnl), -1);
		edit_device_config(cp);
		g_free(dc->dtext);
		g_free(dc->dicon);
	}
}

static void save_module_button_clicked(GtkWidget *button, gpointer data)
{
	controlpanel* cp = (controlpanel *)data;
	configwidgets *cw = &(cp->cw);

	sqlite3 *db;
	char *err_msg = NULL;
	char sql[250];
	int rc;
	gchar *strval;

	if((rc = sqlite3_open(DBPATH, &db)))
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
//printf("Opened database successfully\n");
		g_object_get((gpointer)cw->combo_categ, "active-id", &strval, NULL);
		//printf("Selected id %s\n", strval);
		sprintf(sql, "insert into modules values(%d, '%s', '%s');", atoi(gtk_entry_get_text(GTK_ENTRY(cw->entry_modul))), gtk_entry_get_text(GTK_ENTRY(cw->entry_modtx)), strval);
		g_free(strval);
		if((rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg)) != SQLITE_OK)
		{
			printf("Failed to update data, %s\n", err_msg);
			sqlite3_free(err_msg);
		}
		else
		{
// success
			gtk_window_close(GTK_WINDOW(cw->modulewindow));
		}
	}
	sqlite3_close(db);
}

static gboolean delete_add_module(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return FALSE; // return FALSE to emit destroy signal
}

static void destroy_add_module(GtkWidget *widget, gpointer data)
{
}

static void realize_cb_add_module(GtkWidget *widget, gpointer data)
{
}

static void add_mbutton_clicked(GtkWidget *button, gpointer data)
{
	controlpanel *cp = (controlpanel *)data;
	configwidgets *cw = &(cp->cw);

	/* create a new window */
	cw->modulewindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(cw->modulewindow), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(cw->modulewindow), 2);
	gtk_widget_set_size_request(cw->modulewindow, 200, 200);
	gtk_window_set_title(GTK_WINDOW(cw->modulewindow), "Add Module");
	gtk_window_set_resizable(GTK_WINDOW(cw->modulewindow), TRUE);
	gtk_window_set_modal(GTK_WINDOW(cw->modulewindow), TRUE);
	/* When the window is given the "delete-event" signal (this is given
	* by the window manager, usually by the "close" option, or on the
	* titlebar), we ask it to call the delete_event () function
	* as defined above. The data passed to the callback
	* function is NULL and is ignored in the callback function. */
	g_signal_connect(cw->modulewindow, "delete-event", G_CALLBACK(delete_add_module), NULL);
	/* Here we connect the "destroy" event to a signal handler.  
	* This event occurs when we call gtk_widget_destroy() on the window,
	* or if we return FALSE in the "delete-event" callback. */
	g_signal_connect(cw->modulewindow, "destroy", G_CALLBACK(destroy_add_module), NULL);
	g_signal_connect(cw->modulewindow, "realize", G_CALLBACK(realize_cb_add_module), NULL);

	cw->modulecontainer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(cw->modulewindow), cw->modulecontainer);

	cw->modulegrid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(cw->modulecontainer), cw->modulegrid);

	// MODUL
	cw->label_modul = gtk_label_new("Module ID");
	gtk_grid_attach(GTK_GRID(cw->modulegrid), cw->label_modul, 0, 0, 1, 1);

	cw->entry_modul = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(cw->modulegrid), cw->entry_modul, 1, 0, 1, 1);

	// MODTX
	cw->label_modtx = gtk_label_new("Module Name");
	gtk_grid_attach(GTK_GRID(cw->modulegrid), cw->label_modtx, 0, 2, 1, 1);

	cw->entry_modtx = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(cw->modulegrid), cw->entry_modtx, 1, 2, 1, 1);

	// CATEG
	cw->label_categ = gtk_label_new("Category");
	gtk_grid_attach(GTK_GRID(cw->modulegrid), cw->label_categ, 0, 3, 1, 1);

	cw->combo_categ = gtk_combo_box_text_new();
	fill_combo_categ(cw->combo_categ);
	g_object_set((gpointer)cw->combo_categ, "active-id", "A", NULL);
	gtk_grid_attach(GTK_GRID(cw->modulegrid), cw->combo_categ, 1, 3, 1, 1);

	cw->save_button = gtk_button_new_with_label("Save");
	g_signal_connect(GTK_BUTTON(cw->save_button), "clicked", G_CALLBACK(save_module_button_clicked), (void*)cp);
	gtk_grid_attach(GTK_GRID(cw->modulegrid), cw->save_button, 1, 4, 1, 1);

	gtk_widget_show_all(cw->modulewindow);
}

static void add_button_clicked(GtkWidget *button, gpointer data)
{
	controlpanel *cp = (controlpanel *)data;
	configwidgets *cw = &(cp->cw);

	cw->mode = 0; // Create
	edit_device_config(cp);
}

static void delete_button_clicked(GtkWidget *button, gpointer data)
{
	controlpanel *cp = (controlpanel *)data;
	configwidgets *cw = &(cp->cw);
	deviceconfig *dc = &(cw->dc);

	GtkTreeModel *model;
	GtkTreeSelection *selection;

	cw->mode = 2; // Delete

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(cw->listview));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(cw->listview));
	gtk_tree_selection_get_selected(selection, &model, &(cw->iter));
	gtk_tree_model_get(model, &(cw->iter), COL_MODUL, &(dc->modul), COL_DEVID, &(dc->devid), COL_DTEXT, &(dc->dtext), COL_DTYPE, &(dc->dtype), COL_NUMSTATES, &(dc->numstates), COL_INITVAL, &(dc->initval), COL_DICON, &(dc->dicon), COL_CHNNL, &(dc->chnnl), -1);
	save_button_clicked(cw->save_button, (void*)cp);
//printf("%d\n", dc.devid);
	g_free(dc->dtext);
	g_free(dc->dicon);
}

static gboolean delete_event_config(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return FALSE; // return FALSE to emit destroy signal
}

static void destroy_config(GtkWidget *widget, gpointer data)
{
}

static void realize_cb_config(GtkWidget *widget, gpointer data)
{
}

void display_config(controlpanel *cp)
{
	configwidgets *cw = &(cp->cw);

	/* create a new window */
	cw->configwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(cw->configwindow), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(cw->configwindow), 2);
	gtk_widget_set_size_request(cw->configwindow, 500, 400);
	gtk_window_set_title(GTK_WINDOW(cw->configwindow), "Configuration");
	gtk_window_set_resizable(GTK_WINDOW(cw->configwindow), TRUE);
	gtk_window_set_modal(GTK_WINDOW(cw->configwindow), TRUE);
	/* When the window is given the "delete-event" signal (this is given
	* by the window manager, usually by the "close" option, or on the
	* titlebar), we ask it to call the delete_event () function
	* as defined above. The data passed to the callback
	* function is NULL and is ignored in the callback function. */
	g_signal_connect(cw->configwindow, "delete-event", G_CALLBACK(delete_event_config), NULL);
	/* Here we connect the "destroy" event to a signal handler.  
	* This event occurs when we call gtk_widget_destroy() on the window,
	* or if we return FALSE in the "delete-event" callback. */
	g_signal_connect(cw->configwindow, "destroy", G_CALLBACK(destroy_config), NULL);
	g_signal_connect(cw->configwindow, "realize", G_CALLBACK(realize_cb_config), NULL);

	cw->configcontainer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(cw->configwindow), cw->configcontainer);

	cw->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_set_border_width(GTK_CONTAINER(cw->scrolled_window), 10);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(cw->scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

	gtk_box_pack_start(GTK_BOX(cw->configcontainer), cw->scrolled_window, TRUE, TRUE, 0);

	cw->listview = create_view_and_model(cp);
	g_signal_connect(cw->listview, "row-activated", (GCallback)listview_onRowActivated, (void*)cp);
	gtk_container_add(GTK_CONTAINER(cw->scrolled_window), cw->listview);

	cw->buttoncontainer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(cw->configcontainer), cw->buttoncontainer);

	cw->add_mbutton = gtk_button_new_with_label("Add Module");
	g_signal_connect(GTK_BUTTON(cw->add_mbutton), "clicked", G_CALLBACK(add_mbutton_clicked), (void*)cp);
	gtk_container_add(GTK_CONTAINER(cw->buttoncontainer), cw->add_mbutton);

	cw->add_button = gtk_button_new_with_label("Add");
	g_signal_connect(GTK_BUTTON(cw->add_button), "clicked", G_CALLBACK(add_button_clicked), (void*)cp);
	gtk_container_add(GTK_CONTAINER(cw->buttoncontainer), cw->add_button);

	cw->delete_button = gtk_button_new_with_label("Delete");
	g_signal_connect(GTK_BUTTON(cw->delete_button), "clicked", G_CALLBACK(delete_button_clicked), (void*)cp);
	gtk_container_add(GTK_CONTAINER(cw->buttoncontainer), cw->delete_button);

	cw->refresh_button = gtk_button_new_with_label("Refresh");
	g_signal_connect(GTK_BUTTON(cw->refresh_button), "clicked", G_CALLBACK(refresh_button_clicked), (void*)cp);
	gtk_container_add(GTK_CONTAINER(cw->buttoncontainer), cw->refresh_button);

	gtk_widget_show_all(cw->configwindow);
}

static void configbutton_clicked(GtkWidget *button, gpointer data)
{
	controlpanel* cp = (controlpanel *)data;

	display_config(cp);
}

// Main

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return FALSE; // return FALSE to emit destroy signal
}

static void destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

static void realize_cb(GtkWidget *widget, gpointer data)
{
}

void setup_default_icon(char *filename)
{
	GdkPixbuf *pixbuf;
	GError *err;

	err = NULL;
	pixbuf = gdk_pixbuf_new_from_file(filename, &err);

	if (pixbuf)
	{
		GList *list;      

		list = NULL;
		list = g_list_append(list, pixbuf);
		gtk_window_set_default_icon_list(list);
		g_list_free(list);
		g_object_unref(pixbuf);
	}
}

int main(int argc, char **argv)
{
	controller *c = NULL;
	int i, j, err;
	controlpanel cp;
	//rulescheduler rs;
	//statescheduler ss;

	int usegpio = 0;

	setlocale(LC_ALL, "tr_TR.UTF-8");
	setup_default_icon("./images/controller.png");

	if (argc>1)
	{
		if (!strcmp(argv[1], "gpio"))
		{
			usegpio = 1;
		}
	}
	cp.usegpio = usegpio;

	c = controller_open(V1, usegpio);
	if (c->err)
		printf("Open err=%d\n", c->err);
	cp.c = c;
	c->parent = (void *)&cp;

	gtk_init(&argc, &argv);

	/* create a new window */
	cp.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(cp.window), GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(cp.window), 2);
	//gtk_widget_set_size_request(window, 100, 100);
	gtk_window_set_title(GTK_WINDOW(cp.window), "Controller");
	gtk_window_set_resizable(GTK_WINDOW(cp.window), TRUE);
	/* When the window is given the "delete-event" signal (this is given
	* by the window manager, usually by the "close" option, or on the
	* titlebar), we ask it to call the delete_event () function
	* as defined above. The data passed to the callback
	* function is NULL and is ignored in the callback function. */
	g_signal_connect(cp.window, "delete-event", G_CALLBACK (delete_event), NULL);
	/* Here we connect the "destroy" event to a signal handler.  
	* This event occurs when we call gtk_widget_destroy() on the window,
	* or if we return FALSE in the "delete-event" callback. */
	g_signal_connect(cp.window, "destroy", G_CALLBACK (destroy), NULL);
	g_signal_connect(cp.window, "realize", G_CALLBACK (realize_cb), NULL);

	cp.container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(cp.window), cp.container);
	cp.notebook = gtk_notebook_new();
	//gtk_container_add(GTK_CONTAINER(cp.container), cp.notebook);
	gtk_box_pack_start(GTK_BOX(cp.container), cp.notebook, TRUE, TRUE, 0);

	get_modules(&(cp.m));

	for(i=0;i<cp.m.count;i++)
	{
		module *m = &(cp.m.modules[i]);
		add_iomodule(&cp, c, i);
		for(j=0;j<m->devs.count;j++)
		{
			device *d = &(m->devs.devices[j]);
			switch (d->dtype)
			{
				case A_SWITCH:
				case A_LEVEL:
				case A_BUTTON:
				case S_SWITCH:
				case S_LEVEL:
					//printf("iochannel module %d, devid %d, dtext %s, chnnl %d\n", d->modul, d->devid, d->dtext, d->chnnl);
					if ((err=add_iochanneldevice(&cp, c, m->modul, d->dtext, d->dtype, (d->dstat?d->dstat:d->numstates), d->initval)) >= 0)
					{}
					break;
				case A_BIT:
					//printf("bit module %d, devid %d, dtext %s, chnnl %d\n", d->modul, d->devid, d->dtext, d->chnnl);
					if ((err=add_obitdevice(&cp, c, m->modul, d->dtext, d->initval)) >= 0)
					{}
					break;
				case A_PULSE:
					//printf("pulse module %d, devid %d, dtext %s, chnnl %d\n", d->modul, d->devid, d->dtext, d->chnnl);
					if ((err=add_opulsedevice(&cp, c, m->modul, d->dtext, d->initval)) >= 0)
					{}
					break;
			}
		}
		cp.nt[i].refresh_button = gtk_button_new_with_label("Refresh");
		if (usegpio)
			g_signal_connect(GTK_BUTTON(cp.nt[i].refresh_button), "clicked", G_CALLBACK(drbutton_clicked_gpio), (void *)&(cp.c->iomodules[i]));
		else
			g_signal_connect(GTK_BUTTON(cp.nt[i].refresh_button), "clicked", G_CALLBACK(drbutton_clicked_http), (void *)&(cp.c->iomodules[i]));
		gtk_box_pack_start(GTK_BOX(cp.nt[i].box), cp.nt[i].refresh_button, TRUE, TRUE, 0);
	}

	if (usegpio)
		ochannel_write(c, TRUE);

	GtkWidget *configbutton;
	configbutton = gtk_button_new_with_label("Configuration");
	g_signal_connect(GTK_BUTTON(configbutton), "clicked", G_CALLBACK(configbutton_clicked), (void *)&cp);
	gtk_container_add(GTK_CONTAINER(cp.container), configbutton);

	gtk_widget_show_all(cp.window);

	//dump_controller(c);


	if (usegpio)
	{
//		init_rule_scheduler(&rs, c);
//		init_state_scheduler(&ss, c);
	}
	else
	{
//		init_rule_scheduler(&rs, NULL);
//		init_state_scheduler(&ss, NULL);
	}


	gtk_main();

//	close_rule_scheduler(&rs);
//	close_state_scheduler(&ss);

	free_modules(&(cp.m));

	controller_close(c);

	return(0);
}
