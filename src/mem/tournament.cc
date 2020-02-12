#ifndef TOURNAMENT_CC
#define TOURNAMENT_CC

#include<iostream>
#include<unistd.h>
#include"tournament.h"


template <class T>
Tournament <T> :: Tournament(int n_ext)
{
	tree=new T *[2*n_ext-1];
	std :: queue <int> e_queue;

	for(int i=0; i<n_ext; i++)
		this->e_queue.push(i);
	for(int i=0; i<2*n_ext-1; i++)
		this->tree[i]=NULL;

	if(n_ext%2!=0) {
		std :: cerr << "Tournament tree require even number of players!"
			<< std :: endl;
		//TODO
		//change to a graceful exit
		_exit(-1);
	}
	this->size=2*n_ext-1;
}

template <class T>
Tournament <T> :: ~Tournament()
{
	delete[] tree;
}

template <class T>
int Tournament <T> :: promote(int pos)
{
	int parent_pos=(pos/2) ? (pos%2)==0 : ((pos-1)/2);
	if(this->tree[parent_pos]==NULL)
		this->tree[parent_pos]=this->tree[pos];
	else {
		if(!parent_pos) {
			//return error as clearly this is a
			//non empty root and needs a get_winner
			//first
			return 0;
		}
		else {
			//just put it here and play match
			//with the overflow
			T *overflow=this->tree[parent_pos];
			this->tree[parent_pos]=this->tree[pos];
			if(!this->play_matches(overflow, parent_pos))
				return 0;
		}
	}

	this->tree[pos]=NULL;
	return 1;
}

template <class T>
int Tournament <T> :: play_matches(T *overflow, int pos)
{
	int match_pos=(pos-1) ? (pos%2==0) : (pos+1);

	//return if match position is empty
	if(this->tree[match_pos]==NULL)
		return 0;

	if(this->tree[pos]<=this->tree[match_pos])
		if(!this->promote(pos))
			return 0;
	else
		if(!this->promote(match_pos))
			return 0;

	//update empty queue if an empty position is released
	if(pos>=(this->size+1)/2-1)
		this->e_queue.push(pos);

	return 1;
}

template <class T>
T *Tournament <T> :: get_winner()
{
	T *ret=this->tree[0];
	this->tree[0]=NULL;
	return ret;
}

template <class T>
int Tournament <T> :: feed(T *in)
{
	//continue popping till you have an empty position
	//get an empty position
	int e_pos=this->e_queue.front();
	this->e_queue.pop();

	this->tree[e_pos]=in;
	if(!this->play_matches(NULL, e_pos))
		return 0;
	return 1;
}

#endif
