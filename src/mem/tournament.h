#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include<queue>

#include"lex/comparison_engine.h"

template <class T>
class Tournament
{
private:
	struct node
	{
		T *data;
		int init_pos;
	public:
		bool operator<=(struct node *);
		node(T *, int);
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
	std :: queue <T *> win_queue;
	Tournament(int, struct comparator *);
	~Tournament();
	int feed(T *);
	std :: queue <T *> flush();
};

#endif
