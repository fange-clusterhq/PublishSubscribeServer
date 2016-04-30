# The make file should sit in the root directory of the projecy.
SRC = src
CC = g++
CPPFLAGS = -Wall -Werror -g -std=c++11 -pedantic

SERVER = server
SERVER_SRC = $(SRC)/main.cpp
SERVER_SRC += $(SRC)/server.cpp
SERVER_OBJS = $(SERVER_SRC:.cpp=.o)

default: $(SERVER)

$(SERVER): $(SERVER_OBJS)
	$(CC) $(CPPFLAGS) $(SERVER_OBJS) -o $(SERVER)

.c.o:
	$(CC) $(CPPFLAGS) -c $<  -o $@

clean:
	rm -rf $(SERVER) *.o *~ $(SRC)/*.o

