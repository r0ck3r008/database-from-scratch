#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include<queue>

template <class T>
class Tournament
{
private:
	int size;
	T **tree;
	// NOTE
	// This might fail
	std :: queue <int> e_queue;

private:
	//functions
	int play_matches(T *, int);
	int promote(T *, int);

public:
	Tournament(int);
	~Tournament();
	T *get_winner();
	int feed(T *);
	void print_tree();
};

#endif
