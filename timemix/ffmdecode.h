#ifndef _FFMDECODE_H
#define _FFMDECODE_H

void setup_video_decode();
int video_decode( const char *filename);
int got_video_frame( const unsigned char * rgbbuffer, int linesize, int width, int height, int frameno ); //Return nonzero to stop.

//If you need it, you can extern AVCodecContext *dec_ctx;


#endif

