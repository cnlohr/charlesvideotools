#!/bin/bash

#60 FPS 480p frame grabbing.

avconv -f x11grab -r 60 -s 720x480 -i :0.0+0,0 -vcodec libx264 -pre lossless_ultrafast -threads 0 $1

