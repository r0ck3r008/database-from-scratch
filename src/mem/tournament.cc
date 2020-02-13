#ifndef TOURNAMENT_CC
#define TOURNAMENT_CC

#include<iostream>
#include<unistd.h>
#include"tournament.h"

template <class T>
Tournament <T> :: node :: node(T *in, int init_pos)
{
	this->data=in;
	this->init_pos=init_pos;
}

template <class T>
Tournament <T> :: Tournament(int n_ext)
{
	tree=new struct node *[2*n_ext-1];

	for(int i=0; i<2*n_ext-1; i++)
		this->tree[i]=NULL;

	for(int i=(n_ext-1); i<(2*n_ext)-1; i++)
		this->e_queue.push(i);

	if(n_ext%2!=0) {
		std :: cerr << "Tournament tree requires even number of players!"
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
void Tournament <T> :: push_winner(struct node *winner)
{
	//free up initial position
	this->e_queue.push(winner->init_pos);
	//buffer the winner
	this->win_queue.push(winner->data);
	//delete node
	delete winner;
}

template <class T>
int Tournament <T> :: promote(struct node *winner, int pos)
{
	int parent_pos=(pos%2==0) ? ((pos/2)-1) : (((pos+1)/2)-1);
	this->tree[parent_pos]=winner;
	if(parent_pos==0) {
		if(winner->data!=NULL)
			this->push_winner(winner);
		else
			//this is end of flush
			return 0;
	} else {
		if(!this->play_matches(parent_pos))
			return 0;
	}

	return 1;
}

template <class T>
int Tournament <T> :: play_matches(int pos)
{
	int match_pos=(pos%2==0) ? (pos-1) : (pos+1);
	struct node *player_1=this->tree[pos];
	struct node *player_2=this->tree[match_pos];

	if(player_2==NULL)
		//cant move further as no player to play with
		return 1;

	struct node *winner;
	int winner_pos;
	if(player_1->data==NULL) {
		winner=player_2;
		winner_pos=match_pos;
	} else if(player_2->data==NULL) {
		winner=player_1;
		winner_pos=pos;
	} else if(*(player_1->data)<=*(player_2->data)) {
		winner=player_1;
		winner_pos=pos;
	} else {
		winner=player_2;
		winner_pos=match_pos;
	}

	if(!this->promote(winner, winner_pos))
		return 0;
	return 1;
}

template <class T>
int Tournament <T> :: feed(T *in)
{
	int e_pos;
	if(this->e_queue.size()!=0) {
		e_pos=this->e_queue.front();
		this->e_queue.pop();
	} else {
		std :: cerr << "Cant feed more!\n";
		return 0;
	}

	struct node *new_node=new struct node(in, e_pos);
	this->tree[e_pos]=new_node;

	if(!this->play_matches(e_pos))
		return 0;

	return 1;
}

template <class T>
std :: queue <T *> Tournament <T> :: flush()
{
	while(1) {
		if(!this->feed(NULL)) {
			break;
		}
	}
	return this->win_queue;
}

#endif
