#!/bin/bash

#Take a folder of 60 FPS video and re-encode it as an AVI, as well as make slow motion versions of it.

for f in *.MTS; do
	mencoder $f -o $f.fast.avi -oac pcm -ovc lavc  -lavcopts vcodec=mpeg4:vbitrate=15000
	mencoder $f -o $f.slow.avi -oac pcm -ovc lavc -fps 60 -ofps 30 -speed 0.5 -lavcopts vcodec=mpeg4:vbitrate=15000
	mencoder $f -o $f.rslow.avi -oac pcm -ovc lavc -fps 60 -ofps 15 -speed 0.25 -lavcopts vcodec=mpeg4:vbitrate=9000
done
