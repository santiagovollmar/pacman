#!/bin/bash

gcc \
src/event_handlers.c \
src/main.c \
src/path_finder.c \
src/pacman_gui.c \
src/sleep.c \
src/update.c \
src/colors.c \
src/skins.c \
src/list_utils/linked_list.c \
-lpthread -lncurses -lm -g -o pacman && ./pacman
