
CC = gcc
SRC_DIR = src
OBJ_DIR = target
CFLAGS = $(shell pkg-config --cflags gtk+-3.0) -Wall
LDFLAGS = $(shell pkg-config --libs gtk+-3.0)
EXE = mini

SRCS = $(SRC_DIR)/mini.c
OBJS = $(OBJ_DIR)/mini.o

all: $(EXE)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(EXE): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(EXE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@if [ ! -d $(OBJ_DIR) ]; \
		then mkdir -p $(OBJ_DIR); \
	fi
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXE) $(OBJ_DIR)

.PHONY: all clean
