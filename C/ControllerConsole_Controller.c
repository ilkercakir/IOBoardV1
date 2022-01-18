/*
 * ControllerConsole_Controller.c
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

// compile with gcc -Wall -c "%f" -DIS_RPI -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -g -ftree-vectorize -pipe -DUSE_VCHIQ_ARM -Wno-psabi -mcpu=cortex-a53 -mfloat-abi=hard -mfpu=neon-fp-armv8 -mneon-for-64bits -Wno-deprecated-declarations -I/usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/include -I/usr/lib/jvm/jdk-8-oracle-arm32-vfp-hflt/include/linux

#include <stdio.h>
#include "iochannels.h"
#include "ControllerConsole_Controller.h"

JNIEXPORT jlong JNICALL Java_ControllerConsole_Controller_controller_1open(JNIEnv *env, jobject obj, jint version, jint useGPIO)
{
	controller *c;

	c = controller_open((controller_type)version, (int)useGPIO);

	return((jlong)(long)c);
}

JNIEXPORT void JNICALL Java_ControllerConsole_Controller_controller_1close(JNIEnv *env, jobject obj, jlong handle)
{
	controller *c = (controller *)(long)handle;

	controller_close(c);
}

JNIEXPORT jbyte JNICALL Java_ControllerConsole_Controller_controller_1get_1value(JNIEnv *env, jobject obj, jlong handle, jint modul, jint locked)
{
	controller *c = (controller *)(long)handle;
	unsigned char value;

	value = controller_get_value(c, (unsigned int)modul, (int)locked);

	return((jbyte)value);
}

JNIEXPORT void JNICALL Java_ControllerConsole_Controller_controller_1set_1value(JNIEnv *env, jobject obj, jlong handle, jint modul, jbyte value)
{
	controller *c = (controller *)(long)handle;

	controller_set_value(c, (unsigned int)modul, (unsigned char)value);
}
 
JNIEXPORT jint JNICALL Java_ControllerConsole_Controller_module_1add(JNIEnv *env, jobject obj, jlong handle, jstring jname, jint mtype)
{
	controller *c = (controller *)(long)handle;
	jboolean iscopy; 
	const char *name;
	int id;

	name = (*env)->GetStringUTFChars(env, jname, &iscopy); 

	id = module_add(c, (char *)name, (module_type)mtype);
	return((jint)id);
}

JNIEXPORT jint JNICALL Java_ControllerConsole_Controller_iochannel_1add(JNIEnv *env, jobject obj, jlong handle, jint modul, jstring jname, jint type, jint numstates)
{
	controller *c = (controller *)(long)handle;
	jboolean iscopy; 
	const char *name;
	int id;

	name = (*env)->GetStringUTFChars(env, jname, &iscopy); 
	id = iochannel_add(c, (unsigned int)modul, (char *)name, (device_type)type, (int)numstates);

	return((jint)id);
}

JNIEXPORT jint JNICALL Java_ControllerConsole_Controller_obit_1add(JNIEnv *env, jobject obj, jlong handle, jint modul, jstring jname)
{
	controller *c = (controller *)(long)handle;
	jboolean iscopy; 
	const char *name;
	int id;

	name = (*env)->GetStringUTFChars(env, jname, &iscopy); 
	id = obit_add(c, (unsigned int)modul, (char *)name);

	return((jint)id);
}

JNIEXPORT jint JNICALL Java_ControllerConsole_Controller_opulse_1add(JNIEnv *env, jobject obj, jlong handle, jint modul, jstring jname)
{
	controller *c = (controller *)(long)handle;
	jboolean iscopy; 
	const char *name;
	int id;

	name = (*env)->GetStringUTFChars(env, jname, &iscopy); 
	id = opulse_add(c, (unsigned int)modul, (char *)name);

	return((jint)id);
}

JNIEXPORT jbyte JNICALL Java_ControllerConsole_Controller_iochannel_1get_1value(JNIEnv *env, jobject obj, jlong handle, jint modul, jint channel)
{
	controller *c = (controller *)(long)handle;
	unsigned char value;

	value = iochannel_get_value(c, (unsigned int)modul, (unsigned int)channel);

	return((jbyte)value);
}

JNIEXPORT void JNICALL Java_ControllerConsole_Controller_iochannel_1set_1value(JNIEnv *env, jobject obj, jlong handle, jint modul, jint channel, jbyte value)
{
	controller *c = (controller *)(long)handle;

	iochannel_set_value(c, (unsigned int)modul, (unsigned int)channel, (unsigned char)value);
}

JNIEXPORT jint JNICALL Java_ControllerConsole_Controller_iochannel_1pulse(JNIEnv *env, jobject obj, jlong handle, jint modul, jint channel)
{
	controller *c = (controller *)(long)handle;
	int id;

	id = iochannel_pulse(c, (unsigned int)modul, (unsigned int)channel);

	return((jint)id);
}

JNIEXPORT void JNICALL Java_ControllerConsole_Controller_ochannel_1write(JNIEnv *env, jobject obj, jlong handle, jint locked)
{
	controller *c = (controller *)(long)handle;

	ochannel_write(c, (int)locked);
}

JNIEXPORT jbyte JNICALL Java_ControllerConsole_Controller_obit_1get_1value(JNIEnv *env, jobject obj, jlong handle, jint bit)
{
	controller *c = (controller *)(long)handle;
	unsigned char value;

	value = obit_get_value(c, (unsigned int)bit);

	return((jbyte)value);
}

JNIEXPORT void JNICALL Java_ControllerConsole_Controller_obit_1set_1value(JNIEnv *env, jobject obj, jlong handle, jint bit, jbyte value)
{
	controller *c = (controller *)(long)handle;

	obit_set_value(c, (unsigned int)bit, (unsigned char)value);
}

JNIEXPORT jbyte JNICALL Java_ControllerConsole_Controller_opulse_1get_1value(JNIEnv *env, jobject obj, jlong handle, jint pulse)
{
	controller *c = (controller *)(long)handle;
	unsigned char value;

	value = opulse_get_value(c, (unsigned int)pulse);

	return((jbyte)value);
}

JNIEXPORT void JNICALL Java_ControllerConsole_Controller_opulse_1set_1value(JNIEnv *env, jobject obj, jlong handle, jint pulse, jbyte value)
{
	controller *c = (controller *)(long)handle;

	opulse_set_value(c, (unsigned int)pulse, (unsigned char)value);
}

JNIEXPORT void JNICALL Java_ControllerConsole_Controller_opulse_1out(JNIEnv *env, jobject obj, jlong handle, jint pulse, jint usecs)
{
	controller *c = (controller *)(long)handle;

	opulse_out(c, (unsigned int)pulse, (unsigned int)usecs);
}

JNIEXPORT jbyte JNICALL Java_ControllerConsole_Controller_ichannel_1read(JNIEnv *env, jobject obj, jlong handle)
{
	controller *c = (controller *)(long)handle;
	unsigned char value;

	value = ichannel_read(c);

	return((jbyte)value);
}
