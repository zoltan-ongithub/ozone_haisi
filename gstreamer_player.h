#ifndef __GSTREAMER_PLAYER_H__
#define __GSTREAMER_PLAYER_H__

 
void * gstPlayer_create();
bool gstPlayer_play(void *gstData, const char * url);
void gstPlayer_playPause(void * gstData);
bool gstPlayer_isplay(void * gstData);
void gstPlayer_release(void ** gstData);
bool gstPlayer_setwindow(void * gstData, int x, int y, int width, int height);

#endif /* __GSTREAMER_PLAYER_H__*/
