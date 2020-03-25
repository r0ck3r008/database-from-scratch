#include<gtest/gtest.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h>

#include"test.h"
#include"bigq.h"
#include"rel_op.h"
#include"timer.h"

relation *rela;
Attribute IA = {"int", Int};
Attribute SA = {"string", String};
Attribute DA = {"double", Double};

int clear_pipe (Pipe &in_pipe, Schema *schema, bool print) {
	Record rec;
	int cnt = 0;
	while (in_pipe.Remove (&rec)) {
		if (print) {
			rec.Print (schema);
		}
		cnt++;
	}
	return cnt;
}

int clear_pipe (Pipe &in_pipe, Schema *schema, Function &func, bool print) {
	Record rec;
	int cnt = 0;
	double sum = 0;
	while (in_pipe.Remove (&rec)) {
		if (print) {
			rec.Print (schema);
		}
		int ival = 0; double dval = 0;
		func.Apply (rec, ival, dval);
		sum += (ival + dval);
		cnt++;
	}
	cout << " Sum: " << sum << endl;
	return cnt;
}
int pipesz = 100;
int buffsz = 100; // pages of memory allowed for operations

SelectFile SF_ps, SF_p, SF_s, SF_o, SF_li, SF_c;
DBFile dbf_ps, dbf_p, dbf_s, dbf_o, dbf_li, dbf_c;
CNF cnf_ps, cnf_p, cnf_s, cnf_o, cnf_li, cnf_c;
Record lit_ps, lit_p, lit_s, lit_o, lit_li, lit_c;
Function func_ps, func_p, func_s, func_o, func_li, func_c;

int pAtts = 9;
int psAtts = 5;
int liAtts = 16;
int oAtts = 9;
int sAtts = 7;
int cAtts = 8;
int nAtts = 4;
int rAtts = 3;

void init_SF_ps (char *pred_str, int numpgs) {
	dbf_ps.Open (rel[1]->path());
	get_cnf (pred_str, rel[1]->schema (), cnf_ps, lit_ps);
	SF_ps.Use_n_Pages (numpgs);
}

void init_SF_p (char *pred_str, int numpgs) {
	dbf_p.Open (rel[2]->path());
	get_cnf (pred_str, rel[2]->schema (), cnf_p, lit_p);
	SF_p.Use_n_Pages (numpgs);
}

void init_SF_s (char *pred_str, int numpgs) {
	dbf_s.Open (rel[0]->path());
	get_cnf (pred_str, rel[0]->schema (), cnf_s, lit_s);
	SF_s.Use_n_Pages (numpgs);
}

void init_SF_o (char *pred_str, int numpgs) {
	dbf_o.Open (rel[5]->path());
	get_cnf (pred_str, rel[5]->schema (), cnf_o, lit_o);
	SF_o.Use_n_Pages (numpgs);
}

void init_SF_li (char *pred_str, int numpgs) {
	dbf_li.Open (rel[7]->path());
	get_cnf (pred_str, rel[7]->schema (), cnf_li, lit_li);
	SF_li.Use_n_Pages (numpgs);
}

void init_SF_c (char *pred_str, int numpgs) {
	dbf_c.Open (rel[4]->path());
	get_cnf (pred_str, rel[4]->schema (), cnf_c, lit_c);
	SF_c.Use_n_Pages (numpgs);
}

int q1() {

	Pipe _ps (pipesz), _p (pipesz), _s (pipesz), _o (pipesz), _li (pipesz), _c (pipesz);
    char *pred_ps = "(ps_supplycost < 5.00)";
	init_SF_ps (pred_ps, 100);

	Project P_ps;
	Pipe __ps (pipesz);
	int keepMe[] = {1};
	int numAttsIn = psAtts;
	int numAttsOut = 1;
	P_ps.Use_n_Pages (buffsz);

	DuplicateRemoval D;
	// inpipe = __ps
	Pipe ___ps (pipesz);
	Schema __ps_sch ("__ps", 1, &IA);

	WriteOut W;
	// inpipe = ___ps
	char *fwpath = "test.ps.w.tmp";
	FILE *writefile = fopen (fwpath, "w");

	timer t;
	t.start_timer();

	SF_ps.Run (&dbf_ps, &_ps, &cnf_ps, &lit_ps);
	P_ps.Run (&_ps, &__ps, keepMe, numAttsIn, numAttsOut);
	D.Run (&__ps, &___ps, &__ps_sch);
	W.Run (&___ps, writefile, &__ps_sch);
	SF_ps.WaitUntilDone ();
	P_ps.WaitUntilDone ();
	D.WaitUntilDone ();
	W.WaitUntilDone ();

	t.stop_timer();
	struct timeval diff=t.get_tt();
	fclose(writefile);
    
    FILE *readfile = NULL;
	
	if((readfile = fopen (fwpath, "r")) == NULL){
		_exit(-1);
	}

    char str[128];
    char *token;
    int count = 0;

    while(!feof(readfile)){
        fgets(str, sizeof(char)*128, readfile);
        token = strtok(str, "\n");
        while (token != NULL) {
            count++;
            token = strtok(NULL, ":");
        }
        explicit_bzero(str, sizeof(char)*128);
    }
    fclose(readfile);

    return count;
}

int q2() {

	Pipe _ps (pipesz), _p (pipesz), _s (pipesz), _o (pipesz), _li (pipesz), _c (pipesz);
	char *pred_s = "(s_acctbal < 1000.00)";
	init_SF_s (pred_s, 100);

	Sum T;
	// _s (input pipe)
	Pipe _out (1);
	Function func;
	char *str_sum = "s_acctbal";
	get_cnf (str_sum, rel[0]->schema (), func);
	func.Print ();
	T.Use_n_Pages (1);

	timer t;
	t.start_timer();

	SF_s.Run (&dbf_s, &_s, &cnf_s, &lit_s);
	T.Run (&_s, &_out, &func);
	SF_s.WaitUntilDone ();
	T.WaitUntilDone ();
	Schema out_sch ("out_sch", 1, &DA);
	int cnt = clear_pipe (_out, &out_sch, true);

	t.stop_timer();
	struct timeval diff=t.get_tt();

	dbf_s.Close ();
	return 1;
}

int q3() {
	
	Pipe _ps (pipesz), _p (pipesz), _s (pipesz), _o (pipesz), _li (pipesz), _c (pipesz);
char *pred_s = "(s_suppkey = s_suppkey)";
	init_SF_s (pred_s, 100);

	char *pred_ps = "(ps_suppkey = ps_suppkey)";
	init_SF_ps (pred_ps, 100);

	Join J;
	// left _s
	// right _ps
	Pipe _s_ps (pipesz);
	CNF cnf_p_ps;
	Record lit_p_ps;
	get_cnf ("(s_suppkey = ps_suppkey)", rel[0]->schema(), rel[1]->schema(), cnf_p_ps, lit_p_ps);

	int outAtts = sAtts + psAtts;
	Attribute ps_supplycost = {"ps_supplycost", Double};
	Attribute joinatt[] = {IA,SA,SA,IA,SA,DA,SA, IA,IA,IA,ps_supplycost,SA};
	Schema join_sch ("join_sch", outAtts, joinatt);

	timer t;
	t.start_timer();

	SF_s.Run (&dbf_s, &_s, &cnf_s, &lit_s);
	SF_ps.Run (&dbf_ps, &_ps, &cnf_ps, &lit_ps);
	J.Run (&_s, &_ps, &_s_ps, &cnf_p_ps, &lit_p_ps, rel[0]->schema(),
	       rel[1]->schema());
	SF_s.WaitUntilDone();
	SF_ps.WaitUntilDone ();

	int cnt = clear_pipe (_s_ps, &join_sch, false);

	t.stop_timer();
	struct timeval diff=t.get_tt();

	return cnt;
}

int q4() {
	
}

int q5() {
	Pipe _s(pipesz);
	char *pred_s = "(s_suppkey = s_suppkey)";
	init_SF_s (pred_s, 100);
	SF_s.Run (&dbf_s, &_s, &cnf_s, &lit_s);
	Schema out_sch("out_sch", 1, &DA);

	int cnt = clear_pipe(_s, &out_sch, false);

	return cnt;
}

int q6() {
	Record rec1, rec2;
	double res1 = 0, res2 = 0;
	Schema recs1("recs1", 1, &DA);
	Schema recs2("recs2", 1, &DA);
	char *result1 = new char[1];
	char *result2 = new char[1];
	sprintf(result1, "1%0.7f|", res1);
	sprintf(result2, "2%0.7f|", res2);
	rec1.ComposeRecord(&recs1, result1);
	rec2.ComposeRecord(&recs2, result2);


	rec1.Print(&recs1);
	rec2.Print(&recs2);

	return 1;
}

TEST(DUPLICATE, duplicate) {
	int dist = q1();
	EXPECT_EQ(dist, 2733);
}

TEST(SUMTEST, sum) {
	int dist = q2();
	EXPECT_EQ(dist, 1);
}

TEST(JOINTEST, join) {
	int dist = q3();
	EXPECT_EQ(dist, 800000);
}

TEST(SELECTFILETEST, selectfile) {
	int dist = q5();
	EXPECT_EQ(dist, 10000);
}

int main (int argc, char **argv) {

	testing::InitGoogleTest(&argc, argv);

	setup();

	int stat;

	stat=RUN_ALL_TESTS();

	cleanup();

	return stat;
}
