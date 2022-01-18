cd /var/lib/tomcat9/webapps/IOBoardV1/WEB-INF/classes
sudo javac -cp . ControllerConsole/Device.java
sudo javac -cp . ControllerConsole/Module.java
sudo javac -cp ".:/usr/share/tomcat9/lib/sqlite-jdbc-3.7.2.jar" ControllerConsole/User.java
sudo javac -cp ".:/usr/share/tomcat9/lib/sqlite-jdbc-3.7.2.jar" ControllerConsole/UserDevices.java
sudo javac -cp ".:/usr/share/tomcat9/lib/log4j-api-2.13.0.jar:/usr/share/tomcat9/lib/log4j-core-2.13.0.jar" ControllerConsole/Controller.java
sudo javac -cp ".:/usr/share/tomcat9/lib/servlet-api.jar:/usr/share/tomcat9/lib/log4j-api-2.13.0.jar:/usr/share/tomcat9/lib/log4j-core-2.13.0.jar" ControllerConsole/ConsoleServlet.java
sudo javah -jni -cp . ControllerConsole.Controller
mv ControllerConsole_Controller.h /home/pi/Desktop/C/GPIO/IOBoardV1
make -j4 all

cd /home/pi/Desktop/C/GPIO/IOBoardV1
gcc -shared -o libIOBoardV1.so ControllerConsole_Controller.o iochannels.o gpio.o /usr/lib/libwiringPi.so
/cd /usr/lib
/sudo ln -s /home/pi/Desktop/C/GPIO/IOBoardV1/libIOBoardV1.so libIOBoardV1.so
/cd /home/pi/Desktop/C/GPIO/IOBoardV1