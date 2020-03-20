#ifndef REL_OP_H
#define REL_OP_H

#include<pthread.h>

#include"mem/pipe.h"
#include"mem/bigq.h"
#include"fs/dbfile.h"
#include"fs/record.h"
#include"glbl/defs.h"
#include"function.h"
#include"comparison_engine.h"

#include"rel_ops/sel_file.h"
#include"rel_ops/sel_pipe.h"
#include"rel_ops/project.h"
#include"rel_ops/sum.h"
#include"rel_ops/join.h"
#include"rel_ops/group_by.h"
#include"rel_ops/rem_dup.h"
#include"rel_ops/write_out.h"

#endif
