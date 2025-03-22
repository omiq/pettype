CC65_HOME = /usr/local/share/cc65
CC = cl65
CFLAGS = -t pet -O
LDFLAGS = -t pet -m game.map

SOURCES = src/main.c src/screen.c src/player.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: game.prg

game.prg: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f game.prg $(OBJECTS) game.map 