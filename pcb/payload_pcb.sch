EESchema Schematic File Version 2
LIBS:payload_pcb-rescue
LIBS:ms5607_module
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
LIBS:xbee_s3b
LIBS:mc33063a
LIBS:payload_pcb-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATMEGA328-P IC1
U 1 1 58037836
P 7000 3700
F 0 "IC1" H 6250 4950 50  0000 L BNN
F 1 "ATMEGA328-P" H 7400 2300 50  0000 L BNN
F 2 "Housings_DIP:DIP-28_W7.62mm_LongPads" V 6700 3700 50  0000 C CIN
F 3 "" H 7000 3700 50  0000 C CNN
	1    7000 3700
	1    0    0    -1  
$EndComp
$Comp
L XBee_S3B U2
U 1 1 58038057
P 3400 2650
F 0 "U2" H 3400 3200 60  0000 C CNN
F 1 "XBee_S3B" H 3400 2650 60  0000 C CNN
F 2 "payload:XBee_S3B" H 3400 2700 60  0001 C CNN
F 3 "" H 3400 2700 60  0001 C CNN
	1    3400 2650
	1    0    0    -1  
$EndComp
Text GLabel 8000 4300 2    60   Input ~ 0
XBEE_RX
Text GLabel 8000 4200 2    60   Input ~ 0
XBEE_TX
$Comp
L GND #PWR16
U 1 1 58040DA4
P 6100 4900
F 0 "#PWR16" H 6100 4650 50  0001 C CNN
F 1 "GND" H 6100 4750 50  0000 C CNN
F 2 "" H 6100 4900 50  0000 C CNN
F 3 "" H 6100 4900 50  0000 C CNN
	1    6100 4900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR14
U 1 1 58040DC8
P 5950 4800
F 0 "#PWR14" H 5950 4550 50  0001 C CNN
F 1 "GND" H 5950 4650 50  0000 C CNN
F 2 "" H 5950 4800 50  0000 C CNN
F 3 "" H 5950 4800 50  0000 C CNN
	1    5950 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 4800 6100 4800
Text GLabel 2600 2350 0    60   Input ~ 0
XBEE_RX
Text GLabel 2600 2450 0    60   Input ~ 0
XBEE_TX
$Comp
L Crystal Y1
U 1 1 58042CEB
P 8750 3250
F 0 "Y1" H 8750 3400 50  0000 C CNN
F 1 "Crystal" H 8750 3100 50  0000 C CNN
F 2 "Crystals:Crystal_HC50-U_Vertical" H 8750 3250 50  0001 C CNN
F 3 "" H 8750 3250 50  0000 C CNN
	1    8750 3250
	0    1    1    0   
$EndComp
$Comp
L C C7
U 1 1 58042DB7
P 8900 3100
F 0 "C7" H 8925 3200 50  0000 L CNN
F 1 "22p" H 8925 3000 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Rect_L4_W2.5_P2.5" H 8938 2950 50  0001 C CNN
F 3 "" H 8900 3100 50  0000 C CNN
	1    8900 3100
	0    -1   -1   0   
$EndComp
$Comp
L C C8
U 1 1 58042E38
P 8900 3400
F 0 "C8" H 9000 3300 50  0000 L CNN
F 1 "22p" H 8950 3500 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Rect_L4_W2.5_P2.5" H 8938 3250 50  0001 C CNN
F 3 "" H 8900 3400 50  0000 C CNN
	1    8900 3400
	0    1    1    0   
$EndComp
$Comp
L GND #PWR23
U 1 1 58042ED1
P 9200 3100
F 0 "#PWR23" H 9200 2850 50  0001 C CNN
F 1 "GND" H 9200 2950 50  0000 C CNN
F 2 "" H 9200 3100 50  0000 C CNN
F 3 "" H 9200 3100 50  0000 C CNN
	1    9200 3100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR24
U 1 1 58042EF7
P 9200 3400
F 0 "#PWR24" H 9200 3150 50  0001 C CNN
F 1 "GND" H 9200 3250 50  0000 C CNN
F 2 "" H 9200 3400 50  0000 C CNN
F 3 "" H 9200 3400 50  0000 C CNN
	1    9200 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 3400 9200 3400
Wire Wire Line
	9050 3100 9200 3100
Wire Wire Line
	8000 3200 8500 3200
Wire Wire Line
	8500 3200 8500 3100
Wire Wire Line
	8500 3100 8750 3100
Wire Wire Line
	8000 3300 8500 3300
Wire Wire Line
	8500 3300 8500 3400
Wire Wire Line
	8500 3400 8750 3400
$Comp
L LED_Small D2
U 1 1 58043A6E
P 6900 1900
F 0 "D2" H 6850 2025 50  0000 L CNN
F 1 "Green" H 6725 1800 50  0000 L CNN
F 2 "LEDs:LED-5MM" V 6900 1900 50  0001 C CNN
F 3 "" V 6900 1900 50  0000 C CNN
	1    6900 1900
	-1   0    0    1   
$EndComp
$Comp
L LED_Small D3
U 1 1 58043D2C
P 6900 1550
F 0 "D3" H 6850 1675 50  0000 L CNN
F 1 "Yellow" H 6725 1450 50  0000 L CNN
F 2 "LEDs:LED-5MM" V 6900 1550 50  0001 C CNN
F 3 "" V 6900 1550 50  0000 C CNN
	1    6900 1550
	-1   0    0    1   
$EndComp
$Comp
L LED_Small D4
U 1 1 58043D5D
P 6900 1200
F 0 "D4" H 6850 1325 50  0000 L CNN
F 1 "Red" H 6800 1100 50  0000 L CNN
F 2 "LEDs:LED-5MM" V 6900 1200 50  0001 C CNN
F 3 "" V 6900 1200 50  0000 C CNN
	1    6900 1200
	-1   0    0    1   
$EndComp
Text GLabel 7000 1900 2    60   Input ~ 0
STATUS0
Text GLabel 7000 1550 2    60   Input ~ 0
STATUS1
Text GLabel 7000 1200 2    60   Input ~ 0
STATUS2
Text GLabel 8000 3650 2    60   Input ~ 0
STATUS2
Text GLabel 8000 3550 2    60   Input ~ 0
STATUS1
Text GLabel 8000 3450 2    60   Input ~ 0
STATUS0
NoConn ~ 6100 3200
Text GLabel 8000 2700 2    60   Input ~ 0
BALLAST
Text GLabel 8000 3100 2    60   Input ~ 0
SCK
Text GLabel 8000 3000 2    60   Input ~ 0
MISO
Text GLabel 8000 2900 2    60   Input ~ 0
MOSI
Text GLabel 8000 2800 2    60   Input ~ 0
CSB
NoConn ~ 8000 4900
NoConn ~ 2600 2550
NoConn ~ 2600 2750
NoConn ~ 2600 2850
NoConn ~ 2600 2950
NoConn ~ 2600 3050
NoConn ~ 4150 2250
NoConn ~ 4150 2350
NoConn ~ 4150 2450
NoConn ~ 4150 2550
NoConn ~ 4150 2650
NoConn ~ 4150 2750
NoConn ~ 4150 2850
NoConn ~ 4150 2950
NoConn ~ 4150 3050
NoConn ~ 4150 3150
$Comp
L MS5607_Module J1
U 1 1 5804A8DD
P 10150 4350
F 0 "J1" H 10150 4950 60  0000 C CNN
F 1 "MS5607_Module" V 9950 4500 60  0000 C CNN
F 2 "payload:MS5607_Module" H 10150 4350 60  0001 C CNN
F 3 "" H 10150 4350 60  0001 C CNN
	1    10150 4350
	-1   0    0    1   
$EndComp
Wire Wire Line
	9600 3500 9600 4300
Wire Wire Line
	9600 4300 9850 4300
$Comp
L GND #PWR25
U 1 1 5804AA6C
P 9350 4400
F 0 "#PWR25" H 9350 4150 50  0001 C CNN
F 1 "GND" H 9350 4250 50  0000 C CNN
F 2 "" H 9350 4400 50  0000 C CNN
F 3 "" H 9350 4400 50  0000 C CNN
	1    9350 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 4400 9350 4400
Text GLabel 9850 4500 0    60   Input ~ 0
SCK
Text GLabel 9850 4600 0    60   Input ~ 0
MOSI
Text GLabel 9850 4700 0    60   Input ~ 0
MISO
Text GLabel 9850 4800 0    60   Input ~ 0
CSB
NoConn ~ 8000 4800
Text GLabel 5750 5600 0    60   Input ~ 0
BALLAST
$Comp
L R R3
U 1 1 5804C2D9
P 6100 5750
F 0 "R3" V 6180 5750 50  0000 C CNN
F 1 "4.7k" V 6100 5750 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 6030 5750 50  0001 C CNN
F 3 "" H 6100 5750 50  0000 C CNN
	1    6100 5750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR17
U 1 1 5804C32E
P 6100 5900
F 0 "#PWR17" H 6100 5650 50  0001 C CNN
F 1 "GND" H 6100 5750 50  0000 C CNN
F 2 "" H 6100 5900 50  0000 C CNN
F 3 "" H 6100 5900 50  0000 C CNN
	1    6100 5900
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GDS Q1
U 1 1 58051E04
P 6650 5600
F 0 "Q1" H 6950 5650 50  0000 R CNN
F 1 "Q1" H 6950 5550 50  0001 R CNN
F 2 "TO_SOT_Packages_THT:TO-220_Neutral123_Horizontal" H 6850 5700 50  0001 C CNN
F 3 "" H 6650 5600 50  0000 C CNN
	1    6650 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 5600 6450 5600
Connection ~ 6100 5600
Wire Wire Line
	6750 5400 7850 5400
$Comp
L VDD #PWR19
U 1 1 58052129
P 7500 5700
F 0 "#PWR19" H 7500 5550 50  0001 C CNN
F 1 "VDD" H 7500 5850 50  0000 C CNN
F 2 "" H 7500 5700 50  0000 C CNN
F 3 "" H 7500 5700 50  0000 C CNN
	1    7500 5700
	-1   0    0    1   
$EndComp
$Comp
L D D1
U 1 1 58052197
P 7500 5550
F 0 "D1" H 7500 5650 50  0000 C CNN
F 1 "D" H 7500 5450 50  0001 C CNN
F 2 "Diodes_ThroughHole:Diode_DO-41_SOD81_Horizontal_RM10" H 7500 5550 50  0001 C CNN
F 3 "" H 7500 5550 50  0000 C CNN
	1    7500 5550
	0    -1   -1   0   
$EndComp
Connection ~ 7500 5400
Connection ~ 8750 3100
Connection ~ 8750 3400
Text GLabel 8000 2600 2    60   Input ~ 0
MODE
$Comp
L GND #PWR18
U 1 1 580553B7
P 6750 5800
F 0 "#PWR18" H 6750 5550 50  0001 C CNN
F 1 "GND" H 6750 5650 50  0000 C CNN
F 2 "" H 6750 5800 50  0000 C CNN
F 3 "" H 6750 5800 50  0000 C CNN
	1    6750 5800
	1    0    0    -1  
$EndComp
$Comp
L MC33063A U1
U 1 1 580599C0
P 3350 5700
F 0 "U1" H 3350 6150 60  0000 C CNN
F 1 "MC33063A" H 3350 5550 60  0000 C CNN
F 2 "Housings_DIP:DIP-8_W7.62mm_LongPads" H 3350 5700 60  0001 C CNN
F 3 "" H 3350 5700 60  0001 C CNN
	1    3350 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 5400 4400 5400
Wire Wire Line
	3850 5100 3850 5500
Wire Wire Line
	3850 5100 2750 5100
Wire Wire Line
	2750 5100 2750 5400
Wire Wire Line
	2750 5400 2900 5400
Wire Wire Line
	3850 5500 3750 5500
Connection ~ 3850 5400
Wire Wire Line
	4150 5400 4150 5600
Wire Wire Line
	4150 5600 3750 5600
$Comp
L VDD #PWR12
U 1 1 5805ABD6
P 4400 5400
F 0 "#PWR12" H 4400 5250 50  0001 C CNN
F 1 "VDD" H 4400 5550 50  0000 C CNN
F 2 "" H 4400 5400 50  0000 C CNN
F 3 "" H 4400 5400 50  0000 C CNN
	1    4400 5400
	1    0    0    -1  
$EndComp
$Comp
L CP1 C6
U 1 1 5805AC48
P 4400 5550
F 0 "C6" H 4425 5650 50  0000 L CNN
F 1 "100 uF" H 4450 5450 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D6.3_L11.2_P2.5" H 4400 5550 50  0001 C CNN
F 3 "" H 4400 5550 50  0000 C CNN
	1    4400 5550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR13
U 1 1 5805ACAF
P 4400 5700
F 0 "#PWR13" H 4400 5450 50  0001 C CNN
F 1 "GND" H 4400 5550 50  0000 C CNN
F 2 "" H 4400 5700 50  0000 C CNN
F 3 "" H 4400 5700 50  0000 C CNN
	1    4400 5700
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 5805ACE9
P 3750 6400
F 0 "R1" V 3830 6400 50  0000 C CNN
F 1 "22k" V 3750 6400 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 3680 6400 50  0001 C CNN
F 3 "" H 3750 6400 50  0000 C CNN
	1    3750 6400
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 5805ADB2
P 3350 6250
F 0 "R2" V 3430 6250 50  0000 C CNN
F 1 "36k" V 3350 6250 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 3280 6250 50  0001 C CNN
F 3 "" H 3350 6250 50  0000 C CNN
	1    3350 6250
	0    1    1    0   
$EndComp
Wire Wire Line
	3750 5700 3750 6250
Wire Wire Line
	3750 6250 3500 6250
Connection ~ 3750 6250
$Comp
L GND #PWR11
U 1 1 5805AE88
P 3750 6550
F 0 "#PWR11" H 3750 6300 50  0001 C CNN
F 1 "GND" H 3750 6400 50  0000 C CNN
F 2 "" H 3750 6550 50  0000 C CNN
F 3 "" H 3750 6550 50  0000 C CNN
	1    3750 6550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR8
U 1 1 5805AEC6
P 2900 5700
F 0 "#PWR8" H 2900 5450 50  0001 C CNN
F 1 "GND" H 2900 5550 50  0000 C CNN
F 2 "" H 2900 5700 50  0000 C CNN
F 3 "" H 2900 5700 50  0000 C CNN
	1    2900 5700
	1    0    0    -1  
$EndComp
$Comp
L CP1 CT1
U 1 1 5805AF19
P 2600 5750
F 0 "CT1" H 2625 5850 50  0000 L CNN
F 1 "160 pF" V 2450 5750 50  0000 C CNN
F 2 "Capacitors_ThroughHole:C_Rect_L4_W2.5_P2.5" H 2600 5750 50  0001 C CNN
F 3 "" H 2600 5750 50  0000 C CNN
	1    2600 5750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR7
U 1 1 5805B02F
P 2600 5900
F 0 "#PWR7" H 2600 5650 50  0001 C CNN
F 1 "GND" H 2600 5750 50  0000 C CNN
F 2 "" H 2600 5900 50  0000 C CNN
F 3 "" H 2600 5900 50  0000 C CNN
	1    2600 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 5600 2900 5600
$Comp
L CP1 C5
U 1 1 5805B172
P 2300 6400
F 0 "C5" H 2325 6500 50  0000 L CNN
F 1 "22uF" H 2325 6300 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D5_L11_P2.5" H 2300 6400 50  0001 C CNN
F 3 "" H 2300 6400 50  0000 C CNN
	1    2300 6400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR3
U 1 1 5805B1C3
P 2300 6550
F 0 "#PWR3" H 2300 6300 50  0001 C CNN
F 1 "GND" H 2300 6400 50  0000 C CNN
F 2 "" H 2300 6550 50  0000 C CNN
F 3 "" H 2300 6550 50  0000 C CNN
	1    2300 6550
	1    0    0    -1  
$EndComp
$Comp
L D_Schottky 1N5819
U 1 1 5805B249
P 2050 5650
F 0 "1N5819" H 2100 5750 50  0000 C CNN
F 1 "D5" H 2050 5550 50  0000 C CNN
F 2 "Diodes_ThroughHole:Diode_DO-41_SOD81_Horizontal_RM10" H 2050 5650 50  0001 C CNN
F 3 "" H 2050 5650 50  0000 C CNN
	1    2050 5650
	0    1    1    0   
$EndComp
$Comp
L GND #PWR2
U 1 1 5805B2C6
P 2050 5800
F 0 "#PWR2" H 2050 5550 50  0001 C CNN
F 1 "GND" H 2050 5650 50  0000 C CNN
F 2 "" H 2050 5800 50  0000 C CNN
F 3 "" H 2050 5800 50  0000 C CNN
	1    2050 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 5500 2900 5500
Connection ~ 2300 5500
$Comp
L L_Small L1
U 1 1 5805B4FC
P 2300 6100
F 0 "L1" H 2330 6140 50  0000 L CNN
F 1 "50 uH" H 2330 6060 50  0000 L CNN
F 2 "Choke_Axial_ThroughHole:Choke_Horizontal_RM25mm" H 2300 6100 50  0001 C CNN
F 3 "" H 2300 6100 50  0000 C CNN
	1    2300 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 6250 2300 6200
Wire Wire Line
	2300 6000 2300 5500
Connection ~ 2300 6250
$Comp
L R R4
U 1 1 5806196A
P 6550 1200
F 0 "R4" V 6630 1200 50  0000 C CNN
F 1 "270" V 6550 1200 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 6480 1200 50  0001 C CNN
F 3 "" H 6550 1200 50  0000 C CNN
	1    6550 1200
	0    1    1    0   
$EndComp
$Comp
L R R5
U 1 1 580619BD
P 6550 1550
F 0 "R5" V 6630 1550 50  0000 C CNN
F 1 "270" V 6550 1550 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 6480 1550 50  0001 C CNN
F 3 "" H 6550 1550 50  0000 C CNN
	1    6550 1550
	0    1    1    0   
$EndComp
$Comp
L R R6
U 1 1 58061A0E
P 6550 1900
F 0 "R6" V 6630 1900 50  0000 C CNN
F 1 "270" V 6550 1900 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" V 6480 1900 50  0001 C CNN
F 3 "" H 6550 1900 50  0000 C CNN
	1    6550 1900
	0    1    1    0   
$EndComp
Wire Wire Line
	6300 1200 6400 1200
Wire Wire Line
	6700 1200 6800 1200
Wire Wire Line
	6700 1550 6800 1550
Wire Wire Line
	6300 1550 6400 1550
Wire Wire Line
	6300 1900 6400 1900
Wire Wire Line
	6700 1900 6800 1900
Text GLabel 8000 3750 2    60   Input ~ 0
STATUS3
Text GLabel 8000 3850 2    60   Input ~ 0
STATUS4
Text GLabel 8000 3950 2    60   Input ~ 0
STATUS5
Text GLabel 8000 4600 2    60   Input ~ 0
STATUS6
Text GLabel 8000 4500 2    60   Input ~ 0
STATUS7
Wire Wire Line
	2200 6250 3200 6250
$Comp
L L_Small L2
U 1 1 580F13FF
P 2100 6250
F 0 "L2" V 2200 6200 50  0000 L CNN
F 1 "1 uH" V 2050 6150 50  0000 L CNN
F 2 "Choke_Axial_ThroughHole:Choke_Horizontal_RM15mm" H 2100 6250 50  0001 C CNN
F 3 "" H 2100 6250 50  0000 C CNN
	1    2100 6250
	0    -1   -1   0   
$EndComp
$Comp
L CP1 C9
U 1 1 580F15ED
P 1800 6400
F 0 "C9" H 1825 6500 50  0000 L CNN
F 1 "100uF" H 1825 6300 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D6.3_L11.2_P2.5" H 1800 6400 50  0001 C CNN
F 3 "" H 1800 6400 50  0000 C CNN
	1    1800 6400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR1
U 1 1 580F165E
P 1800 6550
F 0 "#PWR1" H 1800 6300 50  0001 C CNN
F 1 "GND" H 1800 6400 50  0000 C CNN
F 2 "" H 1800 6550 50  0000 C CNN
F 3 "" H 1800 6550 50  0000 C CNN
	1    1800 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1600 6250 2000 6250
Connection ~ 1800 6250
$Comp
L VDD #PWR9
U 1 1 5810A574
P 3200 4000
F 0 "#PWR9" H 3200 3850 50  0001 C CNN
F 1 "VDD" H 3200 4150 50  0000 C CNN
F 2 "" H 3200 4000 50  0000 C CNN
F 3 "" H 3200 4000 50  0000 C CNN
	1    3200 4000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR10
U 1 1 5810A5C4
P 3550 4000
F 0 "#PWR10" H 3550 3750 50  0001 C CNN
F 1 "GND" H 3550 3850 50  0000 C CNN
F 2 "" H 3550 4000 50  0000 C CNN
F 3 "" H 3550 4000 50  0000 C CNN
	1    3550 4000
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P1
U 1 1 5810A614
P 3250 4200
F 0 "P1" H 3250 4350 50  0000 C CNN
F 1 "BAT_TERMINALS" V 3350 4200 50  0000 C CNN
F 2 "payload:XT60" H 3250 4200 50  0001 C CNN
F 3 "" H 3250 4200 50  0000 C CNN
	1    3250 4200
	0    1    1    0   
$EndComp
Text GLabel 1600 6250 0    60   Input ~ 0
3.3V
Text GLabel 2100 2250 0    60   Input ~ 0
3.3V
Text GLabel 5550 2600 0    60   Input ~ 0
3.3V
Text GLabel 6100 2900 0    60   Input ~ 0
3.3V
Text GLabel 8950 4050 2    60   Input ~ 0
3.3V
Text GLabel 9600 3500 1    60   Input ~ 0
3.3V
Text GLabel 2600 2650 0    60   Input ~ 0
3.3V
Text Notes 7350 7500 0    60   ~ 0
Payload Schematic
Text Notes 10600 7650 0    60   ~ 0
1\n
Text Notes 8150 7650 0    60   ~ 0
February 10, 2016
$Comp
L GND #PWR6
U 1 1 581BCC6E
P 2600 3150
F 0 "#PWR6" H 2600 2900 50  0001 C CNN
F 1 "GND" H 2600 3000 50  0000 C CNN
F 2 "" H 2600 3150 50  0000 C CNN
F 3 "" H 2600 3150 50  0000 C CNN
	1    2600 3150
	1    0    0    -1  
$EndComp
$Comp
L CP1 C1
U 1 1 581C194F
P 2350 2100
F 0 "C1" H 2375 2200 50  0000 L CNN
F 1 "CP1" H 2375 2000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2350 2100 50  0001 C CNN
F 3 "" H 2350 2100 50  0000 C CNN
	1    2350 2100
	-1   0    0    1   
$EndComp
$Comp
L CP1 C4
U 1 1 581C1A6C
P 6000 2450
F 0 "C4" H 6025 2550 50  0000 L CNN
F 1 "CP1" H 6025 2350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6000 2450 50  0001 C CNN
F 3 "" H 6000 2450 50  0000 C CNN
	1    6000 2450
	-1   0    0    1   
$EndComp
$Comp
L CP1 C2
U 1 1 581C1AFC
P 2600 2100
F 0 "C2" H 2625 2200 50  0000 L CNN
F 1 "CP1" H 2625 2000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2600 2100 50  0001 C CNN
F 3 "" H 2600 2100 50  0000 C CNN
	1    2600 2100
	-1   0    0    1   
$EndComp
Wire Wire Line
	2100 2250 2600 2250
Connection ~ 2350 2250
$Comp
L GND #PWR4
U 1 1 581C2043
P 2200 1700
F 0 "#PWR4" H 2200 1450 50  0001 C CNN
F 1 "GND" H 2200 1550 50  0000 C CNN
F 2 "" H 2200 1700 50  0000 C CNN
F 3 "" H 2200 1700 50  0000 C CNN
	1    2200 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 2600 6100 2600
Connection ~ 6000 2600
$Comp
L GND #PWR15
U 1 1 581C2954
P 6150 2150
F 0 "#PWR15" H 6150 1900 50  0001 C CNN
F 1 "GND" H 6150 2000 50  0000 C CNN
F 2 "" H 6150 2150 50  0000 C CNN
F 3 "" H 6150 2150 50  0000 C CNN
	1    6150 2150
	1    0    0    -1  
$EndComp
$Comp
L CP1 C11
U 1 1 581C2B8D
P 9700 3900
F 0 "C11" H 9800 3900 50  0000 L CNN
F 1 "CP1" H 9725 3800 50  0001 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9700 3900 50  0001 C CNN
F 3 "" H 9700 3900 50  0000 C CNN
	1    9700 3900
	0    -1   -1   0   
$EndComp
Connection ~ 9600 3900
Text GLabel 6300 1900 0    60   Input ~ 0
3.3V
Text GLabel 6300 1550 0    60   Input ~ 0
3.3V
Text GLabel 6300 1200 0    60   Input ~ 0
3.3V
$Comp
L CONN_01X10 P2
U 1 1 58212C0D
P 10300 2050
F 0 "P2" H 10300 2500 50  0000 C CNN
F 1 "CONN_01X10" V 10400 2050 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x10" H 10300 2050 50  0001 C CNN
F 3 "" H 10300 2050 50  0000 C CNN
	1    10300 2050
	1    0    0    -1  
$EndComp
Text GLabel 10100 1600 0    60   Input ~ 0
STATUS0
Text GLabel 10100 1700 0    60   Input ~ 0
STATUS1
Text GLabel 10100 1800 0    60   Input ~ 0
STATUS2
Text GLabel 10100 1900 0    60   Input ~ 0
STATUS3
Text GLabel 10100 2000 0    60   Input ~ 0
STATUS4
Text GLabel 10100 2100 0    60   Input ~ 0
STATUS5
Text GLabel 10100 2200 0    60   Input ~ 0
STATUS6
Text GLabel 10100 2300 0    60   Input ~ 0
STATUS7
$Comp
L R_Small R7
U 1 1 58213358
P 8850 4050
F 0 "R7" V 8750 4000 50  0000 L CNN
F 1 "4.7k" V 8950 3950 50  0000 L CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" H 8850 4050 50  0001 C CNN
F 3 "" H 8850 4050 50  0000 C CNN
	1    8850 4050
	0    1    1    0   
$EndComp
$Comp
L GND #PWR21
U 1 1 58213429
P 8650 4350
F 0 "#PWR21" H 8650 4100 50  0001 C CNN
F 1 "GND" H 8650 4200 50  0000 C CNN
F 2 "" H 8650 4350 50  0000 C CNN
F 3 "" H 8650 4350 50  0000 C CNN
	1    8650 4350
	1    0    0    -1  
$EndComp
$Comp
L C_Small C3
U 1 1 58213481
P 8650 4250
F 0 "C3" H 8800 4250 50  0000 L CNN
F 1 "0.1uF" H 8660 4170 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8650 4250 50  0001 C CNN
F 3 "" H 8650 4250 50  0000 C CNN
	1    8650 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 4050 8750 4050
Connection ~ 4150 5400
$Comp
L GND #PWR30
U 1 1 58216FF5
P 10200 2650
F 0 "#PWR30" H 10200 2400 50  0001 C CNN
F 1 "GND" H 10200 2500 50  0000 C CNN
F 2 "" H 10200 2650 50  0000 C CNN
F 3 "" H 10200 2650 50  0000 C CNN
	1    10200 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 4150 8650 4050
Connection ~ 8650 4050
Text GLabel 8650 4050 1    60   Input ~ 0
RESET
Text GLabel 10350 2950 0    60   Input ~ 0
RESET
Text GLabel 10350 3050 0    60   Input ~ 0
MOSI
Text GLabel 10350 3350 0    60   Input ~ 0
MISO
Text GLabel 10350 3150 0    60   Input ~ 0
SCK
Text GLabel 10350 3250 0    60   Input ~ 0
3.3V
$Comp
L GND #PWR27
U 1 1 58222696
P 10000 3900
F 0 "#PWR27" H 10000 3650 50  0001 C CNN
F 1 "GND" H 10000 3750 50  0000 C CNN
F 2 "" H 10000 3900 50  0000 C CNN
F 3 "" H 10000 3900 50  0000 C CNN
	1    10000 3900
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X07 P4
U 1 1 582235F0
P 10550 3150
F 0 "P4" H 10550 3550 50  0000 C CNN
F 1 "CONN_01X07" V 10650 3150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x07" H 10550 3150 50  0001 C CNN
F 3 "" H 10550 3150 50  0000 C CNN
	1    10550 3150
	1    0    0    -1  
$EndComp
Text GLabel 10350 3450 0    60   Input ~ 0
BALLAST
NoConn ~ 8000 4700
Text GLabel 9900 1200 0    60   Input ~ 0
MODE
Text GLabel 10100 2500 0    60   Input ~ 0
XBEE_RX
Text GLabel 10100 2400 0    60   Input ~ 0
XBEE_TX
$Comp
L R_Small R8
U 1 1 58225FD3
P 10050 1100
F 0 "R8" H 10080 1120 50  0000 L CNN
F 1 "4.7K" H 10080 1060 50  0000 L CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM7mm" H 10050 1100 50  0001 C CNN
F 3 "" H 10050 1100 50  0000 C CNN
	1    10050 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9900 1200 10250 1200
Connection ~ 10050 1200
Text GLabel 10050 1000 1    60   Input ~ 0
3.3V
$Comp
L CONN_01X02 P5
U 1 1 58229CE5
P 7900 5600
F 0 "P5" H 7900 5750 50  0000 C CNN
F 1 "VALVE" V 8000 5600 50  0000 C CNN
F 2 "Connectors_JST:JST_EH_B02B-EH-A_02x2.50mm_Straight" H 7900 5600 50  0001 C CNN
F 3 "" H 7900 5600 50  0000 C CNN
	1    7900 5600
	0    1    1    0   
$EndComp
$Comp
L VDD #PWR20
U 1 1 5822A048
P 7950 5400
F 0 "#PWR20" H 7950 5250 50  0001 C CNN
F 1 "VDD" H 7950 5550 50  0000 C CNN
F 2 "" H 7950 5400 50  0000 C CNN
F 3 "" H 7950 5400 50  0000 C CNN
	1    7950 5400
	1    0    0    -1  
$EndComp
NoConn ~ 8000 4400
$Comp
L CONN_01X03 P3
U 1 1 5822BCE3
P 10450 1300
F 0 "P3" H 10450 1500 50  0000 C CNN
F 1 "CONN_01X03" V 10550 1300 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 10450 1300 50  0001 C CNN
F 3 "" H 10450 1300 50  0000 C CNN
	1    10450 1300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR29
U 1 1 5822BDEC
P 10100 1300
F 0 "#PWR29" H 10100 1050 50  0001 C CNN
F 1 "GND" H 10100 1150 50  0000 C CNN
F 2 "" H 10100 1300 50  0000 C CNN
F 3 "" H 10100 1300 50  0000 C CNN
	1    10100 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 3900 10000 3900
Wire Wire Line
	9850 4200 9850 3900
Wire Wire Line
	10350 2850 10350 2650
Wire Wire Line
	10350 2650 10200 2650
Wire Wire Line
	10250 1400 10250 1300
Wire Wire Line
	10250 1300 10100 1300
Wire Wire Line
	6000 2300 6000 2150
Wire Wire Line
	6000 2150 6150 2150
Wire Wire Line
	2350 1950 2350 1700
Wire Wire Line
	2200 1700 2600 1700
Wire Wire Line
	2600 1700 2600 1950
Connection ~ 2350 1700
Wire Wire Line
	3300 4000 3550 4000
$EndSCHEMATC
