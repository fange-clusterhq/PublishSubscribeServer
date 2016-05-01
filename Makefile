# The make file should sit in the root directory of the projecy.
SRC = src
CC = g++
CPPFLAGS = -Wall -Werror -g -std=c++11 -pedantic

SERVER_SRC = $(SRC)/main.cpp
SERVER_SRC += $(SRC)/server.cpp
ECHO_SERVER = echoServer
ECHO_SERVER_SRC = $(SERVER_SRC)
ECHO_SERVER_SRC += $(SRC)/echoServer.cpp
ECHO_SERVER_OBJS = $(ECHO_SERVER_SRC:.cpp=.o)

default: $(ECHO_SERVER)

$(ECHO_SERVER): $(ECHO_SERVER_OBJS)
	$(CC) $(CPPFLAGS) $(ECHO_SERVER_OBJS) -o $(ECHO_SERVER)

.c.o:
	$(CC) $(CPPFLAGS) -c $<  -o $@

clean:
	rm -rf $(ECHO_SERVER) *.o *~ $(SRC)/*.o

