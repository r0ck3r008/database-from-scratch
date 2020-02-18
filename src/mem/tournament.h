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
		const Tournament *ref;
	public:
		bool operator<=(struct node);
		node(Record *, int, const Tournament *);
	};
	int size;
	struct node **tree;
	std :: queue <int> e_queue;
	struct comparator *comp;

private:
	//functions
	void push_winner(struct node *);
	int promote(struct node *, int);
	int play_matches(int);

public:
	std :: queue <Record *> win_queue;
	Tournament(int, struct comparator *);
	~Tournament();
	int feed(Record *);
	std :: queue <Record *> *flush();
	int get_nxt_spot(int);
	int get_player_num();
	Record *get_nxt_winner();
};

#endif
