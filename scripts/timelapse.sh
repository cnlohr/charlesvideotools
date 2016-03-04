#mencoder -ovc lavc mf://*.JPG  -mf w=1920:h=1080:fps=30:type=jpg -lavcopts vcodec=mpeg1video:vbitrate=30000000 -vf scale=1920:1080 -o test3_mpeg1.avi
#harddup on vf if you want all frames to be written.

#this also corrects for aspect ratio issues.
#mencoder -ovc lavc mf://*.JPG  -mf fps=30:type=jpg -lavcopts vcodec=mpeg1video:vbitrate=30000000 -vf scale=1920:1276,crop=1920:1080 -o test4c_mpeg1.avi
mencoder -ovc lavc mf://*.JPG  -mf fps=30:type=jpg -lavcopts vcodec=mpeg4:vbitrate=50000000 -vf scale=1920:1276,crop=1920:1080 -o test4c_mpeg4.avi
#mencoder -ovc lavc mf://*.JPG  -mf fps=30:type=jpg -lavcopts vcodec=mpeg2video:vbitrate=30000000 -vf scale=1920:1276,crop=1920:1080 -o test4c_mpeg2.avi
#mencoder -ovc x264 mf://*.JPG  -mf fps=30:type=jpg -x264encopts bitrate=40000 -vf scale=1920:1276,crop=1920:1080 -o test4c_x264.avi
