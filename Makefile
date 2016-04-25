PROG = bindump
CFLAGS = -g -O2 -Wall -std=c99
# CFLAGS = -std=c99 -g -O2 -Wall
# LDLIBS += -L/usr/local/lib -lmylib

all: $(PROG)
# OBJS += $(PROG).o
# $(PROG): $(OBJS)

clean:
	rm -f *.o $(PROG)
