#include"qp_node.h"

using namespace std;

Node :: Node()
{
	this->flag=fl_none;
	this->j_order=0;
	this->rt=NULL;this->lft=NULL;this->up=NULL;this->dwn=NULL;
	this->rt_id=0;this->lft_id=0;this->up_id=0;this->dwn_id=0;
}
Node :: ~Node(){}

Node &Node :: operator=(Node &in)
{
	this->flag=in.flag;
	this->j_order=in.j_order;
	this->rt=in.rt; this->lft=in.lft; this->up=in.up; this->dwn=in.dwn;
	this->rt_id=in.rt_id; this->lft_id=in.lft_id; this->up_id=in.up_id;
	this->dwn_id=in.dwn_id;

	return *(this);
}
