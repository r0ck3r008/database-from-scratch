#include<gtest/gtest.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>

#include"cheader.h"
#include"optimizer/statistics.h"
#include"parser/parse_tree.h"

int* q() {
	
	Statistics s;
        
	char *relName[] = {"supplier","partsupp"};
	int *a = new int[2];

	s.AddRel(relName[0],10000);
	s.AddAtt(relName[0], "s_suppkey",10000);

	s.AddRel(relName[1],800000);
	s.AddAtt(relName[1], "ps_suppkey", 10000);

	a[0] = s.relMap.size();
	a[1] = s.attMap.size();

	return a;
}

TEST(ADDRELATIONTEST, addrel) {
	int *r = q();
	EXPECT_EQ(r[0], 2);
}

TEST(ADDATTRIBUTETEST, addatt) {
	int *r = q();
	EXPECT_EQ(r[1], 2);
}

int main (int argc, char **argv) {

	testing::InitGoogleTest(&argc, argv);

	setup();

	int stat;

	stat=RUN_ALL_TESTS();

	cleanup();

	return stat;
}
