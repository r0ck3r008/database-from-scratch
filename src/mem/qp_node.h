#ifndef QP_NODE_H
#define QP_NODE_H

#include<queue>

#include"rel_ops/rel_ops.h"

struct Node
{
	union
	{
		SelectPipe *sp;
		SelectFile *sf;
		Sum *s;
		Join *j;
		GroupBy *gb;
		DuplicateRemoval *rd;
		WriteOut *wo;
	};
	struct Node *rt, *lft, *up, *dwn;
	int lft_id, rt_id, up_id, dwn_id;
public:
	Node();
	~Node();
	void AddNode(Node *, int, int, int, int);
};

#endif
