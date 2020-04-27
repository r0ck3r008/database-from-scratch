#include <iostream>

#include"entrypoint.h"

char *cat_file="tmp/catalog.txt";

using namespace std;

extern "C" {
	int yyparse(void);   // defined in y.tab.c
}

void init_stats()
{
	Catalog c;

	//names
	char *region="region";
	char *nation="nation";
	char *part="part";
	char *supplier="supplier";
	char *partsupp="partsupp";
	char *customer="customer";
	char *orders="orders";
	char *lineitem="lineitem";

	//region
	c.addRel(region, "region.tbl", Heap, 5);
	c.addAtt(region, "r_regionkey", -1, Int);
	c.addAtt(region, "r_name", -1, String);

	//nation
	c.addRel(nation, "nation.tbl", Heap, 25);
	c.addAtt(nation, "n_nationkey", 25, Int);
	c.addAtt(nation, "n_regionkey", 5, Int);
	c.addAtt(nation, "n_name", 25, String);

	//part
	c.addRel(part, "part.tbl", Heap, 200000);
	c.addAtt(part, "p_partkey", 200000, Int);
	c.addAtt(part, "p_size", 50, Int);
	c.addAtt(part, "p_container", 40, String);

	//supplier
	c.addRel(supplier, "supplier.tbl", Heap, 10000);
	c.addAtt(supplier, "s_suppkey", 10000, Int);
	c.addAtt(supplier, "s_nationkey", 25, Int);
	c.addAtt(supplier, "s_acctbal", 9955, Double);

	//partsupp
	c.addRel(partsupp, "partsupp.tbl", Heap, 800000);
	c.addAtt(partsupp, "ps_suppkey", 10000, Int);
	c.addAtt(partsupp, "ps_partkey", 200000, Int);

	//customer
	c.addRel(customer, "customer.tbl", Heap, 150000);
	c.addAtt(customer, "c_custkey", 150000, Int);
	c.addAtt(customer, "c_nationkey", 25, Int);
	c.addAtt(customer, "c_mktsegment", 5, String);

	//orders
	c.addRel(orders, "orders.tbl", Heap, 1500000);
	c.addAtt(orders, "o_orderkey", 1500000, Int);
	c.addAtt(orders, "o_custkey", 150000, Int);


	//lineitem
	c.addRel(lineitem, "lineitem.tbl", Heap, 6001215);
	c.addAtt(lineitem, "l_returnflag", 3, Int);
	c.addAtt(lineitem, "l_discount", 11, Double);
	c.addAtt(lineitem, "l_shipmode", 7, String);
	c.addAtt(lineitem, "l_partkey", 200000, Int);
	c.addAtt(lineitem, "l_shipinstruct", 4, String);
	c.addAtt(lineitem, "l_orderkey", 1500000, Int);
	c.addAtt(lineitem, "l_receiptdate", 2554, String);

	c.write(cat_file);
}

int main ()
{
	init_stats();

	cout << "Enter the query: \n";
	yyparse();

	Catalog c;
	c.read(cat_file);
	Qptree qpt(&c);
	struct query q(finalFunction, tables, boolean, groupingAtts,
			attsToSelect, distinctAtts, distinctFunc);
	qpt.process(&q);
}
