CC = gcc
BUILD_PATH = build
EXEC = $(BUILD_PATH)/ucwiki
SD_PATH=tanoku-sundown-11d2add
SRC  = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=$(BUILD_PATH)/%.o) $(SD_PATH)/libsundown.a

INCLUDES = -Iinc \
	   -I/usr/include \
	   -I./$(SD_PATH)/src \
	   -I./$(SD_PATH)/html \
	   -I./ctemplate-1.0

CFLAGS = -W -Wall -g
LDFLAGS = -L/usr/lib -L$(BUILD_PATH)

all: prepare sundown $(EXEC)

prepare:
	mkdir -p $(BUILD_PATH)

$(BUILD_PATH)/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

sundown:
	make -C $(SD_PATH) libsundown.a

clean:
	-@(rm -f $(EXEC) $(OBJ))

clear: clean
	make -C $(SD_PATH) clean
