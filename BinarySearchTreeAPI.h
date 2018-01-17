#ifndef BST_GUARD
#define BST_GUARD

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/**************************************
Interface for a Binary Tree ADT
Author:  Judi McCuaig, James Fraser
October, 2012, mod. Nov 2017
Modfied: Denis Nikitenko (Jan 2018)
**************************************/

/**
 * Function pointer typedefs
 */
//typedef int (*CompareFunc)(const void* a, const void* b);
//typedef void (*DeleteFunc)(void* data);
//typedef void (*PrintFunc)(void* data);

/*
 * Typedef the void* to make the API cleaner and more readable.
 */
typedef void* TreeDataPtr;

/*
 * A Single Binary Tree Node.
 * Left and Right branches
 * void* data
 */

typedef struct binTreeNode{
	TreeDataPtr data;
	TreeNode*   left;
	TreeNode*   right;
	//TreeNode * parent; Optional but useful
    //Tree* parentTree;  Optional but gets you access to function pointers.
} TreeNode;

/**
 * This is the definition of the Binary Tree.
 *
 */
typedef struct binTree
{
	TreeNode*   root;
    void (*deleteData)(void* toBeDeleted);
    int (*compareData)(const void* first,const void* second);

	// Additions must work with abstract data types.
	// Additional function pointers to generalize tree.
	// Additional properties can be added such as count.
} Tree;

/**
 * Creates a TreeNode. TreeNode children are set to NULL and data is set to the passed in data.
 * @param data - the data to be stored within the Node. 
 * @return Newly created TreeNode
 *
 */
TreeNode* createTreeNode(TreeDataPtr data);

/**
 * createBinTree allocates memory for Tree and assigned function pointers
 * @param  compare [Function pointer to Compare data in tree nodes]
 * @param  del     [Function pointer to delete data from node]
 * @return Newly created Tree
 */
Tree createBinTree(int (*compareFunction)(const void* first,const void* second), void (*deleteFunction)(void* toBeDeleted));

/**
 * destroyBinTree - remove all items and free memory
 * @param  toDestroy - the tree
 * @return
 */
void  destroyBinTree(Tree* toDestroy);

/**
 * Add data to a tree
 * @param theTree 
 * @param data    
 */
void addToTree(Tree* theTree, TreeDataPtr data);

/**
 * Remove data from the tree
 * @param theTree 
 * @param data    
 */
void removeFromTree(Tree* theTree, TreeDataPtr data);


/**
 * This function searches the tree for the target data
 * @param  theTree 
 * @param  data    
 * @return NULL if fail, otherwise return data
 */
TreeDataPtr findInTree(Tree theTree, TreeDataPtr data);

/**
 * Get data from the root of the Tree if it exists.
 * @param  theTree 
 * @return NULL if tree is empty, otherwise return root
 */
TreeDataPtr getRootData(Tree theTree);

/**
 * [printInOrder This function prints the tree using an inOrder traversal
 * @param theTree   [description]
 * @param printData [description]
 */
void printInOrder(Tree theTree, PrintFunc printData);

/**
 * [printPreOrder This function prints the tree using an preOrder traversal
 * @param theTree   [description]
 * @param printData [description]
 */
void printPreOrder(Tree theTree, PrintFunc printData);

/**
 * [printPostOrder This function prints the tree using an postOrder traversal
 * @param theTree   [description]
 * @param printData [description]
 */
void printPostOrder(Tree theTree, PrintFunc printData);

/**
 * Checks if a tree is empty
 * @param  theTee [description]
 * @return false if not empty, true otherwise
 */
bool isTreeEmpty(Tree theTee);


/**
 * Helper function for checking if a single node is a leaf (no children)
 * @param  treeNode [description]
 * @return false if not a leaf, true otherwise
 */
bool isLeaf(TreeNode * treeNode);

/**
 * Helper funciton for checking if a single node has two children.
 * @param  treeNode [description]
 * @return false if has two children, true otherwise
 */
bool hasTwoChildren( TreeNode *treeNode);

/**
 * Helper funciton Get the height of a particulat Node in the tree. 
 * @param  treeNode [description]
 * @return    (1-Based) heigh for the tree. 
 */
int getHeight( TreeNode* treeNode );


/**
 * We may add additional API functions in later assignments
 */


#endif
