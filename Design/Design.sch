EESchema Schematic File Version 2
LIBS:Design-rescue
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
LIBS:components
LIBS:Symbols_EN60617_13Mar2013
LIBS:Symbols_EN60617-10_HF-Radio_DRAFT_12Sep2013
LIBS:Symbols_Transformer-Diskrete_RevA
LIBS:Symbols_ICs-Diskrete_RevD10
LIBS:Symbols_ICs-Opto_RevB_16Sep2013
LIBS:Symbols_Socket-DIN41612_RevA
LIBS:Symbols_DCDC-ACDC-Converter_RevC_20Jul2012
LIBS:Symbols_Microcontroller_Philips-NXP_RevA_06Oct2013
LIBS:SymbolsSimilarEN60617+oldDIN617-RevE8
LIBS:ac-dc
LIBS:rfcom
LIBS:video
LIBS:microchip_pic16mcu
LIBS:microchip_pic10mcu
LIBS:onsemi
LIBS:brooktre
LIBS:Oscillators
LIBS:stm32
LIBS:sensors
LIBS:elec-unifil
LIBS:graphic
LIBS:Zilog
LIBS:ir
LIBS:dc-dc
LIBS:ftdi
LIBS:microchip_pic18mcu
LIBS:msp430
LIBS:powerint
LIBS:hc11
LIBS:Lattice
LIBS:Xicor
LIBS:switches
LIBS:references
LIBS:silabs
LIBS:transf
LIBS:maxim
LIBS:gennum
LIBS:cmos_ieee
LIBS:analog_devices
LIBS:nordicsemi
LIBS:microchip_pic32mcu
LIBS:74xgxx
LIBS:nxp_armmcu
LIBS:supertex
LIBS:relays
LIBS:ttl_ieee
LIBS:motor_drivers
LIBS:microchip_pic12mcu
LIBS:actel
LIBS:stm8
LIBS:diode
LIBS:Power_Management
LIBS:ESD_Protection
LIBS:Altera
LIBS:microchip_dspic33dsc
LIBS:pspice
LIBS:logo
LIBS:Design-cache
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
L SR1100 D1
U 1 1 5707226B
P 2000 750
F 0 "D1" H 2000 850 50  0000 C CNN
F 1 "SR1100" H 2000 650 50  0000 C CNN
F 2 "" H 2000 750 50  0000 C CNN
F 3 "" H 2000 750 50  0000 C CNN
	1    2000 750 
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR01
U 1 1 57072383
P 2250 600
F 0 "#PWR01" H 2250 690 20  0001 C CNN
F 1 "+5V" H 2250 690 30  0000 C CNN
F 2 "" H 2250 600 60  0000 C CNN
F 3 "" H 2250 600 60  0000 C CNN
	1    2250 600 
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 57072428
P 2250 1800
F 0 "R2" V 2330 1800 50  0000 C CNN
F 1 "1k" V 2250 1800 50  0000 C CNN
F 2 "" V 2180 1800 50  0000 C CNN
F 3 "" H 2250 1800 50  0000 C CNN
	1    2250 1800
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 570724EF
P 2250 1500
F 0 "R1" V 2330 1500 50  0000 C CNN
F 1 "1k" V 2250 1500 50  0000 C CNN
F 2 "" V 2180 1500 50  0000 C CNN
F 3 "" H 2250 1500 50  0000 C CNN
	1    2250 1500
	1    0    0    -1  
$EndComp
Text GLabel 1750 900  0    60   Input ~ 0
VSR_CAM_PIN_1
Text GLabel 1850 1650 0    60   Input ~ 0
VSR_CAM_PIN_2
$Comp
L SR1100 D2
U 1 1 57072985
P 2000 1200
F 0 "D2" H 2000 1300 50  0000 C CNN
F 1 "SR1100" H 2000 1100 50  0000 C CNN
F 2 "" H 2000 1200 50  0000 C CNN
F 3 "" H 2000 1200 50  0000 C CNN
	1    2000 1200
	0    1    1    0   
$EndComp
Connection ~ 2250 600 
Wire Wire Line
	2000 600  3600 600 
Connection ~ 2250 1650
Connection ~ 2000 1650
Wire Wire Line
	1750 900  3000 900 
Wire Wire Line
	2250 600  2250 1350
$Comp
L LM393 U1
U 1 1 57072E24
P 3300 1550
F 0 "U1" H 3450 1700 50  0000 C CNN
F 1 "LM393" H 3550 1400 50  0000 C CNN
F 2 "" H 3300 1550 50  0000 C CNN
F 3 "" H 3300 1550 50  0000 C CNN
	1    3300 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 900  2000 1050
Connection ~ 2000 900 
Wire Wire Line
	2000 1350 2000 1650
Wire Wire Line
	1850 1650 2250 1650
Wire Wire Line
	3000 900  3000 1450
$Comp
L R R4
U 1 1 570732CC
P 2600 1800
F 0 "R4" V 2680 1800 50  0000 C CNN
F 1 "5.1k" V 2600 1800 50  0000 C CNN
F 2 "" V 2530 1800 50  0000 C CNN
F 3 "" H 2600 1800 50  0000 C CNN
	1    2600 1800
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 57073370
P 2600 1400
F 0 "R3" V 2680 1400 50  0000 C CNN
F 1 "4.7k" V 2600 1400 50  0000 C CNN
F 2 "" V 2530 1400 50  0000 C CNN
F 3 "" H 2600 1400 50  0000 C CNN
	1    2600 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 600  2600 1250
Wire Wire Line
	2600 1550 2600 1650
Connection ~ 2600 1650
Wire Wire Line
	2600 1650 3000 1650
Wire Wire Line
	3200 2000 3200 1850
Connection ~ 2600 2000
Wire Wire Line
	3200 600  3200 1250
Connection ~ 2600 600 
$Comp
L R R5
U 1 1 57073B48
P 3600 1100
F 0 "R5" V 3680 1100 50  0000 C CNN
F 1 "1k" V 3600 1100 50  0000 C CNN
F 2 "" V 3530 1100 50  0000 C CNN
F 3 "" H 3600 1100 50  0000 C CNN
	1    3600 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 600  3600 950 
Connection ~ 3200 600 
Wire Wire Line
	3600 1250 3600 1550
Text GLabel 3750 1550 2    60   Output ~ 0
CAM_BIN
Wire Wire Line
	3600 1550 3750 1550
Connection ~ 3600 1550
Wire Wire Line
	2250 2000 3200 2000
Connection ~ 2250 2000
Wire Wire Line
	2600 1950 2600 2000
Wire Wire Line
	2250 2150 2250 1950
$Comp
L GND-RESCUE-Design #PWR02
U 1 1 57075378
P 2250 2150
F 0 "#PWR02" H 2250 2150 30  0001 C CNN
F 1 "GND" H 2250 2080 30  0001 C CNN
F 2 "" H 2250 2150 60  0000 C CNN
F 3 "" H 2250 2150 60  0000 C CNN
	1    2250 2150
	1    0    0    -1  
$EndComp
$EndSCHEMATC
