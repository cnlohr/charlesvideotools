#!/bin/bash
#avconv -r 30 -f alsa -i pulse -video_size 1280x720 -f x11grab -framerate 30 -i :0.0 \
#	 -ab 320k -b 15000k -qmin 1 -qmax 1 -y $1

avconv -f x11grab -r 30 -s 1280x720 -i :0.0+0,0 -vcodec libx264 -pre lossless_ultrafast -threads 0 $1

