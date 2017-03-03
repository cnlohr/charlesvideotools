#include <stdio.h>
#include "ffmdecode.h"
#include <stdlib.h>
#include <string.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixfmt.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>


int gwidth;
int gheight;
int ofps;
int gframe = 0;
int maxframe = 0;
int speedup_ratio = 0;
int outframe = 0;
const char * outname;
		uint8_t outbuf[10000000];

///////////////////////////////For output

AVCodec *codec;
int got_output;
AVCodecContext *c= NULL;
AVFrame *frame;
AVPacket pkt;
FILE * f;
uint8_t endcode[] = { 0, 0, 1, 0xb7 };
struct SwsContext * ctx;
extern AVCodecContext *dec_ctx;

void setup_output( int codec_id )
{
	int i, ret, x, y, got_output;

    codec = avcodec_find_encoder(codec_id);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

	c->sample_aspect_ratio.num=dec_ctx->sample_aspect_ratio.num;
	c->sample_aspect_ratio.den=dec_ctx->sample_aspect_ratio.den;

	/* put sample parameters */
	/* resolution must be a multiple of two */
	c->width = gwidth;
	c->height = gheight;
	/* frames per second */
	c->time_base = (AVRational){1,ofps};

	printf( "OPS: %d\n", ofps );
	/* emit one intra frame every ten frames
	 * check frame pict_type before passing frame
	 * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
	 * then gop_size is ignored and the output of encoder
	 * will always be I frame irrespective to gop_size
	 */
	c->bit_rate = 19000000;
	c->gop_size = 10;
	c->global_quality = 1;
	c->max_b_frames = 1;

	printf( "TIMEBASE %d/%d\n", c->pkt_timebase.num, c->pkt_timebase.den );


	c->pix_fmt = AV_PIX_FMT_YUV420P; //PIX_FMT_RGB24;//AV_PIX_FMT_YUV420P;

    if (codec_id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);


    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
	}

    f = fopen(outname, "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", outname);
        exit(1);
    }


    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    ret = av_image_alloc(frame->data, frame->linesize, c->width, c->height,
                         c->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }

	ctx = sws_getContext(gwidth, gheight,
                                  AV_PIX_FMT_RGB24, gwidth, gheight,
                                  AV_PIX_FMT_YUV420P, 0, 0, 0, 0);

}

void finish_output()
{
	/* get the delayed frames */
	for (got_output = 1; got_output; gframe++) {
		fflush(stdout);
#define VER2
#ifdef VER2
		int ret = avcodec_encode_video2(c, &pkt, NULL, &got_output);
		if (ret < 0) {
		    fprintf(stderr, "Error encoding frame\n");
		    exit(1);
		}

		if (got_output) {
		    printf("Write frame %3d (size=%5d)\n", outframe, pkt.size);
		    fwrite(pkt.data, 1, pkt.size, f);
		    av_free_packet(&pkt);
		}

     #else
   fflush(stdout);
          got_output = avcodec_encode_video(c, outbuf, sizeof(outbuf), NULL);
         printf("write frame %3d (size=%5d)\n", outframe, got_output);
         fwrite(outbuf, 1, got_output, f);
		    av_free_packet(&pkt);
#endif
	}

	/* add sequence end code to have a real MPEG file */
	fwrite(endcode, 1, sizeof(endcode), f);
	fclose(f);

	avcodec_free_context(&c);
	av_freep(&frame->data[0]);
	av_frame_free(&frame);
	printf("\n");
}


///////////////////////////////
uint32_t * rgbaccumbuffer;

int got_video_frame( const unsigned char * rgbbuffer, int linesize, int width, int height, int frameno )
{
	int x, y, i, ret;
	gwidth = width; gheight = height;
	if( gframe++ == 0 )
	{
		setup_output(  AV_CODEC_ID_H264 );//AV_CODEC_ID_MPEG1VIDEO ); 
		rgbaccumbuffer = calloc( gwidth * gheight, 3 * 4 );
	}
	printf( "Frame %d\n", gframe );

	for( y = 0; y < gheight; y++ )
	for( x = 0; x < gwidth; x++ )
	{
		rgbaccumbuffer[(x+y*gwidth)*3+0] += rgbbuffer[(x+y*gwidth)*3+0];
		rgbaccumbuffer[(x+y*gwidth)*3+1] += rgbbuffer[(x+y*gwidth)*3+1];
		rgbaccumbuffer[(x+y*gwidth)*3+2] += rgbbuffer[(x+y*gwidth)*3+2];
	}

	if( gframe % speedup_ratio  == 0 )
	{
		uint8_t tmpbuff[gwidth*gheight*3];
		av_init_packet(&pkt);
		pkt.data = NULL;    // packet data will be allocated by the encoder
		pkt.size = 0;
		fflush(stdout);

		for( y = 0; y < gheight; y++ )
		for( x = 0; x < gwidth; x++ )
		{
			tmpbuff[(x+y*gwidth)*3+0] = (rgbaccumbuffer[(x+y*gwidth)*3+0] + speedup_ratio/2) / speedup_ratio;
			tmpbuff[(x+y*gwidth)*3+1] = (rgbaccumbuffer[(x+y*gwidth)*3+1] + speedup_ratio/2) / speedup_ratio;
			tmpbuff[(x+y*gwidth)*3+2] = (rgbaccumbuffer[(x+y*gwidth)*3+2] + speedup_ratio/2) / speedup_ratio;
		}
		memset( rgbaccumbuffer, 0, gwidth*gheight*4*3 );
		printf( "Writing frame\n" );
		const uint8_t * inData[1] = { tmpbuff }; // RGB24 have one plane
		int inLinesize[1] = { gwidth*3 }; // RGB stride
		sws_scale(ctx, inData, inLinesize, 0, gheight, frame->data, frame->linesize);
		frame->pts = outframe;// = av_frame_get_best_effort_timestamp(frame);
		printf( "OF: %d\n", outframe );
		outframe++;

#ifdef VER2
		/* encode the image */
		ret = avcodec_encode_video2(c, &pkt, frame, &got_output);

		//ret = avcodec_encode_video(c, outbuf, sizeof(outbuf), frame);
		//if( ret ) got_output = 1;

		if (ret < 0) {
		    fprintf(stderr, "Error encoding frame\n");
		    exit(1);
		}

		if (got_output) {

		    printf("Write frame %3d (size=%5d)\n", outframe, pkt.size);
		    fwrite(pkt.data, 1, pkt.size, f);
		    av_free_packet(&pkt);
		}
#else
		ret = avcodec_encode_video(c, outbuf, sizeof(outbuf), frame);

		if (ret) {

		    printf("Write frame %3d (size=%5d)\n", outframe, ret);
		    fwrite(outbuf, 1, ret, f);
		    av_free_packet(&pkt);
		}


#endif

	}
//	if( gframe > 400 ) return 1;
	maxframe = gframe;
	return 0;
}

int main( int argc, char ** argv )
{
	FILE * f;
	int line;
	setup_video_decode();
	if( argc < 4 )
	{
		fprintf( stderr, "Need Parameters:   [video name] [speedup ratio] [ofps] [outname]\n" );
		exit( -1 );
	}
	speedup_ratio = atoi( argv[2] );
	ofps = atoi( argv[3] );
	outname = argv[4];
	video_decode( argv[1] );
	finish_output();
	printf( "Done\n" );
}
