# I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CC=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS=-c -Wall -DASIO_STANDALONE -Ivendor/asio/include
CPFLAGS=-pthread -std=c++0x

all: chkdir hello

hello: main.o ID.o FingerTable.o sha1.o LocalNode.o RemoteNode.o
	$(CC) obj/main.o obj/ID.o obj/FingerTable.o obj/sha1.o obj/LocalNode.o obj/RemoteNode.o -o bin/main $(CPFLAGS)

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp -o obj/main.o $(CPFLAGS)

ID.o: src/ID.cpp
	$(CC) $(CFLAGS) src/ID.cpp -o obj/ID.o $(CPFLAGS)

FingerTable.o: src/FingerTable.cpp
	$(CC) $(CFLAGS) src/FingerTable.cpp -o obj/FingerTable.o $(CPFLAGS)

sha1.o: src/sha1.cpp
	$(CC) $(CFLAGS) src/sha1.cpp -o obj/sha1.o $(CPFLAGS)

LocalNode.o: src/LocalNode.cpp
	$(CC) $(CFLAGS) src/LocalNode.cpp -o obj/LocalNode.o $(CPFLAGS)

RemoteNode.o: src/RemoteNode.cpp
	$(CC) $(CFLAGS) src/RemoteNode.cpp -o obj/RemoteNode.o $(CPFLAGS)

clean:
	rm -rf bin obj

chkdir:
	mkdir -p bin
	mkdir -p obj