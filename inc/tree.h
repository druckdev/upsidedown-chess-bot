#ifndef TREE_H
#define TREE_H

struct tree {
	void* elem;
	struct tree* parent;
	struct list* children;
};

/*
 * Creates a new child with `elem` at tree node `tree. If `tree` is NULL it will
 * make the new child the actual tree and not append to the children list. This
 * means, this function can also be used to create new trees.
 *
 * Returns the root of the tree. NOTE(Aurel): This might be either the argument
 * given or a newly created node.
 *
 * Runtime: O(1)
 */
struct tree* tree_add_child(struct tree* tree, void* elem);

void tree_destroy(struct tree* tree);

#endif /* TREE_H */
