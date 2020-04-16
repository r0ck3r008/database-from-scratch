#include"qp_tree.h"

using namespace std;

Node :: Node()
{
	this->rt=NULL;
	this->lft=NULL;
	this->up=NULL;
	this->dwn=NULL;
	this->lft_id=0;
	this->rt_id=0;
	this->up_id=0;
	this->dwn=0;
}
Node :: ~Node(){}

void Node :: AddNode(Node *in, int lft, int rt, int up, int dwn)
{
	if(!lft_id && lft) {
		this->lft_id=lft;
		this->lft=in;
		in->AddNode(this, 0, lft, 0, 0);
	} else if(!rt_id && rt) {
		this->rt_id=rt;
		this->rt=in;
		in->AddNode(this, rt, 0, 0, 0);
	} else if(!up_id && up) {
		this->up_id=up;
		this->up=in;
		in->AddNode(this, 0, 0, 0, up);
	} else if(!dwn_id && dwn) {
		this->dwn_id=dwn;
		this->dwn=in;
		in->AddNode(this, 0, 0, dwn, 0);
	} else {
		return;
	}
}
