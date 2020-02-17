#include<iostream>
#include<unistd.h>
#include"tournament.h"
#include"lex/comparison_engine.h"

Tournament :: node :: node(Record *in, int init_pos)
{
	this->data=in;
	this->init_pos=init_pos;
}

Tournament :: Tournament(int n_ext, struct OrderMaker *order)
{
	if(!n_ext) {
		std :: cerr << "Invalid number of players!";
		_exit(-1);
	}
	tree=new struct node *[2*n_ext-1];

	for(int i=0; i<2*n_ext-1; i++)
		this->tree[i]=NULL;

	for(int i=(n_ext-1); i<(2*n_ext)-1; i++)
		this->e_queue.push(i);

	this->size=2*n_ext-1;
	this->ceng=new ComparisonEngine;
	this->order=order;
}

Tournament :: ~Tournament()
{
	delete this->ceng;
	delete[] this->tree;
}

void Tournament :: push_winner(struct node *winner)
{
	//free up initial position
	this->e_queue.push(winner->init_pos);
	//buffer the winner
	if((winner->data->check_null()))
		std :: cerr << "Received NULL!\n";
	this->win_queue.push(winner->data);
	//delete node
	delete winner;
}

int Tournament :: promote(struct node *winner, int pos)
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

int Tournament :: play_matches(int pos)
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
	} else if(ceng->Compare(player_1->data, player_2->data,
				this->order)<0) {
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

int Tournament :: feed(Record *in)
{
	int e_pos;
	if((e_pos=this->get_nxt_spot(1))<0) {
		std :: cerr << "Cant feed more!\n";
		return 0;
	}
	struct node *new_node=new struct node(in, e_pos);
	this->tree[e_pos]=new_node;

	if(!this->play_matches(e_pos))
		return 0;

	return 1;
}

std :: queue <Record *> *Tournament :: flush()
{
	while(1) {
		if(!this->feed(NULL)) {
			break;
		}
	}
	return &(this->win_queue);
}

int Tournament :: get_nxt_spot(int pop)
{
	int e_pos;
	if(this->e_queue.size()!=0) {
		e_pos=this->e_queue.front();
		if(pop)
			this->e_queue.pop();
	} else {
		return -1;
	}

	return e_pos;
}

int Tournament :: get_player_num()
{
	return (((this->size)+1)/2);
}

Record *Tournament :: get_nxt_winner()
{
	if(!this->win_queue.size())
		return NULL;

	Record *rec=this->win_queue.front();
	this->win_queue.pop();
	return rec;
}
