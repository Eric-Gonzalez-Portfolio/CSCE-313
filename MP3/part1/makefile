# makefile
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    cc = clang++
else ifeq ($(UNAME), Darwin)
    cc = g++
else
    cc = g++-4.7
endif

all: dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	$(cc) -std=c++11 -c -g reqchannel.cpp

dataserver: dataserver.cpp reqchannel.o 
	$(cc) -std=c++11 -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp reqchannel.o
	$(cc) -std=c++11 -o client client.cpp reqchannel.o
