# makefile

all: dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	g++ -c -g reqchannel.cpp

dataserver: dataserver.cpp reqchannel.o 
	g++ -g -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp reqchannel.o
	g++ -g -o client client.cpp reqchannel.o -lpthread
	
clean: 
		rm -f *.o
		rm -f dataserver
		rm -f client
		rm -f fifo_unknown\ request1
		rm -f fifo_data1_1
		rm -f fifo_data1_2
		rm -f fifo_data2_1
		rm -f fifo_data2_2
		rm -f fifo_data3_1
		rm -f fifo_data3_2
		rm -f fifo_data4_1
		rm -f fifo_data4_2
		rm -f fifo_data5_1
		rm -f fifo_data5_2
		rm -f fifo_control1
		rm -f fifo_control2
		clear
