# The make file should sit in the root directory of the projecy.
CC = g++
CPPFLAGS = -Wall -Wextra -Werror -g -std=c++11 -pedantic
PROJECT_ROOT := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
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
PUBLISH_SUBSCRIBE_SERVER = $(BIN)/publishSubscribeServer
PUBLISH_SUBSCRIBE_SERVER_SRC = $(shell find $(PUBLISH_SUBSCRIBE_SERVER_DIR) -type f -name "*.cpp")
PUBLISH_SUBSCRIBE_SERVER_HEADER = $(shell find $(PUBLISH_SUBSCRIBE_SERVER_DIR) -type f -name "*.h")
PUBLISH_SUBSCRIBE_SERVER_OBJS = $(PUBLISH_SUBSCRIBE_SERVER_SRC:.cpp=.o)

all: $(BIN) $(PUBLISH_SUBSCRIBE_SERVER)

$(BIN):
	mkdir -p $(BIN)

$(PUBLISH_SUBSCRIBE_SERVER): $(SERVER_OBJS) $(EXTERNAL_OBJS) $(PUBLISH_SUBSCRIBE_SERVER_OBJS)
	$(CC) $(CPPFLAGS) $(INCFLAG) $(PUBLISH_SUBSCRIBE_SERVER_OBJS) $(EXTERNAL_OBJS) $(SERVER_OBJS) -o $@

$(PUBLISH_SUBSCRIBE_SERVER_OBJS): %.o: %.cpp $(PUBLISH_SUBSCRIBE_SERVER_HEADER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $<  -o $@

$(SERVER_OBJS): %.o: %.cpp $(SERVER_HEADER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $<  -o $@

$(EXTERNAL_OBJS): %.o: %.cpp $(EXTERNAL_HEADER)
	$(CC) $(CPPFLAGS) $(INCFLAG) -c $<  -o $@

clean:
	rm -rf $(BIN)
	rm -rf $(SERVER_OBJS)
	rm -rf $(EXTERNAL_OBJS)
	rm -rf $(PUBLISH_SUBSCRIBE_SERVER_OBJS)

.PHONY: all clean

