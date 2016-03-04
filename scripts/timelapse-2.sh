#!/bin/bash

for f in `ls *.MP4`; do
	mencoder $f -o $f.s10.mpg -oac pcm -ovc lavc -fps 30 -ofps 30 -speed 10 -lavcopts vcodec=mpeg4:vbitrate=15000:autoaspect
#	mencoder $f -o $f.fast.avi -oac pcm -ovc lavc  -lavcopts vcodec=mpeg4:vbitrate=15000
#	mencoder $f -o $f.rslow.avi -oac pcm -ovc lavc -fps 60 -ofps 15 -speed 0.25 -lavcopts vcodec=mpeg4:vbitrate=9000
#	 exit
done
