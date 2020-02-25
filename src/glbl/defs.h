#ifndef DEFS_H
#define DEFS_H

#define MAX_ANDS 20
#define MAX_ORS 20

#define PAGE_SIZE 131072


enum Target {Left, Right, Literal};
enum CompOperator {LessThan, GreaterThan, Equals};
enum Type {Int, Double, String};
enum fType {Heap, Sorted, Tree};

unsigned int Random_Generate();

#ifdef NEED_STRUCTS

#include"lex/comparison.h"

struct SortInfo
{
	struct OrderMaker *order;
	int run_len;
};

struct metadata
{
	int curLength;
	fType type;
	struct SortInfo *s_info;
};

#endif

#endif

