#include<stdlib.h>

#include"query.h"

char *cat_file="tmp/catalog";

query :: query(struct FuncOperator *func, struct TableList *tbls,
		struct AndList *a_list, struct NameList *grp_atts,
		struct NameList *sel_atts, int dis_att, int dis_func,
		int query_type, char *output_var, char *table, char *file,
		struct AttrList *atts_create, struct NameList *atts_sort)
{
	this->func=func;
	this->tbls=tbls;
	this->a_list=a_list;
	this->grp_atts=grp_atts;
	this->sel_atts=sel_atts;
	this->dis_att=dis_att;
	this->dis_func=dis_func;
	this->query_type=query_type;
	this->output_var=output_var;
	this->table=table;
	this->file=file;
	this->atts_create=atts_create;
	this->atts_sort=atts_sort;
}
query :: ~query(){}

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
	c.addRel(region, "region.tbl", Heap, 5);
	c.addAtt(region, "r_regionkey", -1, Int, 1);
	c.addAtt(region, "r_name", -1, String, 0);
	c.addAtt(region, "r_comment", -1, String, 0);

	c.addRel(nation, "nation.tbl", Heap, 25);
	c.addAtt(nation, "n_nationkey", -1, Int, 1);
	c.addAtt(nation, "n_name", -1, String, 0);
	c.addAtt(nation, "n_regionkey", 5, Int, 0);
	c.addAtt(nation, "n_comment", -1, String, 0);

	c.addRel(part, "part.tbl", Heap, 200000);
	c.addAtt(part, "p_partkey", -1,  Int, 1);
	c.addAtt(part, "p_name", -1, String, 0);
	c.addAtt(part, "p_mfgr", -1,  String, 0);
	c.addAtt(part, "p_brand", -1, String, 0);
	c.addAtt(part, "p_type", -1, String, 0);
	c.addAtt(part, "p_size", 50,  Int, 0);
	c.addAtt(part, "p_container", 40, String, 0);
	c.addAtt(part, "p_retailprice", -1, Double, 0);
	c.addAtt(part, "p_comment", -1, String, 0);

	c.addRel(supplier, "supplier.tbl", Heap, 10000);
	c.addAtt(supplier, "s_suppkey", -1, Int, 1);
	c.addAtt(supplier, "s_name", -1, String, 0);
	c.addAtt(supplier, "s_address", -1, String, 0);
	c.addAtt(supplier, "s_nationkey", 25, Int, 0);
	c.addAtt(supplier, "s_phone", -1, String, 0);
	c.addAtt(supplier, "s_acctbal", 9955, Double, 0);
	c.addAtt(supplier, "s_comment", -1, String, 0);


	c.addRel(partsupp, "partsupp.tbl", Heap, 800000);
	c.addAtt(partsupp, "ps_partkey",  200000, Int, 1);
	c.addAtt(partsupp, "ps_suppkey",  10000, Int, 0);
	c.addAtt(partsupp, "ps_availqty",  -1, Int, 0);
	c.addAtt(partsupp, "ps_supplycost",  -1, Double, 0);
	c.addAtt(partsupp, "ps_comment", -1, String, 0);



	c.addRel(customer, "customer.tbl", Heap, 150000);
	c.addAtt(customer, "c_custkey", -1, Int, 1);
	c.addAtt(customer, "c_name",  -1, String, 0);
	c.addAtt(customer, "c_address", -1, String, 0);
	c.addAtt(customer, "c_nationkey", 25, Int, 0);
	c.addAtt(customer, "c_phone",  -1, String, 0);
	c.addAtt(customer, "c_acctbal", -1, Double, 0);
	c.addAtt(customer, "c_mktsegment",  5, String, 0);
	c.addAtt(customer, "c_comment",  -1, String, 0);



	c.addRel(orders, "orders.tbl", Heap, 1500000);
	c.addAtt(orders, "o_orderkey", -1, Int, 1);
	c.addAtt(orders, "o_custkey", 150000, Int, 0);
	c.addAtt(orders, "o_orderstatus",  -1, String, 0);
	c.addAtt(orders, "o_totalprice",  -1, Double, 0);
	c.addAtt(orders, "o_orderdate",  -1, String, 0);
	c.addAtt(orders, "o_orderpriority",  -1, String, 0);
	c.addAtt(orders, "o_clerk",  -1, String, 0);
	c.addAtt(orders, "o_shippriority", -1, Int, 0);
	c.addAtt(orders, "o_comment",  -1, String, 0);



	c.addRel(lineitem, "lineitem.tbl", Heap, 6001215);
	c.addAtt(lineitem, "l_orderkey",  1500000, Int, 1);
	c.addAtt(lineitem, "l_partkey",  200000, Int, 0);
	c.addAtt(lineitem, "l_suppkey",  -1, Int, 0);
	c.addAtt(lineitem, "l_linenumber",  -1, Int, 0);
	c.addAtt(lineitem, "l_quantity", -1, Double, 0);
	c.addAtt(lineitem, "l_extendedprice",  -1, Double, 0);
	c.addAtt(lineitem, "l_discount",  11, Double, 0);
	c.addAtt(lineitem, "l_tax",  -1, Double, 0);
	c.addAtt(lineitem, "l_returnflag", 3, String, 0);
	c.addAtt(lineitem, "l_linestatus",  -1, String, 0);
	c.addAtt(lineitem, "l_shipdate",  -1, String, 0);
	c.addAtt(lineitem, "l_commitdate",  -1, String, 0);
	c.addAtt(lineitem, "l_receiptdate",  2554, String, 0);
	c.addAtt(lineitem, "l_shipinstruct",  4, String, 0);
	c.addAtt(lineitem, "l_shipmode",  7, String, 0);
	c.addAtt(lineitem, "l_comment",  -1, String, 0);

	c.write(cat_file);
}

void query :: dispatcher()
{
	if(this->query_type==1) {
		init_stats();
		Catalog c;
		c.read(cat_file);
		Qptree qpt(&c);
		qpt.process(this);
	} else {
		Ddl ddl(this, cat_file);
		ddl.process();
	}
}
