# The make file should sit in the root directory of the projecy.
CC = g++
CPPFLAGS = -Wall -Werror -g -std=c++11 -pedantic
PROJECT_ROOT := $(shell readlink $(dir $(lastword $(MAKEFILE_LIST))) -f)
BIN = $(PROJECT_ROOT)/bin
SRC = $(PROJECT_ROOT)/src
INCFLAG = -I$(SRC)
SERVER_DIR = $(SRC)/server
ECHO_SERVER_DIR = $(SRC)/echoServer
PUBLISH_SUBSCRIBE_SERVER_DIR = $(SRC)/publishSubscribeServer
EXTERNAL_DIR = $(SRC)/external

SERVER_SRC = $(shell find $(SERVER_DIR) -type f -name "*.cpp")
SERVER_HEADER = $(shell find $(SERVER_DIR) -type f -name "*.h")
SERVER_OBJS = $(SERVER_SRC:.cpp=.o)
EXTERNAL_SRC = $(shell find $(EXTERNAL_DIR) -type f -name "*.cpp")
EXTERNAL_HEADER = $(shell find $(EXTERNAL_DIR) -type f -name "*.h")
EXTERNAL_OBJS = $(EXTERNAL_SRC:.cpp=.o)
ECHO_SERVER = $(BIN)/echoServer
ECHO_SERVER_SRC = $(shell find $(ECHO_SERVER_DIR) -type f -name "*.cpp")
ECHO_SERVER_HEADER = $(shell find $(ECHO_SERVER_DIR) -type f -name "*.h")
ECHO_SERVER_OBJS = $(ECHO_SERVER_SRC:.cpp=.o)
PUBLISH_SUBSCRIBE_SERVER = $(BIN)/publishSubscribeServer
PUBLISH_SUBSCRIBE_SERVER_SRC = $(shell find $(PUBLISH_SUBSCRIBE_SERVER_DIR) -type f -name "*.cpp")
PUBLISH_SUBSCRIBE_SERVER_HEADER = $(shell find $(PUBLISH_SUBSCRIBE_SERVER_DIR) -type f -name "*.h")
PUBLISH_SUBSCRIBE_SERVER_OBJS = $(PUBLISH_SUBSCRIBE_SERVER_SRC:.cpp=.o)

all: $(BIN) $(ECHO_SERVER) $(PUBLISH_SUBSCRIBE_SERVER)

$(BIN):
	mkdir -p $(BIN)

$(PUBLISH_SUBSCRIBE_SERVER): $(SERVER_OBJS) $(EXTERNAL_OBJS) $(PUBLISH_SUBSCRIBE_SERVER_OBJS)
	$(CC) $(CPPFLAGS) $(INCFLAG) $(PUBLISH_SUBSCRIBE_SERVER_OBJS) $(SERVER_OBJS) -o $@

$(ECHO_SERVER): $(SERVER_OBJS) $(EXTERNAL_OBJS) $(ECHO_SERVER_OBJS)
	$(CC) $(CPPFLAGS) $(INCFLAG) $(ECHO_SERVER_OBJS) $(SERVER_OBJS) -o $@

$(PUBLISH_SUBSCRIBE_SERVER_OBJS): %.o: %.cpp $(PUBLISH_SUBSCRIBE_SERVER_HEADER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $<  -o $@

$(ECHO_SERVER_OBJS): %.o: %.cpp $(ECHO_SERVER_HEADER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $<  -o $@

$(SERVER_OBJS): %.o: %.cpp $(SERVER_HEADER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $<  -o $@

$(EXTERNAL_OBJS): %.o: %.cpp $(EXTERNAL_HEADER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $<  -o $@
clean:
	rm -rf $(BIN)
	rm -rf $(PUBLISH_SUBSCRIBE_SERVER_OBJS)
	rm -rf $(ECHO_SERVER_OBJS)

.PHONY: all clean

