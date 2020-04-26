#ifndef STATISTICS_H
#define STATISTICS_H

#include<iostream>
#include<unordered_map>

#include"db/schema.h"
#include"db/catalog.h"
#include"parser/parse_tree.h"

class Statistics
{
	std::unordered_map<std::string, Schema *>
						relMap;
	Catalog *c;

private:
	double traverse(struct AndList *,
			struct OrList *);
	double operate(struct OrList *);
	void get_rel(char *, Schema **, Attribute **);

public:
	Statistics(Catalog *);
	~Statistics();
	void addRel(char *, char *);
	double Estimate(struct AndList *);
};

#endif
