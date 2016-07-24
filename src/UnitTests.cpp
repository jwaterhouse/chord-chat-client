#include <string>

#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>

#include "ID.h"
#include "FingerTable.h"
#include "RemoteNode.h"

TEST_CASE("IDCopyConstructor", "[ID]")
{
	char b1[ID_LEN] = {0};

	for(int i = 0; i < ID_LEN; ++i)
	{
			b1[i] = i;
	}

	ID id1(b1);
	ID id2(id1);

	REQUIRE(id1 == id2);
}

TEST_CASE("IDStringConstructor", "[ID]")
{
	std::string s = "some TEST_CASE string";

	ID id1(s);
	ID id2(s);

	REQUIRE(id1 == id2);
}

TEST_CASE("IDEquality", "[ID]")
{
	char b1[ID_LEN] = {0};
	char b2[ID_LEN] = {0};
	char b3[ID_LEN] = {1};
	char b4[ID_LEN] = {2};

	for(int i = 0; i < ID_LEN; ++i)
	{
			b1[i] = i;
			b2[i] = i;
	}

	ID id1(b1);
	ID id2(b2);
	ID id3(b3);
	ID id4(b4);

	REQUIRE(id1 == id2);
	REQUIRE(id1 <= id2);
	REQUIRE(id1 >= id2);
	REQUIRE(id3 != id4);
	REQUIRE(id3 < id4);
	REQUIRE(id3 <= id4);
	REQUIRE(id4 > id3);
	REQUIRE(id4 >= id3);
}

TEST_CASE("IDAddition", "[ID]")
{
	char b1[ID_LEN] = {1};
	char b2[ID_LEN] = {2};
	char b3[ID_LEN] = {3};

	ID id1(b1);
	ID id2(b2);
	ID id3(b3);

	REQUIRE((id1 + id2) == id3);
	id1 += id2;
	REQUIRE(id1 == id3);
}

TEST_CASE("IDSubtraction", "[ID]")
{
	char b1[ID_LEN] = {1};
	char b2[ID_LEN] = {2};
	char b3[ID_LEN] = {3};

	ID id1(b1);
	ID id2(b2);
	ID id3(b3);

	REQUIRE((id3 - id2) == id1);
	id3 -= id2;
	REQUIRE(id3 == id1);
}

TEST_CASE("FingerTableGetSetNode", "[FingerTable]")
{
	FingerTable f = FingerTable();
	Node n1(new RemoteNode("blah", "blah", 1234));
	Node n2(new RemoteNode("blah2", "blah2", 1235));
	f.setNode(0, n1);
	f.setNode(10, n2);

	REQUIRE(f.getNode(0) == n1);
	REQUIRE(f.getNode(10) == n2);
	REQUIRE(f.getNode(9) == n2);	// Should return the node at index 10
	REQUIRE(f.getNodeAt(9) == nullptr);	// Should return nullptr (no node at index 9)
	REQUIRE(f.getNode(11) == nullptr);	// Should return nullptr since no node at index > 10
}
