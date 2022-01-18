#ifndef devH
#define devH

typedef struct
{
	int modul;
	int chnnl;
	int devid;
	device_type dtype;
	int fromval;
	int toval;
}sdev;

typedef struct
{
	int modul;
	int chnnl;
	int devid;
	device_type dtype;
	int value;
}adev;

#endif
