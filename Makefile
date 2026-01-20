CC = gcc
CFLAGS = -O2 -Wall

CFLAGS += $(shell pkg-config --cflags hunspell)
LDLIBS += $(shell pkg-config --libs hunspell)

huncolor: huncolor.o xdg_dirs.o
huncolor.o: huncolor.c
xdg_dirs.o: xdg_dirs.c

install: huncolor
	install huncolor $(HOME)/bin
