#ifndef __TREE_H
#define __TREE_H

#include "collection/graph.h"

NS_FW_BASE_BEGIN

typedef int (TreeCallback)(void* item, size_t argc, ...);

class Tree : public Graph {

public:

	Tree();
	virtual ~Tree();

	//Node* addNode(Node* parentNode, void* nodeValue = NULL, void* edgeValue = NULL);
	//Edge* addEdge(Node* from, Node* to, void* value);
	//void cleanUp();
	//Edge* removeEdge(Node*, Node*);
	//Node* removeNode(Node*);
	Vertex* get(const int* path);
	//Vertex* traverseDFS(TreeCallback pre, TreeCallback in, TreeCallback post, void* args, Node* root = NULL);
};

NS_FW_BASE_END

#endif