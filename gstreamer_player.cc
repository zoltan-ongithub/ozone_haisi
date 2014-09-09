#include "gstreamer_player.h"
#include <gst/gst.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>

   
typedef struct _CustomData {
  GstElement *playbin;
  GstElement *source;
  GstElement *sink;
  bool playing;
  GstBus *bus;
  bool stop;
  pthread_t threadid;
} CustomData;
   
/* playbin flags */
typedef enum {
  GST_PLAY_FLAG_VIDEO         = (1 << 0), /* We want video output */
  GST_PLAY_FLAG_AUDIO         = (1 << 1), /* We want audio output */
  GST_PLAY_FLAG_TEXT          = (1 << 2)  /* We want subtitle output */
} GstPlayFlags;

static gboolean gstPlayer_handle_message (GstBus *bus, GstMessage *msg, gpointer data0)
{  
  CustomData *data = (CustomData *)data0;

  g_print("gstPlayer_handle_message msg=%d,%s \n", GST_MESSAGE_TYPE(msg), GST_MESSAGE_TYPE_NAME(msg));

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR: {
      GError *err;
      gchar *debug;
      
      gst_message_parse_error (msg, &err, &debug);
      g_print ("handle_message:ERROR: %s\n", err->message);
      g_error_free (err);
      g_free (debug);
      
      gst_element_set_state (data->playbin, GST_STATE_READY);
      data->stop=true;
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      g_print("handle_message:EOS\n");
      gst_element_set_state (data->playbin, GST_STATE_READY);
      data->stop=true;
      break;
    case GST_MESSAGE_BUFFERING: {
      gint percent = 0;
      
      /* If the stream is live, we do not care about buffering. */
      if (data->playing) break;
      
      gst_message_parse_buffering (msg, &percent);
      g_print("handle_message:BUFFERING(%3d%%)\n", percent);
      /* Wait until buffering is complete before start/resume playing */
      if (percent < 100)
        gst_element_set_state (data->playbin, GST_STATE_PAUSED);
      else
        gst_element_set_state (data->playbin, GST_STATE_PLAYING);
      break;
    }
    case GST_MESSAGE_CLOCK_LOST:
      /* Get a new clock */
      g_print("handle_message:CLOCK_LOST\n");
      gst_element_set_state (data->playbin, GST_STATE_PAUSED);
      gst_element_set_state (data->playbin, GST_STATE_PLAYING);
      break;
    case GST_MESSAGE_STATE_CHANGED:      
      GstState old_state, new_state, pending_state;
      gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
      if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->playbin)) {
        g_print("handle_message:STATE_CHANGED %s to %s:\n",
            gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
        data->playing = (new_state == GST_STATE_PLAYING);
      }       
      break;
    default:
      /* Unhandled message */
      break;
    }
    
    return true;
}

void *gstPlayer_loopThread(void *arg)
{
  g_print("---gstPlayer_loopThread start\n");
  CustomData *data = (CustomData *)arg;
  data->stop = false;
  GstMessage *msg;   
   while (!data->stop) {
      // Call your bus message handler
      if(msg = gst_bus_pop (data->bus))
      {
          gstPlayer_handle_message (data->bus, msg, data);
          gst_message_unref (msg);
      }
      usleep(100);
   }  
   
   if(data->playbin != NULL)
   {
       gst_element_set_state (data->playbin, GST_STATE_NULL);
       gst_object_unref (data->playbin);
       data->playbin=NULL;
   }
  g_print("---gstPlayer_loopThread end\n");
  return NULL;
}
///////////////////////////////////////////////////////////////////////////
void * gstPlayer_create()
{
  g_print("---gstPlayer_create\n");
  CustomData *data = new CustomData;
  memset (data,0,sizeof(CustomData));
  return data;
}

bool gstPlayer_play(void *gstData, const char * url)
{
  g_print("---gstPlayer_play %s \n",url);
  CustomData *data = (CustomData *)gstData; 
  gint flags;
   
  /* Initialize GStreamer */
  gst_init (NULL, NULL);

  /* Create the elements */
  data->playbin = gst_pipeline_new("player");
  
  /* Create source & sink for ts player */
  data->source=gst_element_factory_make ("souphttpsrc", "souphttpsrc");
  data->sink=gst_element_factory_make ("playersinkbin", "playersinkbin");

  if (!data->playbin || !data->source || !data->sink) {
    g_printerr ("Not all elements could be created.\n");
    return false;
  }

  gst_bin_add_many(GST_BIN(data->playbin), data->source, data->sink, NULL);

  if (!gst_element_link_many(data->source, data->sink, NULL))
  {
    g_printerr("Failed to link source and sink elements\n");
    return false;
  }

  /* Set the URI to play */
  g_object_set (data->source, "location", url, NULL);
   
  /* Add a bus watch, so we get notified when a message arrives */
  data->bus = gst_pipeline_get_bus(GST_PIPELINE(data->playbin));
  gst_bus_add_watch(data->bus, gstPlayer_handle_message, data);
    
  GstStateChangeReturn ret = gst_element_set_state (data->playbin, GST_STATE_READY);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the ready state.\n");
    return false;
  }
 
  ret = gst_element_set_state (data->playbin, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    return false;
  }

 #if 0
   // Create a GLib Main Loop and set it to run 
  int err = pthread_create(&(data->threadid), NULL, gstPlayer_loopThread, data);
  if (err != 0)
  {
      g_printerr("can't create thread: %s\n", strerror(err));
      return false;
  }
  #endif
  return true;
}
void gstPlayer_playPause(void * gstData)
{
    CustomData *data = (CustomData *)gstData;
    GstStateChangeReturn ret;
    if(data->playing)
    {
        g_print("---gstPlayer_playPause set PAUSED\n");
        ret = gst_element_set_state (data->playbin, GST_STATE_PAUSED);
    }
    else
    {
        g_print("---gstPlayer_playPause set PLAYING\n");
        ret = gst_element_set_state (data->playbin, GST_STATE_PLAYING);
    }
    
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        return;
    }
}
bool gstPlayer_isplay(void * gstData)
{
   if(gstData==NULL)
   {
      return false;
   }
   CustomData *data = (CustomData *)gstData; 
   g_print("---gstPlayer_isplay %d\n",!data->stop);
   return  !data->stop;
}

bool gstPlayer_setwindow(void * gstData, int x, int y, int width, int height)
{
   char rect[20];

   if(gstData==NULL)
   {
      return false;
   }

   CustomData *data = (CustomData *)gstData;
   sprintf(rect, "%d,%d,%d,%d", x, y, width, height );
   g_object_set(data->sink, "rectangle", rect, NULL);
}

void gstPlayer_release(void ** gstData)
{
    if(gstData==NULL || *gstData==NULL)
    {
        return;
    }
    g_print("---gstPlayer_release\n");
    CustomData *data = (CustomData *)*gstData;   
    data->stop=true;   
    if(data->threadid != 0)
    {
       pthread_join(data->threadid, NULL);
       data->threadid = 0; 
    } 
    if(data->bus != NULL)
    {
      gst_object_unref (data->bus);
      data->bus = NULL;
    }

    if(data->playbin != NULL)
    {
       gst_element_set_state (data->playbin, GST_STATE_NULL);
       gst_object_unref (data->playbin);
       data->playbin=NULL;
    }

    delete data;
    *gstData=NULL; 
}



