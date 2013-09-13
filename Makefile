CFLAGS = -g -O

CFLAGS += -Wall -Wextra

override CFLAGS += -std=gnu99

.PHONY: all
all: esh savetty

esh: esh.o input.o input-impl.o
esh.o: esh.c input.h
input.o: input.c input.h input-impl.h mem.h
input-impl.o: input-impl.c input-impl.h mem.h

savetty: savetty.o
savetty.o: savetty.c

.PHONY: clean
clean:
	rm -f esh *.o
