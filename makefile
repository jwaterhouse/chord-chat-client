CC=g++
CFLAGS=-c -Wall -DASIO_STANDALONE -Ivendor/asio/include -Ivendor/UnitTest++/src
CPFLAGS=-pthread -std=c++0x
LDFLAGS_TEST=vendor/UnitTest++/libs/libUnitTest++.a

all: chat

chat: chkdir ChordChat

test: chkdir TestChord

TestChord: UnitTests.o ID.o FingerTable.o sha1.o LocalNode.o RemoteNode.o ChatClient.o
	$(CC) obj/UnitTests.o obj/ID.o obj/FingerTable.o obj/sha1.o obj/LocalNode.o obj/RemoteNode.o obj/ChatClient.o -o bin/TestChord $(CPFLAGS) $(LDFLAGS_TEST)

UnitTests.o: src/UnitTests.cpp
	$(CC) $(CFLAGS) src/UnitTests.cpp -o obj/UnitTests.o $(CPFLAGS)

ChordChat: ChordChat.o ID.o FingerTable.o sha1.o LocalNode.o RemoteNode.o ChatClient.o
	$(CC) obj/ChordChat.o obj/ID.o obj/FingerTable.o obj/sha1.o obj/LocalNode.o obj/RemoteNode.o obj/ChatClient.o -o bin/ChordChat $(CPFLAGS) $(LDFLAGS)

ChordChat.o: src/ChordChat.cpp
	$(CC) $(CFLAGS) src/ChordChat.cpp -o obj/ChordChat.o $(CPFLAGS)

ChatClient.o: src/ChatClient.cpp
	$(CC) $(CFLAGS) src/ChatClient.cpp -o obj/ChatClient.o $(CPFLAGS)

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
