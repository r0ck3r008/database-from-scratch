#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "optimizer/statistics.h"

void fill ()
{
	Statistics s;

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
	s.AddRel(region, 5);
	s.AddAtt(region, "r_regionkey", -1);
	s.AddAtt(region, "r_name", -1);

	//nation
	s.AddRel(nation, 25);
	s.AddAtt(nation, "n_nationkey", 25);
	s.AddAtt(nation, "n_regionkey",5);
	s.AddAtt(nation, "n_name",25);

	//part
	s.AddRel(part, 200000);
	s.AddAtt(part, "p_partkey",200000);
	s.AddAtt(part, "p_size",50);
	s.AddAtt(part, "p_container",40);

	//supplier
	s.AddRel(supplier, 10000);
	s.AddAtt(supplier, "s_suppkey",10000);
	s.AddAtt(supplier, "s_nationkey",25);

	//partsupp
	s.AddRel(partsupp, 800000);
	s.AddAtt(partsupp, "ps_suppkey", 10000);
	s.AddAtt(partsupp, "ps_partkey", 200000);

	//customer
	s.AddRel(customer, 150000);
	s.AddAtt(customer, "c_custkey",150000);
	s.AddAtt(customer, "c_nationkey",25);
	s.AddAtt(customer, "c_mktsegment",5);

	//orders
	s.AddRel(orders,1500000);
	s.AddAtt(orders, "o_orderkey",1500000);
	s.AddAtt(orders, "o_custkey",150000);


	//lineitem
	s.AddRel(lineitem, 6001215);
	s.AddAtt(lineitem, "l_returnflag",3);
	s.AddAtt(lineitem, "l_discount",11);
	s.AddAtt(lineitem, "l_shipmode",7);
	s.AddAtt(lineitem, "l_partkey",200000);
	s.AddAtt(lineitem, "l_shipinstruct",4);
	s.AddAtt(lineitem, "l_orderkey",1500000);
	s.AddAtt(lineitem, "l_receiptdate",2554);

	s.Write("statistics.txt");
}

int main(int argc, char *argv[])
{
	fill();
}
