clear all; close all; clc;
s = serial('/dev/cu.usbserial-A403A3MP','BaudRate',57600,'DataBits',8);


fopen(s);

idn = fscanf(s)

fclose(s);
