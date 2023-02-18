#ifndef __VERTEX_H
#define __VERTEX_H

#include "basedef.h"
#include "collection/parray.h"

NS_FW_BASE_BEGIN

class Graph;
class Vertex;
class Edge;
typedef int (VERTEXHANDLER)(Graph* graph, Vertex* vertex, ...);
typedef int (VVERTEXHANDLER)(Graph* graph, Vertex* vertex, va_list args);

enum class VertexFlags {
	none = 0,
	queued = 1,
	processing = 2,
	complete = 3
};


class Vertex {
protected: PROP(int, id);
protected: PROP(Graph*, graph);
protected: PROP(Vertex*, parent);
protected: PROP_R(PArray, edges);
protected: PROP(VertexFlags, flag);
protected: PROP(void*, data);
public:
	Vertex(int id, Vertex* parent, void* data);
	virtual ~Vertex();

	virtual void* setData(void* data);
};


NS_FW_BASE_END

#endif