CXX = clang
CFLAGS = -Wall

client:
	$(CXX) $(CFLAGS) client.c -o client

server:
	$(CXX) $(CFLAGS) server.c -o server

build:
	$(CXX) $(CFLAGS) client.c -o client
	$(CXX) $(CFLAGS) server.c -o server

clear:
	rm server
	rm client
