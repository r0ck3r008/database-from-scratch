#ifndef QP_NODE_H
#define QP_NODE_H

#include<queue>

#include"rel_ops/rel_ops.h"


enum node_mask : uint8_t
{
	fl_none=0,
	fl_sel_pipe=1<<0,
	fl_sel_file=1<<1,
	fl_sum=1<<2,
	fl_join=1<<3
};

struct Node
{
	uint8_t flag;
	int j_order;
	struct Node *rt, *lft, *up, *dwn;
	int lft_id, rt_id, up_id, dwn_id;

public:
	Node();
	~Node();
	Node &operator=(Node &);
};

#endif
