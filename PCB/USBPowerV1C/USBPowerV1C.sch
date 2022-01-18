EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:kicad_custom_lib
LIBS:USBPowerV1C-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "noname.sch"
Date "13 aug 2021"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L KLEMENS_1 P1
U 1 1 60E943C9
P 1300 1050
F 0 "P1" V 1250 1050 40  0000 C CNN
F 1 "KLEMENS_1" V 1350 1050 40  0000 C CNN
F 2 "~" H 1300 1050 60  0000 C CNN
F 3 "~" H 1300 1050 60  0000 C CNN
	1    1300 1050
	1    0    0    -1  
$EndComp
$Comp
L BARREL_JACK CON1
U 1 1 610E143B
P 1150 1600
F 0 "CON1" H 1150 1850 60  0000 C CNN
F 1 "BARREL_JACK" H 1150 1400 60  0000 C CNN
F 2 "" H 1150 1600 60  0000 C CNN
F 3 "" H 1150 1600 60  0000 C CNN
	1    1150 1600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 610E1903
P 950 1250
F 0 "#PWR01" H 950 1250 30  0001 C CNN
F 1 "GND" H 950 1180 30  0001 C CNN
F 2 "" H 950 1250 60  0000 C CNN
F 3 "" H 950 1250 60  0000 C CNN
	1    950  1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  1250 950  1150
$Comp
L VCC #PWR02
U 1 1 610E1988
P 950 850
F 0 "#PWR02" H 950 950 30  0001 C CNN
F 1 "VCC" H 950 950 30  0000 C CNN
F 2 "" H 950 850 60  0000 C CNN
F 3 "" H 950 850 60  0000 C CNN
	1    950  850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  950  950  850 
Wire Wire Line
	1450 1500 1500 1500
Wire Wire Line
	1500 600  1500 2300
Connection ~ 1500 950 
Wire Wire Line
	1450 1700 1550 1700
Wire Wire Line
	1550 700  1550 2900
Wire Wire Line
	850  2300 1650 2300
Connection ~ 1500 1500
Wire Wire Line
	1550 2900 850  2900
Connection ~ 1550 1700
$Comp
L LED D1
U 1 1 610E1C25
P 2350 2550
F 0 "D1" H 2350 2650 50  0000 C CNN
F 1 "LED" H 2350 2450 50  0000 C CNN
F 2 "~" H 2350 2550 60  0000 C CNN
F 3 "~" H 2350 2550 60  0000 C CNN
	1    2350 2550
	0    1    1    0   
$EndComp
$Comp
L R R1
U 1 1 610E1C34
P 2550 2600
F 0 "R1" V 2630 2600 40  0000 C CNN
F 1 "10K" V 2557 2601 40  0000 C CNN
F 2 "~" V 2480 2600 30  0000 C CNN
F 3 "~" H 2550 2600 30  0000 C CNN
	1    2550 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 2300 1650 2350
Wire Wire Line
	1650 2350 2350 2350
Connection ~ 1500 2300
Wire Wire Line
	2350 2750 2450 2750
Wire Wire Line
	2450 2750 2450 2650
Wire Wire Line
	2450 2650 2550 2650
Wire Wire Line
	2550 2650 2550 2350
Wire Wire Line
	2900 2850 1550 2850
Connection ~ 1550 2850
$Comp
L CONN_2 P6
U 1 1 610E1F63
P 3250 2550
F 0 "P6" V 3200 2550 40  0000 C CNN
F 1 "GND" V 3300 2550 40  0000 C CNN
F 2 "" H 3250 2550 60  0000 C CNN
F 3 "" H 3250 2550 60  0000 C CNN
	1    3250 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 2450 2900 2850
Connection ~ 2550 2850
Connection ~ 2900 2650
Wire Wire Line
	1500 950  950  950 
Wire Wire Line
	950  1150 1550 1150
$Comp
L RJ45_2 J1
U 1 1 610EADFD
P 2300 950
F 0 "J1" H 2500 1450 60  0000 C CNN
F 1 "RJ45_2" H 2150 1450 60  0000 C CNN
F 2 "~" H 2300 950 60  0000 C CNN
F 3 "~" H 2300 950 60  0000 C CNN
	1    2300 950 
	0    1    1    0   
$EndComp
$Comp
L RJ45_2 J5
U 1 1 610EAE66
P 2300 1800
F 0 "J5" H 2500 2300 60  0000 C CNN
F 1 "RJ45_2" H 2150 2300 60  0000 C CNN
F 2 "~" H 2300 1800 60  0000 C CNN
F 3 "~" H 2300 1800 60  0000 C CNN
	1    2300 1800
	0    1    1    0   
$EndComp
$Comp
L RJ45_2 J2
U 1 1 610EAEF1
P 3350 950
F 0 "J2" H 3550 1450 60  0000 C CNN
F 1 "RJ45_2" H 3200 1450 60  0000 C CNN
F 2 "~" H 3350 950 60  0000 C CNN
F 3 "~" H 3350 950 60  0000 C CNN
	1    3350 950 
	0    1    1    0   
$EndComp
$Comp
L RJ45_2 J6
U 1 1 610EAEF8
P 3350 1800
F 0 "J6" H 3550 2300 60  0000 C CNN
F 1 "RJ45_2" H 3200 2300 60  0000 C CNN
F 2 "~" H 3350 1800 60  0000 C CNN
F 3 "~" H 3350 1800 60  0000 C CNN
	1    3350 1800
	0    1    1    0   
$EndComp
$Comp
L RJ45_2 J3
U 1 1 610EAF28
P 4500 950
F 0 "J3" H 4700 1450 60  0000 C CNN
F 1 "RJ45_2" H 4350 1450 60  0000 C CNN
F 2 "~" H 4500 950 60  0000 C CNN
F 3 "~" H 4500 950 60  0000 C CNN
	1    4500 950 
	0    1    1    0   
$EndComp
$Comp
L RJ45_2 J7
U 1 1 610EAF2E
P 4500 1800
F 0 "J7" H 4700 2300 60  0000 C CNN
F 1 "RJ45_2" H 4350 2300 60  0000 C CNN
F 2 "~" H 4500 1800 60  0000 C CNN
F 3 "~" H 4500 1800 60  0000 C CNN
	1    4500 1800
	0    1    1    0   
$EndComp
$Comp
L RJ45_2 J4
U 1 1 610EAF34
P 5550 950
F 0 "J4" H 5750 1450 60  0000 C CNN
F 1 "RJ45_2" H 5400 1450 60  0000 C CNN
F 2 "~" H 5550 950 60  0000 C CNN
F 3 "~" H 5550 950 60  0000 C CNN
	1    5550 950 
	0    1    1    0   
$EndComp
$Comp
L RJ45_2 J8
U 1 1 610EAF3A
P 5550 1800
F 0 "J8" H 5750 2300 60  0000 C CNN
F 1 "RJ45_2" H 5400 2300 60  0000 C CNN
F 2 "~" H 5550 1800 60  0000 C CNN
F 3 "~" H 5550 1800 60  0000 C CNN
	1    5550 1800
	0    1    1    0   
$EndComp
Wire Wire Line
	1500 600  5300 600 
Connection ~ 2900 600 
Connection ~ 4050 600 
Wire Wire Line
	1550 700  5350 700 
Connection ~ 4050 700 
Connection ~ 2900 700 
Wire Wire Line
	1500 800  5400 800 
Connection ~ 2900 800 
Connection ~ 4050 800 
Wire Wire Line
	1550 900  5450 900 
Connection ~ 4050 900 
Connection ~ 2900 900 
Wire Wire Line
	1500 1000 5500 1000
Connection ~ 2900 1000
Connection ~ 4050 1000
Wire Wire Line
	1550 1100 5550 1100
Connection ~ 4050 1100
Connection ~ 2900 1100
Wire Wire Line
	1500 1200 5600 1200
Connection ~ 2900 1200
Connection ~ 4050 1200
Wire Wire Line
	1550 1300 5650 1300
Connection ~ 4050 1300
Connection ~ 2900 1300
Wire Wire Line
	1850 1450 5300 1450
Connection ~ 2900 1450
Connection ~ 4050 1450
Wire Wire Line
	1850 1550 5350 1550
Connection ~ 4050 1550
Connection ~ 2900 1550
Wire Wire Line
	1850 1650 5400 1650
Connection ~ 2900 1650
Connection ~ 4050 1650
Wire Wire Line
	1850 1750 5450 1750
Connection ~ 2900 1750
Connection ~ 4050 1750
Wire Wire Line
	1850 1850 5500 1850
Connection ~ 4050 1850
Connection ~ 2900 1850
Wire Wire Line
	1850 1950 5550 1950
Connection ~ 2900 1950
Connection ~ 4050 1950
Wire Wire Line
	1850 2050 5600 2050
Connection ~ 4050 2050
Connection ~ 2900 2050
Wire Wire Line
	1850 2150 5650 2150
Connection ~ 2900 2150
Connection ~ 4050 2150
Connection ~ 1850 600 
Connection ~ 1500 800 
Connection ~ 1850 800 
Connection ~ 1500 1000
Connection ~ 1850 1000
Connection ~ 1500 1200
Connection ~ 1850 1200
Connection ~ 1850 700 
Connection ~ 1550 1150
Connection ~ 1550 900 
Connection ~ 1850 900 
Connection ~ 1550 1100
Connection ~ 1850 1100
Connection ~ 1550 1300
Connection ~ 1850 1300
Wire Wire Line
	5300 1450 5300 600 
Connection ~ 5100 600 
Connection ~ 5100 1450
Wire Wire Line
	5350 1550 5350 700 
Connection ~ 5100 1550
Connection ~ 5100 700 
Wire Wire Line
	5400 1650 5400 800 
Connection ~ 5100 800 
Connection ~ 5100 1650
Wire Wire Line
	5450 1750 5450 900 
Connection ~ 5100 1750
Connection ~ 5100 900 
Wire Wire Line
	5500 1850 5500 1000
Connection ~ 5100 1000
Wire Wire Line
	5550 1950 5550 1100
Connection ~ 5100 1950
Connection ~ 5100 1100
Wire Wire Line
	5600 2050 5600 1200
Connection ~ 5100 1200
Connection ~ 5100 2050
Wire Wire Line
	5650 2150 5650 1300
Connection ~ 5100 2150
Connection ~ 5100 1300
Connection ~ 5100 1850
$Comp
L USB-MICRO-B CON2
U 1 1 6116C32E
P 1400 2600
F 0 "CON2" H 1150 3050 60  0000 C CNN
F 1 "USB-MICRO-B" H 1350 2100 60  0000 C CNN
F 2 "~" H 1400 2600 60  0000 C CNN
F 3 "~" H 1400 2600 60  0000 C CNN
	1    1400 2600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
