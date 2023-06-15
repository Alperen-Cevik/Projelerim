all: compile run

compile:
	@g++ server.cpp -lws2_32 -o server
	@g++ client.cpp -lws2_32  -o client

run:
	./server