#include <UnitTest++.h>
#include <string>
#include "../include/ID.h"
#include "../include/FingerTable.h"
#include "../include/RemoteNode.h"

TEST(IDCopyConstructor)
{
	char b1[ID_LEN] = {0};
	
	for(int i = 0; i < ID_LEN; ++i)
	{
			b1[i] = i;
	}
	
	ID id1(b1);
	ID id2(id1);
	
	CHECK(id1 == id2);
}

TEST(IDStringConstructor)
{
	std::string s = "some test string";
	
	ID id1(s);
	ID id2(s);
	
	CHECK(id1 == id2);
}

TEST(IDEquality)
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
	
	CHECK(id1 == id2);
	CHECK(id3 != id4);
	CHECK(id3 < id4);
	CHECK(id3 <= id4);
	CHECK(id4 > id3);
	CHECK(id4 >= id3);
}

TEST(IDAddition)
{
	char b1[ID_LEN] = {1};
	char b2[ID_LEN] = {2};
	char b3[ID_LEN] = {3};
	
	ID id1(b1);
	ID id2(b2);
	ID id3(b3);
	
	CHECK((id1 + id2) == id3);
	id1 += id2;
	CHECK(id1 == id3);
}

TEST(IDSubtraction)
{
	char b1[ID_LEN] = {1};
	char b2[ID_LEN] = {2};
	char b3[ID_LEN] = {3};
	
	ID id1(b1);
	ID id2(b2);
	ID id3(b3);
	
	CHECK((id3 - id2) == id1);
	id3 -= id2;
	CHECK(id3 == id1);
}

TEST(FingerTableGetSetNode)
{
	FingerTable f = FingerTable();
	Node n1(new RemoteNode("blah", "blah", 1234));
	Node n2(new RemoteNode("blah2", "blah2", 1235));
	f.setNode(0, n1);
	f.setNode(10, n2);
	
	CHECK(f.getNode(0) == n1);
	CHECK(f.getNode(10) == n2);
	CHECK(f.getNode(9) == n2);	// Should return the node at index 10
	CHECK(f.getNodeAt(9) == NULL);	// Should return NULL (no node at index 9)
	CHECK(f.getNode(11) == NULL);	// Should return NULL since no node at index > 10
}

int main()
{
    return UnitTest::RunAllTests();
}


