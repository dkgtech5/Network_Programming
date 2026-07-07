# =========================================
# Remote Command Execution System
# Makefile
# =========================================

CC = gcc
CFLAGS = -Wall

SERVER = server
CLIENT = client

SERVER_SRC = server.c command.c auth.c logger.c
CLIENT_SRC = client.c

all: $(SERVER) $(CLIENT)

$(SERVER):
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $(SERVER)

$(CLIENT):
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $(CLIENT)

run-server:
	./$(SERVER)

run-client:
	./$(CLIENT)

clean:
	rm -f $(SERVER) $(CLIENT)

rebuild: clean all