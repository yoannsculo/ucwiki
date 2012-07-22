CC = gcc
BUILD_PATH = build
EXEC = $(BUILD_PATH)/ucwiki
SRC = $(wildcard src/*.c) 
OBJ = $(SRC:src/%.c=$(BUILD_PATH)/%.o)

INCLUDES = -Iinc \
	   -I/usr/include \
	   -I./tanoku-sundown-11d2add/src \
	   -I./tanoku-sundown-11d2add/html \
	   -I./ctemplate-1.0

CFLAGS = -W -Wall -g
LDFLAGS = -L/usr/lib -L$(BUILD_PATH) -lsundown

all: prepare sundown $(EXEC)

prepare:
	mkdir -p $(BUILD_PATH)

$(BUILD_PATH)/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

sundown:
	make -C tanoku-sundown-11d2add
	cp tanoku-sundown-11d2add/libsundown.so* $(BUILD_PATH)

clean:
	-@(rm -f $(EXEC) $(OBJ))
	make -C tanoku-sundown-11d2add clean
	rm $(BUILD_PATH)/libsundown*
