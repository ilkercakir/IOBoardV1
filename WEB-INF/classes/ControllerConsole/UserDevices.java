package ControllerConsole;

import java.util.ArrayList;
import java.util.ResourceBundle;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.Statement;
import java.sql.ResultSet;
import java.sql.DriverManager;
import java.sql.SQLException;

public class UserDevices
{
	private ResourceBundle rb;
	private String DBpath;

	public UserDevices()
	{
		rb = ResourceBundle.getBundle("ControllerConsole.Global");
		DBpath = rb.getString("ControllerConsole.DBPATH");
	}

	private Device newDeviceFromRS(ResultSet rs)
	{
		Device dev = new Device();
		try
		{
			dev.setAuthorizationLevel(rs.getString("level"));
			dev.setDeviceModule(Integer.parseInt(rs.getString("modul")));
			dev.setDeviceID(Integer.parseInt(rs.getString("devid")));
			dev.setDeviceText(rs.getString("dtext"));
			dev.setDeviceChannel(Integer.parseInt(rs.getString("chnnl")));
			dev.setDeviceType(Integer.parseInt(rs.getString("dtype")));
			dev.setDeviceTypeText(rs.getString("dttext"));
			int dstat = Integer.parseInt(rs.getString("dstat"));
			if (dstat==0)
				dev.setDeviceNumStates(Integer.parseInt(rs.getString("numstates")));
			else
				dev.setDeviceNumStates(Integer.parseInt(rs.getString("dstat")));
			dev.setDeviceInitialValue(Integer.parseInt(rs.getString("initval")));
			dev.setDeviceCategory(rs.getString("categ"));
			dev.setDeviceCategoryText(rs.getString("catxt"));
			dev.setDeviceIcon(rs.getString("dicon"));
			dev.setDeviceTypeIcon(rs.getString("dticon"));
		}
		catch(SQLException sqlex)
		{
        		//throw sqlex;	
		}
		return(dev);
	}

	private Module newModuleFromRS(ResultSet rs)
	{
		Module mdl = new Module();
		try
		{
			mdl.setModule(Integer.parseInt(rs.getString("modul")));
			mdl.setModuleText(rs.getString("modtx"));
			mdl.setModuleCategory(rs.getString("categ"));
			mdl.setModuleCategoryText(rs.getString("catxt"));
			mdl.setModuleType(Integer.parseInt(rs.getString("mtype")));
		}
		catch(SQLException sqlex)
		{
        		//throw sqlex;	
		}
		return(mdl);
	}

	public ArrayList<Module> getModulesOfUser(User user)
	{
		ArrayList<Module> modules = new ArrayList<Module>();

		Connection conn = null;
		ResultSet rs = null;
		Statement stat = null;
	
		try
		{
			Class.forName("org.sqlite.JDBC");
		}
		catch (ClassNotFoundException ex)
		{
			//throw ex;
		}

		try
		{
			conn = DriverManager.getConnection("jdbc:sqlite:" + DBpath);
			stat = conn.createStatement();
			if (user.isSuperUser()) // superuser
				rs = stat.executeQuery("select modules.modul, modules.modtx, modules.categ, devcat.catxt, devcat.mtype from modules inner join devcat on devcat.categ = modules.categ order by modules.modul;");
			else
				rs = stat.executeQuery("select modules.modul, modules.modtx, modules.categ, devcat.catxt, devcat.mtype from modules inner join devcat on devcat.categ = modules.categ where exists (select * from authorizations where authorizations.username = '" + user.getUserName() + "' and authorizations.modul = modules.modul) order by modules.modul;");
			while (rs.next()) 
			{
				Module mdl = newModuleFromRS(rs);
				modules.add(mdl);
			}
			rs.close();
			stat.close();
			conn.close();
		}
		catch(SQLException sqlex)
		{
        		//throw sqlex;	
		}
		return(modules);
	}

	public ArrayList<Device> getDevicesOfUser(User user)
	{
		ArrayList<Device> devices = new ArrayList<Device>();

		Connection conn = null;
		ResultSet rs = null;
		Statement stat = null;
	
		try
		{
			Class.forName("org.sqlite.JDBC");
		}
		catch (ClassNotFoundException ex)
		{
			//throw ex;
		}

		try
		{
			conn = DriverManager.getConnection("jdbc:sqlite:" + DBpath);
			stat = conn.createStatement();
			if (user.isSuperUser()) // superuser
				rs = stat.executeQuery("select 'W' as level, devices.modul, devices.devid, devices.dtext, devices.chnnl, devices.dtype, devicetypes.dtext as dttext, devices.numstates, devices.initval, devicetypes.dstat, devicetypes.categ, devcat.catxt, devices.dicon, devicetypes.dicon as dticon from devices inner join devicetypes on devicetypes.dtype = devices.dtype inner join devcat on devcat.categ = devicetypes.categ order by devices.modul, devices.chnnl;");
			else
				rs = stat.executeQuery("select authorizations.level, devices.modul, devices.devid, devices.dtext, devices.chnnl, devices.dtype, devicetypes.dtext as dttext, devices.numstates, devices.initval, devicetypes.dstat, devicetypes.categ, devcat.catxt, devices.dicon, devicetypes.dicon as dticon from devices inner join devicetypes on devicetypes.dtype = devices.dtype inner join devcat on devcat.categ = devicetypes.categ inner join authorizations on authorizations.modul = devices.modul and authorizations.devid = devices.devid inner join users on users.username = authorizations.username where users.username = '" + user.getUserName() + "' order by devices.modul, devices.chnnl;");
			while (rs.next()) 
			{
				Device dev = newDeviceFromRS(rs);
				devices.add(dev);
			}
			rs.close();
			stat.close();
			conn.close();
		}
		catch(SQLException sqlex)
		{
        		//throw sqlex;	
		}
		return(devices);
	}

	public ArrayList<Module> getModules()
	{
		ArrayList<Module> modules = new ArrayList<Module>();

		Connection conn = null;
		ResultSet rs = null;
		Statement stat = null;
	
		try
		{
			Class.forName("org.sqlite.JDBC");
		}
		catch (ClassNotFoundException ex)
		{
			//throw ex;
		}

		try
		{
			conn = DriverManager.getConnection("jdbc:sqlite:" + DBpath);
			stat = conn.createStatement();
			rs = stat.executeQuery("select modules.modul, modules.modtx, modules.categ, devcat.catxt, devcat.mtype from modules inner join devcat on devcat.categ = modules.categ order by modules.modul;");
			while (rs.next()) 
			{
				Module mdl = newModuleFromRS(rs);
				modules.add(mdl);
			}
			rs.close();
			stat.close();
			conn.close();
		}
		catch(SQLException sqlex)
		{
        		//throw sqlex;	
		}
		return(modules);
	}

	public ArrayList<Device> getDevices()
	{
		ArrayList<Device> devices = new ArrayList<Device>();

		Connection conn = null;
		ResultSet rs = null;
		Statement stat = null;
	
		try
		{
			Class.forName("org.sqlite.JDBC");
		}
		catch (ClassNotFoundException ex)
		{
			//throw ex;
		}

		try
		{
			conn = DriverManager.getConnection("jdbc:sqlite:" + DBpath);
			stat = conn.createStatement();
			rs = stat.executeQuery("select 'W' as level, devices.modul, devices.devid, devices.dtext, devices.chnnl, devices.dtype, devicetypes.dtext as dttext, devices.numstates, devices.initval, devicetypes.dstat, devicetypes.categ, devcat.catxt, devices.dicon, devicetypes.dicon as dticon from devices inner join devicetypes on devicetypes.dtype = devices.dtype inner join devcat on devcat.categ = devicetypes.categ order by devices.modul, devices.chnnl;");
			while (rs.next()) 
			{
				Device dev = newDeviceFromRS(rs);
				devices.add(dev);
			}
			rs.close();
			stat.close();
			conn.close();
		}
		catch(SQLException sqlex)
		{
        		//throw sqlex;	
		}
		return(devices);
	}
}