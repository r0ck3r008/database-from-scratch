#ifndef REL_OP_H
#define REL_OP_H

#include<pthread.h>

#include"mem/pipe.h"
#include"fs/dbfile.h"
#include"fs/record.h"
#include"function.h"
#include"comparison_engine.h"

class RelationalOp
{
public:
	// blocks the caller until the particular relational operator
	// has run to completion
	virtual void WaitUntilDone ();

	// tell us how much internal memory the operation can use
	virtual void Use_n_Pages (int);
};

#include"rel_ops/sel_file.h"
#include"rel_ops/sel_pipe.h"
#include"rel_ops/project.h"
#include"rel_ops/join.h"
#include"rel_ops/write_out.h"

#endif
