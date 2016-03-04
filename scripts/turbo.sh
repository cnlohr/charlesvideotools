
i=`ls *.MP4`

for k in $i; do
#	ffmpeg -i $k -s 1280x720 -an -vcodec mpeg4 -vpre vhq $k.mpg
	mencoder -o $k.avi $k -fps 30 -speed 10 -ofps 30 -oac mp3lame -srate 44100 -vf scale=1280:720 -ovc xvid -xvidencopts fixed_quant=2
done


