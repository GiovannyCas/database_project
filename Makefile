C++ = g++
CFLAGS = -g
RM = rm -f



all: Server Client

Server: server.cpp
	$(C++) -std=c++11 -Wall -Wextra -O2 $(CFLAGS) server.cpp -o server 

Client: client.cpp
	$(C++) -std=c++11 -Wall -Wextra -O2 $(CFLAGS) client.cpp -o client

clean: 
	$(RM) server
	$(RM) client 