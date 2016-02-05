CC     = $(CXX)
CFLAGS = -g -Wall $(shell pkg-config --cflags glib-2.0 gio-2.0 bluez)
LIBS   = $(shell pkg-config --libs glib-2.0 gio-2.0 bluez)


TARGET=ghwif


all: $(TARGET)

$(TARGET):  main.o btooth.o serial.o
	$(CC) $(CFLAGS) -L$(SDKTARGETSYSROOT)/usr/bin -o $(TARGET) main.o btooth.o serial.o $(LIBS)

main.o:  main.cpp btooth.h serial.h
	$(CC) $(CFLAGS) -c $<

btooth.o:  btooth.cpp btooth.h
	$(CC) $(CFLAGS) -c $<

serial.o:  serial.cpp serial.h
	$(CC) $(CFLAGS) -c $<

clean: 
	$(RM) $(TARGET) *.o *~
