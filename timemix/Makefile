all : timemixlapse

CFLAGS:=-O1 -g

timemixlapse : timemixlapse.o ffmdecode.o
	gcc -o $@ $^ -lavcodec -lavformat -lavutil -lswscale -g

clean :
	rm -rf *.o *~ timemixlapse
