
CC := gcc
CPP := g++ -Wno-deprecated -g
SRC_DIR := src
OBJ_DIR := target
GTK := gtk+-3.0
CFLAGS := $(shell pkg-config --cflags $(GTK)) -Wall -I include
LDFLAGS := $(shell pkg-config --libs $(GTK))
EXE := sukeban

SRCS := include/img.h $(SRC_DIR)/main.cpp
OBJS := $(OBJ_DIR)/main.o

all: $(EXE)

include/img.h: image/suke_icon_500.png
	mkdir -p include
	gdk-pixbuf-csource --raw --name=suke_icon --static $< > $@

include/icon.h: image/suke_icon.png
	mkdir -p include
	gdk-pixbuf-csource --raw --name=suke_icon_mini --static $< > $@

TAGS:
	find $(shell pkg-config --cflags-only-I $(GTK) | sed s/-I//g) -name "*.h" | xargs etags

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(EXE): include/img.h include/icon.h $(OBJS)
	$(CPP) $(OBJS) $(LDFLAGS) -o $(EXE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if [ ! -d $(OBJ_DIR) ]; \
		then mkdir -p $(OBJ_DIR); \
	fi
	$(CPP) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXE) $(OBJ_DIR) include/img.h TAGS

.PHONY: all clean
