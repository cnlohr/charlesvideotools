all : testprog standalone boxer

CFLAGS:=-O2 -g
LDFLAGS:=
LDLIBS:=-lX11 -lXext -lpthread -lXinerama

standalone : standalone.o
	g++ -o $@ $^ $(LDFLAGS) $(LDLIBS)

testprog : testprog.o xosd.o
	gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)

boxer : boxer.o
	g++ -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -rf *.o *~ testprog standalone
