<%@ page import ="java.util.*" %>
<%@ page import ="ControllerConsole.*" %>
<%@ page language="java" contentType="text/html; charset=ISO-8859-9" pageEncoding="utf-8" %>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-9" />
<link rel="stylesheet" type="text/css" href="css/slidebutton.css">
<link rel="stylesheet" type="text/css" href="css/tabstrip.css">
</head>
<jsp:useBean id="user" class="ControllerConsole.User" scope="session"/>
<jsp:useBean id="module" class="ControllerConsole.Module" scope="session"/>
<jsp:useBean id="devices" class="ControllerConsole.UserDevices" scope="page"/>
<jsp:useBean id="controller" class="ControllerConsole.Controller" scope="application"/>
<body>
<%
byte selectedvalue;
int i, m, ischecked;
if (user.isLoggedIn())
{
%>
<div class="css3-tabstrip">
    <ul>
<%
	ArrayList<Device> userDevices = devices.getDevicesOfUser(user);
        Iterator<ControllerConsole.Module> moduleIter = devices.getModulesOfUser(user).iterator();
	m = 0;
	ischecked = 0;
  
        while (moduleIter.hasNext())
	{
		ControllerConsole.Module mdl = moduleIter.next();
%>
	<li>
		<input type="radio" name="css3-tabstrip-0" <%if (ischecked==0) {%>checked="checked"<%}%> id="css3-tabstrip-0-<%=m%>" /><label for="css3-tabstrip-0-<%=m%>"><%=mdl.getModuleText()%></label>
            	<div>
		<table align="center" width="90%">
<%
		Iterator<Device> channelIter = userDevices.iterator();
		while (channelIter.hasNext())
		{
			Device dev = channelIter.next();
			if (dev.getDeviceModule()!=mdl.getModule()) continue;
%>
 <tr>
  <td align="right"><img width="128px" height="128px" src="images/<%=dev.getDeviceIcon()%>"
<%
switch(dev.getDeviceType())
{
	case 5: // S_SWITCH
%> style="cursor:pointer" onclick="javascript:sensorClick(<%=dev.getDeviceModule()%>, <%=dev.getDeviceChannel()%>, <%=dev.getDeviceID()%>, <%=dev.getDeviceType()%>)"
<%
	break;
	case 6: // S_LEVEL
%> style="cursor:pointer" onclick="javascript:sensorClick(<%=dev.getDeviceModule()%>, <%=dev.getDeviceChannel()%>, <%=dev.getDeviceID()%>, <%=dev.getDeviceType()%>)"
<%
	break;
}
%>
></td>
  <td><font style="font-name:Arial;font-size:24pt;width:256px;height:128px;"><b><%=dev.getDeviceText()%></b><br><i><%=dev.getDeviceCategoryText()%>, <%=dev.getDeviceTypeText()%></i></font></td>
  <%selectedvalue=0;%>
  <td>
<%
switch(dev.getDeviceType())
{
	case 1: // A_SWITCH
		selectedvalue = controller.getChannelValue(dev.getDeviceModule(), dev.getDeviceChannel());
%>
<label class="switch">
  <input type="checkbox" <%if (!dev.getAuthorizationLevel().equals("W")){%>disabled<%}%> id="dev_<%=dev.getDeviceModule()%>_<%=dev.getDeviceID()%>" <%if (selectedvalue!=0){%>checked<%}%> onchange="javascript:channelClick(<%=dev.getDeviceModule()%>, <%=dev.getDeviceChannel()%>, <%=dev.getDeviceID()%>, <%=dev.getDeviceType()%>)">
  <span class="slider round"></span>
</label> 
<%
		break;
	case 2: // A_LEVEL
		selectedvalue = controller.getChannelValue(dev.getDeviceModule(), dev.getDeviceChannel());
%>
<select style="font-name:Arial;font-size:64pt;width:200px;height:128px;" <%if (!dev.getAuthorizationLevel().equals("W")){%>disabled<%}%> id="dev_<%=dev.getDeviceModule()%>_<%=dev.getDeviceID()%>" onchange="javascript:channelClick(<%=dev.getDeviceModule()%>, <%=dev.getDeviceChannel()%>, <%=dev.getDeviceID()%>, <%=dev.getDeviceType()%>)"><%for (i=0;i<dev.getDeviceNumStates();i++){%><option value="<%=i%>"<%if (i==selectedvalue){%> selected<%}%>><%=i%></option><%}%></select>
<%
		break;
	case 3: // A_PULSE
%>
<img width="128px" height="128px" id="dev_<%=dev.getDeviceModule()%>_<%=dev.getDeviceID()%>" src="images/<%=dev.getDeviceTypeIcon()%>" <%if (!dev.getAuthorizationLevel().equals("W")){%>disabled<%}else{%>style="cursor:pointer"<%}%> onclick="javascript:pulseClick(<%=dev.getDeviceModule()%>, <%=dev.getDeviceChannel()%>, <%=dev.getDeviceID()%>, <%=dev.getDeviceType()%>, <%=dev.getDeviceInitialValue()%>)"></td>
<%
		break;
	case 4: // A_BUTTON
%>
<img width="128px" height="128px" id="dev_<%=dev.getDeviceModule()%>_<%=dev.getDeviceID()%>" src="images/<%=dev.getDeviceTypeIcon()%>" <%if (!dev.getAuthorizationLevel().equals("W")){%>disabled<%}else{%>style="cursor:pointer"<%}%> onclick="javascript:channelPulseClick(<%=dev.getDeviceModule()%>, <%=dev.getDeviceChannel()%>, <%=dev.getDeviceID()%>, <%=dev.getDeviceType()%>)"/>
<%
		break;
	case 5: // S_SWITCH
		selectedvalue = controller.getChannelValue(dev.getDeviceModule(), dev.getDeviceChannel());
%>
<label class="switch">
  <input type="checkbox" disabled id="dev_<%=dev.getDeviceModule()%>_<%=dev.getDeviceID()%>" <%if (selectedvalue!=0){%>checked<%}%> onclick="javascript:sensorClick(<%=dev.getDeviceModule()%>, <%=dev.getDeviceChannel()%>, <%=dev.getDeviceID()%>, <%=dev.getDeviceType()%>)">
  <span class="slider round"></span>
</label> 
<%
		break;
	case 6: // S_LEVEL
		selectedvalue = controller.getChannelValue(dev.getDeviceModule(), dev.getDeviceChannel());
%>
<select style="font-name:Arial;font-size:64pt;width:200px;height:128px;" disabled id="dev_<%=dev.getDeviceModule()%>_<%=dev.getDeviceID()%>" onclick="javascript:sensorClick(<%=dev.getDeviceModule()%>, <%=dev.getDeviceChannel()%>, <%=dev.getDeviceID()%>, <%=dev.getDeviceType()%>)"><%for (i=0;i<dev.getDeviceNumStates();i++){%><option value="<%=i%>"<%if (i==selectedvalue){%> selected<%}%>><%=i%></option><%}%></select>
<%
		break;
	case 7: // A_BIT
		selectedvalue = controller.getBitValue(dev.getDeviceChannel());
%>
<label class="switch">
  <input type="checkbox" <%if (!dev.getAuthorizationLevel().equals("W")){%>disabled<%}%> id="dev_<%=dev.getDeviceModule()%>_<%=dev.getDeviceID()%>" <%if (selectedvalue!=0){%>checked<%}%> onchange="javascript:bitClick(<%=dev.getDeviceModule()%>, <%=dev.getDeviceChannel()%>, <%=dev.getDeviceID()%>, <%=dev.getDeviceType()%>)">
  <span class="slider round"></span>
</label> 
<%
		break;
}
%>
  </td>
  <td></td>
 </tr>
<%
		}
			
%>            
            </table>
            </div>
        </li>
<%
		m++;
		ischecked = 1;
	}
}
else
{
%>
Not logged in
<%
}
%>
    </ul>
</div>

</body>
</html>

<script type="text/javascript">
function channelCallback(responseText)
{
 	var obj = JSON.parse(responseText);
	document.getElementById('dev_' + obj.modul + '_' + obj.devid).value = obj.value;
}

function channelClick(moduleid, channelid, deviceid, devicetype)
{
	var value;
	var channelobj = eval('document.getElementById("dev_' + moduleid + '_' + deviceid + '");');
	switch (devicetype)
	{
		case 1: // A_SWITCH
			if (channelobj.checked)
				value = 1;
			else
				value = 0;
			break;
		case 2: // A_LEVEL
			value = channelobj.value;
			break;
		default:
			return;
	}

 	var xmlHttp = new XMLHttpRequest();
	xmlHttp.onreadystatechange = function() { 
 		if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
			channelCallback(xmlHttp.responseText);
	}
	var url = 'ControllerConsole?channel&modul=' + moduleid + '&id=' + channelid + '&devid=' + deviceid + '&value=' + value;
 	xmlHttp.open('GET', url, true); // asynchronous 
 	xmlHttp.send(null);
}

function bitCallback(responseText)
{
 	var obj = JSON.parse(responseText);
	document.getElementById('dev_' + obj.modul + '_' + obj.devid).value = obj.value;
}

function bitClick(moduleid, channelid, deviceid, devicetype)
{
	var value;
	var bitobj = eval('document.getElementById("dev_' + moduleid + '_' + deviceid + '");');

	if (bitobj.checked)
		value = 1;
	else
		value = 0;

 	var xmlHttp = new XMLHttpRequest();
	xmlHttp.onreadystatechange = function() { 
 		if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
			bitCallback(xmlHttp.responseText);
	}
	var url = 'ControllerConsole?bit&modul=' + moduleid + '&id=' + channelid + '&devid=' + deviceid + '&value=' + value;

 	xmlHttp.open('GET', url, true); // asynchronous 
 	xmlHttp.send(null);
}

function pulseCallback(responseText)
{
 	var obj = JSON.parse(responseText);
	//document.getElementById('dev_' + obj.modul + '_' + obj.devid).value = obj.value;
}

function pulseClick(moduleid, channelid, deviceid, devicetype, initval)
{
	var pulseobj = eval('document.getElementById("dev_' + moduleid + '_' + deviceid + '");');
	var value = initval;

 	var xmlHttp = new XMLHttpRequest();
	xmlHttp.onreadystatechange = function() { 
 		if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
			pulseCallback(xmlHttp.responseText);
	}
	var url = 'ControllerConsole?pulse&modul=' + moduleid + '&id=' + channelid + '&devid=' + deviceid + '&value=' + value;
 	xmlHttp.open('GET', url, true); // asynchronous 
 	xmlHttp.send(null);
}

function channelPulseCallback(responseText)
{
 	var obj = JSON.parse(responseText);
	//document.getElementById('dev_' + obj.modul + '_' + obj.devid).value = obj.value;
}

function channelPulseClick(moduleid, channelid, deviceid, devicetype)
{
	var pulseobj = eval('document.getElementById("dev_' + moduleid + '_' + deviceid + '");');

 	var xmlHttp = new XMLHttpRequest();
	xmlHttp.onreadystatechange = function() { 
 		if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
			channelPulseCallback(xmlHttp.responseText);
	}
	var url = 'ControllerConsole?channelpulse&modul=' + moduleid + '&id=' + channelid + '&devid=' + deviceid;
 	xmlHttp.open('GET', url, true); // asynchronous 
 	xmlHttp.send(null);
}

function sensorCallback(responseText)
{
 	var obj = JSON.parse(responseText);
	document.getElementById('dev_' + obj.modul + '_' + obj.devid).value = obj.value;
//alert(obj.value);
}

function sensorClick(moduleid, channelid, deviceid, devicetype)
{
 	var xmlHttp = new XMLHttpRequest();
	xmlHttp.onreadystatechange = function() { 
 		if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
			sensorCallback(xmlHttp.responseText);
	}
	var url = 'ControllerConsole?readchannel&modul=' + moduleid + '&id=' + channelid + '&devid=' + deviceid;
 	xmlHttp.open('GET', url, true); // asynchronous 
 	xmlHttp.send(null);
}
</script>