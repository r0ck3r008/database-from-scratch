#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include<queue>

#include"fs/record.h"
#include"lex/comparison_engine.h"

class Tournament
{
private:
	struct node
	{
		Record *data;
		int init_pos;
	public:
		node(Record *, int);
	};
	int size;
	struct node **tree;
	std :: queue <int> e_queue;
	struct ComparisonEngine *ceng;
	struct OrderMaker *order;

private:
	//functions
	void push_winner(struct node *);
	int promote(struct node *, int);
	int play_matches(int);

public:
	std :: queue <Record *> win_queue;
	Tournament(int, struct OrderMaker *);
	~Tournament();
	int feed(Record *);
	std :: queue <Record *> *flush();
};

#endif
