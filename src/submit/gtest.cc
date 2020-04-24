#include<gtest/gtest.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>
#include<iostream>

#include"statistics.h"
#include"parse_tree.h"
#include"qp_tree.h"

using namespace std;

int q() {
	Qptree qtree(NULL, NULL);
	int f = qtree.dispense_pipe();
	return f;
}

int qop() {
	sel_op_comp selop;
	operation le(1);
	operation ri(2);
	operation *l, *r;
	l = &le;
	r = &ri;
	l->cost = 200.0;
	r->cost = 20000.0;
	int f = selop.operator()(l, r);
	return f;
}

TEST(QTREETEST, pipecount) {
	int fr = q();
	EXPECT_EQ(fr, 0);
}

TEST(QTREETEST, selops){
	int fr = qop();
	EXPECT_EQ(fr, 1);
}

int main (int argc, char **argv) {

	testing::InitGoogleTest(&argc, argv);

	int stat;

	stat=RUN_ALL_TESTS();

	return stat;
}
