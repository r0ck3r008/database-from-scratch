#include<gtest/gtest.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>
#include<iostream>

#include"db/catalog.h"
#include"db/schema.h"

using namespace std;

int q() {
	Schema schem;
	char ch = 'char';
	char *c = &ch;
	schem.addAtt(c, Int, 1, 1);
	return schem.attMap.size();
}

Schema *qop() {
	Catalog cat;
	char ch = 'char';
	char *c = &ch;
	return cat.snap(c);
}

TEST(SCHEMATEST, addattribute) {
	int fr = q();
	EXPECT_EQ(fr, 1);
}

TEST(CATALOGTEST, snap){
	Schema *fr = qop();
	EXPECT_EQ(fr, nullptr);
}

int main (int argc, char **argv) {

	testing::InitGoogleTest(&argc, argv);

	int stat;

	stat=RUN_ALL_TESTS();

	return stat;
}
