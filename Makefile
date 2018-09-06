################################################################################
# Copyright (C) 2016-8 Jari Ojanen
################################################################################

#SDKTARGETSYSROOT=/mnt/buildroot/output/host
#CXX=arm-buildroot-linux-gnueabihf-c++

CXX=g++

CC     = $(CXX)
#CFLAGS = -g -std=c++14 -Wall $(shell pkg-config --cflags glib-2.0 gio-2.0 bluez)
#LIBS   = $(shell pkg-config --libs glib-2.0 gio-2.0 bluez)
CFLAGS = -g -std=c++11 -Wall -I.
LIBS   = -luv
LIBS  += -lsensors
#LIBS  += -lbluetooth
LIBS  += -lcurl

TARGET2=hwif

# btooth.o

OBJECTS2  = main2.o serial.o Socket.o measures.o
OBJECTS2 += sensors.o
#OBJECTS2 += btooth.o
OBJECTS2 += web.o
OBJECTS2 += disk.o
OBJECTS2 += rpi/gpio.o

OBJ_UTEST = test/tdisk.o
OBJ_UTEST += disk.o


all: $(TARGET2)

utest: $(OBJ_UTEST)
	$(CC) $(CFLAGS) -I. -L$(SDKTARGETSYSROOT)/usr/bin -o utest $(OBJ_UTEST) $(LIBS) -lgtest -lpthread

$(TARGET2): $(OBJECTS2)  
	$(CC) $(CFLAGS) -L$(SDKTARGETSYSROOT)/usr/bin -o $(TARGET2) $(OBJECTS2) $(LIBS)

main.o:  main.cpp btooth.h serial.h Socket.h
	$(CC) $(CFLAGS) -c $<

main2.o:  main2.cpp serial.h Socket.h measures.h
	$(CC) $(CFLAGS) -c $<

btooth.o:  btooth.cpp btooth.h
	$(CC) $(CFLAGS) -c $<

serial.o:  serial.cpp serial.h
	$(CC) $(CFLAGS) -c $<

sensors.o:  sensors.cpp sensors.h infoitem.h
	$(CC) $(CFLAGS) -c $<

web.o: web.cpp web.h infoitem.h
	$(CC) $(CFLAGS) -c $<

disk.o: disk.cpp disk.h infoitem.h
	$(CC) $(CFLAGS) -c $<

Socket.o:  Socket.cpp Socket.h
	$(CC) $(CFLAGS) -c $<

measures.o:  measures.cpp measures.h
	$(CC) $(CFLAGS) -c $<

clean: 
	$(RM) $(TARGET) *.o *~
