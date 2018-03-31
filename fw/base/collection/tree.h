#ifndef __TREE_H
#define __TREE_H

//#include "collection/Array.h"
#include "collection/map.h"

NS_FW_BASE_BEGIN

class Node;
class Edge {
protected: PROP_R(Node*, from);
protected: PROP_R(Node*, to);
protected: PROP(void*, value);
public:
	//static int compare(Object*, Object*);
	Edge(Node* from, Node* to, void* value = NULL);
	virtual ~Edge(void);
};

class Tree;
class Node {
protected: PROP(Tree*, tree);
protected: PROP(void*, value);
protected: PROP(size_t, flag);
protected: PROP_R(Map*, edges);
public:
	//Node(void);
	//Node(void*);
	Node(Tree* parent, void* value = NULL);
	void init(Tree* parent, void* value);
	virtual ~Node();
};

typedef int (TreeCallback)(void* item, size_t argc, ...);

class Tree {
	static TreeCallback createNode_;
	static TreeCallback deleteNode_;
	static TreeCallback createEdge_;
	static TreeCallback deleteEdge_;

protected: PROP_R(Array*, nodes);
protected: PROP_R(Array*, edges);
protected: PROP_R(Node*, root);

public:
	TreeCallback* createNode;
	TreeCallback* deleteNode;
	TreeCallback* createEdge;
	TreeCallback* deleteEdge;

	Tree();
	void init();
	virtual ~Tree();

	Node* addNode(Node* parentNode, void* nodeValue = NULL, void* edgeValue = NULL);
	Edge* addEdge(Node* from, Node* to, void* value);
	//void cleanUp();
	//Edge* removeEdge(Node*, Node*);
	//Node* removeNode(Node*);
	Node* get(const int* path);
	Node* traverseDFS(TreeCallback pre, TreeCallback in, TreeCallback post, void* args, Node* root = NULL);
};

NS_FW_BASE_END

#endif