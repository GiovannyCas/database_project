C++ = g++
C++Ver = -std=gnu++17
CFLAGS = -g
RM = rm -f
OBJS = hashtable.o avl.o zset.o



all: server.exe Client



server.exe: server.o $(OBJS)
	$(C++) $(C++Ver) -Wall -Wextra -O2 $(CFLAGS) -o $@ server.o $(OBJS)

Client: client.cpp
	$(C++) $(C++Ver) -Wall -Wextra -O2 $(CFLAGS) client.cpp -o client

server.o: server.cpp hashtable.h common.h zset.h list.h
	$(C++) $(C++Ver) -Wall -Wextra -O2 $(CFLAGS) -c server.cpp

hashtable.o: hashtable.cpp hashtable.h
	$(C++) $(C++Ver) -Wall -Wextra -O2 $(CFLAGS) -c hashtable.cpp

avl.o: avl.cpp avl.h
	$(C++) $(C++Ver) -Wall -Wextra -O2 $(CFLAGS) -c avl.cpp

zset.o: zset.cpp zset.h hashtable.h avl.h common.h
	$(C++) $(C++Ver) -Wall -Wextra -O2 $(CFLAGS) -c zset.cpp

clean: 
	$(RM) server
	$(RM) server.exe
	$(RM) client 
	$(RM) hashtable
	$(RM) zset
	$(RM) avl
	$(RM) zset.o
	$(RM) avl.o
	$(RM) hashtable.o
	$(RM) server.o
