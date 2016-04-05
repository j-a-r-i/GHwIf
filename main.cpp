#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "btooth.h"
#include "sensors.h"

#define PORT 8001
#define BLOCK_SIZE 100

GOutputStream* ostream = NULL;

//using namespace std;

//------------------------------------------------------------------------------
class ConnData {
public:
    GSocketConnection *connection;
    char message[BLOCK_SIZE];
};

//------------------------------------------------------------------------------
class Measures {
public:
  int counter;
  int t1;
  int t2;
  int t3;

  char buffer[100];

  Measures() :
    counter(0),
    t1(0),
    t2(1),
    t3(2)
  {}

  void update() {
    counter++;
    if (counter > 0xFFFF)
      counter = 0;

    int d = rand() % 4;
    d -= 2;

    t1 += d;
  }
  
  char *toJson() {
    sprintf(buffer, "{\"c\":%d, \"t1\":%d, \"t2\":%d, \"t3\":%d}", counter, t1, t2, t3);
    return buffer;
  }
};

Measures gMeasures;
  
//------------------------------------------------------------------------------
void onWritten(GObject* srcObject,
	       GAsyncResult *res,
	       gpointer userData)
{
    GError *err = NULL;
    int count;

    count = g_output_stream_write_finish(ostream,
					 res,
					 &err);
    if (count == -1) {
	g_error ("Error when writing message");
	if (err != NULL) {
	    g_error("%s", err->message);
	    g_clear_error(&err);
	}
	return;
    }
    if (count == 0) {
	g_print("write connection closed");
	return;
    }
}

//------------------------------------------------------------------------------
gboolean OnTimer(gpointer data)
{
    //const char *msg = "{'T':10}";
    char *msg = gMeasures.toJson();
    
    g_print("timer\n");

    gMeasures.update();
    
    if (ostream != NULL) {
	// start async write
	//
	g_output_stream_write_async(ostream,
				    msg,
				    strlen(msg),
				    G_PRIORITY_DEFAULT,
				    NULL,
				    onWritten,
				    data);
    }
    return TRUE;
}

//------------------------------------------------------------------------------
void onMessage(GObject* source_object,
	       GAsyncResult *res,
	       gpointer user_data)
{
    GInputStream *istream = G_INPUT_STREAM (source_object);
    GError *err = NULL;
    ConnData *data = (ConnData*)user_data;
    int count;

    count = g_input_stream_read_finish(istream,
				       res,
				       &err);
    if (count == -1) {
	g_error ("Error when receiving message");
	if (err != NULL) {
	    g_error("%s", err->message);
	    g_clear_error(&err);
	}
	ostream = NULL;
	g_object_unref (G_SOCKET_CONNECTION (data->connection));
	delete data;
	return;
    }
    if (count == 0) {
	g_print("connection closed");
	ostream = NULL;
	g_object_unref (G_SOCKET_CONNECTION (data->connection));
	delete data;
	return;
    }
    data->message[count] = 0;
    g_print("Message: \"%s\"\n", data->message);

    // start new read
    //
    g_input_stream_read_async(istream,
			      data->message,
			      sizeof (data->message),
			      G_PRIORITY_DEFAULT,
			      NULL,
			      onMessage,
			      data);
}

//------------------------------------------------------------------------------
gboolean onConnection(GSocketService *server,
		      GSocketConnection *connection,
		      GObject *sourceObject,
		      gpointer userData)
{
    g_print("connection\n");
	
    GInputStream *istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));   
    ConnData *data = new ConnData;

    ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

    data->connection = (GSocketConnection*)g_object_ref(connection);

    g_input_stream_read_async(istream,
			      data->message,
			      sizeof (data->message),
			      G_PRIORITY_DEFAULT,
			      NULL,
			      onMessage,
			      data);
    return FALSE;
}

//------------------------------------------------------------------------------
gboolean onEvent(GIOChannel *source,
		 GIOCondition condition,
		 gpointer data)
{
    g_print("event %d\n", condition);
    
    return TRUE;
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    GMainLoop *loop;
    GError * err = NULL;

    // Sensors
    Sensors s;

    s.scan();
    

    // Bluetooth scan
    //
    Bluetooth b;
    b.scan();

#if 0
    GIOChannel *channel = g_io_channel_unix_new(b.Handle());
    g_io_add_watch(channel,
		   (GIOCondition)(G_IO_IN | G_IO_OUT | G_IO_ERR | G_IO_HUP),
		   onEvent,
		   channel);
#endif
    
    //b.close();

    
    // socket server
    //
    GSocketService * server = g_socket_service_new();

    g_socket_listener_add_inet_port((GSocketListener*)server,
				    PORT,
				    NULL,
				    &err);
    if (err != NULL) {
	g_error("ERROR: %s", err->message);
    }

    g_signal_connect (server, "incoming", G_CALLBACK(onConnection), NULL);

    g_socket_service_start(server);

    // mainloop and timer
    //
    loop = g_main_loop_new ( NULL , FALSE );

    g_timeout_add (5000 , OnTimer , loop); 
    g_main_loop_run (loop);
    g_main_loop_unref(loop);

    return 0;
}
