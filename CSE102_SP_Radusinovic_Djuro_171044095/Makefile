#Files to be compiled
OBJS = init.c init.h program.c loadGame.h loadGame.c movement.h movement.c LList.h LList.c
#CC - compiler
CC = gcc
#FLAGS - provides additional comilation info
FLAGS = -Wall -Wextra -Winit-self
#LINKERS - libraries that have to be linked
LINKERS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
#executable file's name
OBJ_NAME = program
#ansi check
ANSI = -ansi
all: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) $(LINKERS) -o $(OBJ_NAME) $(ANSI)
