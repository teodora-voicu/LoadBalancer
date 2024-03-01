CC = gcc
CFLAGS = -Wall -Werror -g -ggdb3

# Source files
MAIN_SRC = main.c
LOAD_BALANCER_SRC = load_balancer.c
SERVER_SRC = server.c
HASHTABLE2_SRC = hashtable2.c

# Object files
MAIN_OBJ = $(MAIN_SRC:.c=.o)
LOAD_BALANCER_OBJ = $(LOAD_BALANCER_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o)
HASHTABLE2_OBJ = $(HASHTABLE2_SRC:.c=.o)

# Header files
LOAD_BALANCER_H = load_balancer.h
SERVER_H = server.h
HASHTABLE2_H = hashtable2.h

# Targets
build: tema2

tema2: $(MAIN_OBJ) $(LOAD_BALANCER_OBJ) $(SERVER_OBJ) $(HASHTABLE2_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(MAIN_OBJ): $(MAIN_SRC) $(LOAD_BALANCER_H) $(SERVER_H) $(HASHTABLE2_H)
	$(CC) $(CFLAGS) -c $<

$(LOAD_BALANCER_OBJ): $(LOAD_BALANCER_SRC) $(LOAD_BALANCER_H) $(SERVER_H) $(HASHTABLE2_H)
	$(CC) $(CFLAGS) -c $<

$(SERVER_OBJ): $(SERVER_SRC) $(SERVER_H) $(HASHTABLE2_H)
	$(CC) $(CFLAGS) -c $<

$(HASHTABLE2_OBJ): $(HASHTABLE2_SRC) $(HASHTABLE2_H)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(MAIN_OBJ) $(LOAD_BALANCER_OBJ) $(SERVER_OBJ) $(HASHTABLE2_OBJ) tema21 *.h.gch
