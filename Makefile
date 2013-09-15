CFLAGS = -g -O

CFLAGS += -Werror=all -Werror=extra -Werror=format -Werror=unused
CFLAGS += -Werror=redundant-decls -Werror=missing-declarations

override CFLAGS += -std=gnu99 -D_GNU_SOURCE

.PHONY: all
all: esh savetty

esh: esh.o io.o input-impl.o spawn.o
esh.o: esh.c io.h spawn.h
io.o: io.c io.h input-impl.h mem.h
input-impl.o: input-impl.c input-impl.h mem.h
spawn.o: spawn.c spawn.h

savetty: savetty.o
savetty.o: savetty.c

.PHONY: clean
clean:
	rm -f esh *.o
