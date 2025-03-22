CC = cl65
CFLAGS = -t pet -O
LDFLAGS = -t pet

SOURCES = src/main.c
TARGET = game.prg

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o 