#SDKTARGETSYSROOT=/mnt/buildroot/output/host
#CXX=arm-buildroot-linux-gnueabihf-c++

CXX=g++

CC     = $(CXX)
#CFLAGS = -g -std=c++14 -Wall $(shell pkg-config --cflags glib-2.0 gio-2.0 bluez)
#LIBS   = $(shell pkg-config --libs glib-2.0 gio-2.0 bluez) -lsensors
CFLAGS = -g -std=c++11 -Wall
LIBS   = -luv -lbluetooth

TARGET=ghwif
TARGET2=hwif

OBJECTS2=main2.o serial.o Socket.o measures.o btooth.o

all: $(TARGET2)

$(TARGET):  main.o btooth.o serial.o sensors.o
	$(CC) $(CFLAGS) -L$(SDKTARGETSYSROOT)/usr/bin -o $(TARGET) main.o btooth.o serial.o sensors.o $(LIBS)

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

sensors.o:  sensors.cpp sensors.h
	$(CC) $(CFLAGS) -c $<

Socket.o:  Socket.cpp Socket.h
	$(CC) $(CFLAGS) -c $<

measures.o:  measures.cpp measures.h
	$(CC) $(CFLAGS) -c $<

clean: 
	$(RM) $(TARGET) *.o *~
