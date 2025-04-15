C++ = g++
CFLAGS = -g
RM = rm -f
OBJS = hashtable.o



all: Server Client



Server: server.o $(OBJS)
	$(C++) -std=c++11 -Wall -Wextra -O2 $(CFLAGS) -o server server.o $(OBJS)

Client: client.cpp
	$(C++) -std=c++11 -Wall -Wextra -O2 $(CFLAGS) client.cpp -o client

server.o: server.cpp hashtable.h
	$(C++) -std=c++11 -Wall -Wextra -O2 $(CFLAGS) -c server.cpp

hashtable.o: hashtable.cpp hashtable.h
	$(C++) -std=c++11 -Wall -Wextra -O2 $(CFLAGS) -c hashtable.cpp

clean: 
	$(RM) server
	$(RM) client 