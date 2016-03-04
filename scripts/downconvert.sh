#For downconverting from Sony's .mts to 30 FPS AVIs.

i=`ls *.MTS`

for k in $i; do
#	ffmpeg -i $k -s 1280x720 -an -vcodec mpeg4 -vpre vhq $k.mpg
	mencoder -o $k.avi $k -fps 60 -ofps 30 -oac mp3lame -vf scale=1280:720 -ovc xvid -xvidencopts fixed_quant=2
done


