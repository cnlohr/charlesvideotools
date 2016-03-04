#!/bin/bash
#avconv -video_size 720x480 -framerate 30 -f x11grab -i :0.0+0,0   -vcodec mpeg2video \
#	-b 15000k -qmin 1 -qmax 1 $1

avconv -f x11grab -r 30 -s 854x480 -i :0.0+0,0 -vcodec libx264 -pre lossless_ultrafast -threads 0 $1

