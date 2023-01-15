#ifndef __EDGE_H
#define __EDGE_H

#include "basedef.h"

NS_FW_BASE_BEGIN

class Edge;
class Graph;
typedef int EDGEHANDLER(Graph* graph, Edge*, ...);

class Vertex;
class Edge {
protected: PROP(Graph*, graph);
protected: PROP(Vertex*, from);
protected: PROP(Vertex*, to);
protected: PROP(void*, data);
public:
	Edge(Vertex* from, Vertex* to, void* data);
	virtual ~Edge();

	static EDGEHANDLER defaultHandler;
};

NS_FW_BASE_END

#endif