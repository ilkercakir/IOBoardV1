package ControllerConsole;

import java.util.ArrayList;
import java.util.Iterator;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.LogManager;

public class Controller
{
	private long controllerHandle;
	private String libraryPath;
	static Logger logger = LogManager.getLogger(ControllerConsole.Controller.class);

	private native long controller_open(int version, int useGPIO);
	private native void controller_close(long handle);
	private native byte controller_get_value(long handle, int module, int locked);
	private native void controller_set_value(long handle, int module, byte value);
	private native int module_add(long handle, String name, int mtype);
	private native int iochannel_add(long handle, int module, String name, int type, int numstates);
	private native int obit_add(long handle, int module, String name);
	private native int opulse_add(long handle, int module, String name);
	private native byte iochannel_get_value(long handle, int module, int channel);
	private native void iochannel_set_value(long handle, int module, int channel, byte value);
	private native int iochannel_pulse(long handle, int module, int channel);
	private native void ochannel_write(long handle, int locked);
	private native byte obit_get_value(long handle, int bit);
	private native void obit_set_value(long handle, int bit, byte value);
	private native byte opulse_get_value(long handle, int pulse);
	private native void opulse_set_value(long handle, int pulse, byte value);
	private native void opulse_out(long handle, int pulse, int usecs);
	private native byte ichannel_read(long handle);

	public Controller()
	{
		controllerHandle = 0;

		logger.debug("init debug messages");
		logger.info("init info messages");
		logger.warn("init warn messages");
		logger.error("init error messages");
		logger.fatal("init fatal messages");

		try
		{
			libraryPath = System.getProperty("java.library.path");
			logger.info("Controller java.library.path = " + libraryPath);
			System.loadLibrary("IOBoardV1");
			logger.info("Native code library loaded");
		}
		catch (UnsatisfiedLinkError e)
		{
      			logger.error("Native code library failed to load" + e);
		}
	}

	public int Open()
	{
		Device dev;
		int mid, id;

		controllerHandle = controller_open(1, 1); // Version 1, use GPIO
		logger.info("controller_open() handle " + controllerHandle);

		UserDevices udev = new UserDevices();
		ArrayList<Device> allDevices = udev.getDevices();

		Iterator<Module> moduleIter = udev.getModules().iterator();
        	while (moduleIter.hasNext())
		{
			Module mdl = moduleIter.next();
			mid = module_add(controllerHandle, mdl.getModuleText(), mdl.getModuleType());
			logger.info("module_add " + mdl.getModuleText() + " ID:" + mdl.getModule());
			if (mid == mdl.getModule())
			{
				logger.info("module_add " + mid + " db:" + mdl.getModule());
			}
			else
			{
				logger.error("module_add " + mid + " failed");
				return(-1); // channels init failed
			}

			Iterator<Device> channelIter = allDevices.iterator();
	        	while (channelIter.hasNext())
			{
				dev = channelIter.next();
				if (dev.getDeviceModule() != mdl.getModule()) continue;
				switch (dev.getDeviceType())
				{
					case 1: // A_SWITCH
					case 2: // A_LEVEL
					case 4: // A_BUTTON
					case 5: // S_SWITCH
					case 6: // S_LEVEL
						id = iochannel_add(controllerHandle, mdl.getModule(), dev.getDeviceText(), dev.getDeviceType(), dev.getDeviceNumStates());
						logger.info("iochannel_add " + dev.getDeviceText() + " channel:" + id);
						if (id == dev.getDeviceChannel())
						{
							iochannel_set_value(controllerHandle, mdl.getModule(), dev.getDeviceChannel(), (byte)dev.getDeviceInitialValue());
							logger.info("iochannel_set_value " + mdl.getModule() + "/" + dev.getDeviceID() + ", initial value " + (byte)dev.getDeviceInitialValue());
						}
						else
						{
							logger.error("iochannel_add " + mdl.getModule() + "/" + dev.getDeviceID() + " failed");
							return(-1); // channels init failed
						}
						break;
					case 3: // A_PULSE
						id = opulse_add(controllerHandle, mdl.getModule(), dev.getDeviceText());
						logger.info("opulse_add " + dev.getDeviceText() + " channel:" + id);
						if (id == dev.getDeviceChannel())
						{
							opulse_set_value(controllerHandle, dev.getDeviceChannel(), (byte)dev.getDeviceInitialValue());
							logger.info("opulse_set_value " + dev.getDeviceChannel() + ", initial value " + (byte)dev.getDeviceInitialValue());
						}
						else
						{
							logger.error("opulse_add " + dev.getDeviceID() + " failed");
							return(-3); // pulses init failed
						}
						break;
					case 7: // A_BIT
						id = obit_add(controllerHandle, mdl.getModule(), dev.getDeviceText());
						logger.info("obit_add " + dev.getDeviceText() + " channel:" + id);
						if (id == dev.getDeviceChannel())
						{
							obit_set_value(controllerHandle, dev.getDeviceChannel(), (byte)dev.getDeviceInitialValue());
							logger.info("obit_set_value " + dev.getDeviceChannel() + ", initial value " + (byte)dev.getDeviceInitialValue());
						}
						else
						{
							logger.error("obit_add " + dev.getDeviceID() + " failed");
							return(-2); // bits init failed
						}
						break;

				}
			}
		}
		ochannel_write(controllerHandle, 1);

		logger.info("ichannel_read() = " + ichannel_read(controllerHandle));

		logger.info("controller Open() completed");
		return(0);
	}

	public byte getChannelValue(int module, int channel)
	{
		return(iochannel_get_value(controllerHandle, module, channel));
	}

	public void setChannelValue(int module, int channel, byte value)
	{
		iochannel_set_value(controllerHandle, module, channel, value);
	}

	public void channelPulse(int module, int channel)
	{
		iochannel_pulse(controllerHandle, module, channel);
	}

	public byte getBitValue(int bit)
	{
		return(obit_get_value(controllerHandle, bit));
	}

	public void setBitValue(int bit, byte value)
	{
		obit_set_value(controllerHandle, bit, value);
	}

	public byte getPulseValue(int pulse)
	{
		return(opulse_get_value(controllerHandle, pulse));
	}

	public void setPulseValue(int pulse, byte value)
	{
		opulse_set_value(controllerHandle, pulse, value);
	}
	
	public void pulseOut(int pulse, int usecs)
	{
		opulse_out(controllerHandle, pulse, usecs);
	}

	public void writeChannel()
	{
		ochannel_write(controllerHandle, 1);
	}

	public byte readChannel()
	{
		return(ichannel_read(controllerHandle));
	}

	public void Close()
	{
		Device dev;

		UserDevices udev = new UserDevices();
		Iterator<Device> channelIter = udev.getDevices().iterator();
        	while (channelIter.hasNext())
		{
			dev = channelIter.next();
			switch (dev.getDeviceType())
			{
				case 1: // A_SWITCH
				case 2: // A_LEVEL
				case 4: // A_BUTTON
				case 5: // S_SWITCH
				case 6: // S_LEVEL
					iochannel_set_value(controllerHandle, dev.getDeviceModule(), dev.getDeviceChannel(), (byte)dev.getDeviceInitialValue());
					logger.info("iochannel_set_value " + dev.getDeviceModule() + "/" + dev.getDeviceID() + ", initial value " + (byte)dev.getDeviceInitialValue());
					break;
				case 3: // A_PULSE
					opulse_set_value(controllerHandle, dev.getDeviceChannel(), (byte)dev.getDeviceInitialValue());
					logger.info("opulse_set_value " + dev.getDeviceChannel() + ", initial value " + (byte)dev.getDeviceInitialValue());
					break;
				case 7: // A_BIT
					obit_set_value(controllerHandle, dev.getDeviceChannel(), (byte)dev.getDeviceInitialValue());
					logger.info("obit_set_value " + dev.getDeviceChannel() + ", initial value " + (byte)dev.getDeviceInitialValue());
					break;
			}
		}

		controller_close(controllerHandle);
		controllerHandle = 0;

		logger.info("controller Close() completed");
	}
}