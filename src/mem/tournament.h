#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include<queue>

template <class T>
class Tournament
{
private:
	struct node
	{
		node(T *, int);
		T *data;
		int init_pos;
	};
	int size;
	struct node **tree;
	std :: queue <T *> win_queue;
	std :: queue <int> e_queue;

private:
	//functions
	void push_winner(struct node *);
	int promote(struct node *, int);
	int play_matches(int);

public:
	Tournament(int);
	~Tournament();
	T *get_winner();
	int feed(T *);
};

#endif
