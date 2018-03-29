#ifndef __TREE_H
#define __TREE_H

#include "base/Object.h"
#include "collection/Array.h"
#include "collection/Map.h"

NS_FW_BASE_BEGIN

#define TYPE_TREE GET_TYPE(Tree)

class Tree;
class Node;
class Edge : public Object {
	friend class RunTime;
	DECL_TYPE(Edge);
	PROP_R(Node*, a);
	PROP_R(Node*, b);
	PROP(Object*, value);
public:
	static int compare(Object*, Object*);
	Edge(Node*, Node*, Object* = NULL);
	~Edge(void);
};

class Node : public Object {
	friend class RunTime;
	DECL_TYPE(Node);
	PROP(Tree*, tree);
	PROP(Object*, value);
	PROP(size_t, flag);
	PROP_R(Map*, edges);
public:
	Node(void);
	Node(Object*);
	Node(Tree*, Object*);
	void init(Tree*, Object*);
	~Node();
};

class Tree : public Object {
	friend class RunTime;
	DECL_TYPE(Tree);
protected:
	PROP_R(Array*, nodes);
	PROP_R(Map*, edges);
	PROP_R(Node*, root);

public:
	Tree();
	void init();
	virtual ~Tree();

	Node* addNode(Node*, Object* = NULL, Object* = NULL);
	Edge* addEdge(Node*, Node*, Object*);
	void cleanUp();
	//Edge* removeEdge(Node*, Node*);
	//Node* removeNode(Node*);

	Node* traverseDFS(Function, Function, Function, Object*, Node* = NULL);
};

NS_FW_BASE_END

#endif