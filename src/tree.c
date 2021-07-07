#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "tree.h"
#include "list.h"

struct tree*
tree_add_child(struct tree* tree, void* elem)
{
	if(!elem)
		return tree;

	struct tree* child = calloc(1, sizeof(*child));
	if(!child)
		return NULL;
	child->elem = elem;

	if (!tree)
		tree = child;
	else
		list_push(tree->children, child);

	return tree;
}

void
tree_destroy(struct tree* tree)
{
	assert(false && "Not implemented yet.");
}
