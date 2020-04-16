#include"qp_tree.h"

void QPTree :: process(struct TableList *tables)
{
	struct TableList *curr=tables;
	while(curr!=NULL) {
		this->s->CopyRel(curr->tableName, curr->aliasAs);
		curr=curr->next;
	}
}

void QPTree :: process(struct AndList *boolean)
{

}
