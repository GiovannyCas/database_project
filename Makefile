C++ = g++
CFLAGS = -g
RM = rm -f



all: Server Client

Server: server.cpp
	$(C++) -Wall -Waextra -O2 $(CFLAGS) server.cpp -o server 

Client: client.cpp
	$(C++) -Wall -Waextra -O2 $(CFLAGS) client.cpp -o client

clean: 
	$(RM) server
	$(RM) client 