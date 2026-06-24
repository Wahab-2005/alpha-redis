CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11

SRC = main.c \
      p1_tableau/tableau.c \
      p2_hashage/hash.c \
      p3_collisions/liste_simple.c \
      p4_liste/liste_double.c \
      p5_cli/cli.c

OBJ = $(SRC:.c=.o)
TARGET = alpha-redis

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
