##
# vibesh
#
# @file
# @version 0.1
CC = clang
CFLAGS = -Wall -Wvla -Werror
VERSION = -std=c23
LIBS = -lcurl

################################################

default: all

gdb: CFLAGS += -g
gdb: all

################################################

all: bin vibesh

bin:
	mkdir -p ../bin

vibesh: main.c
	$(CC) $(CFLAGS) $(VERSION) $(LIBS) -o ../bin/vibesh main.c

###############################################

clean:
	rm ../bin/vibesh

# end
