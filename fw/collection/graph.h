#ifndef __GRAPH_H
#define __GRAPH_H

#include "collection/edge.h"
#include "collection/vertex.h"

NS_FW_BASE_BEGIN

typedef Vertex* (CREATEVERTEXHANDLER)(Graph* graph, Vertex* parent, void* data);
typedef Vertex* (DELETEVERTEXHANDLER)(Graph* graph, Vertex* vertex);
typedef Edge* (CREATEEDGEHANDLER)(Graph* graph, Vertex* from, Vertex* to, void* data);
typedef Edge* (DELETEEDGEHANDLER)(Graph* graph, Edge* edge);

class Graph : public Collection {
protected: PROP_R(Vertex*, root);
protected: PROP_R(PArray, vertices);
protected: PROP_R(PArray, edges);
protected:
	Vertex* createVertex(void* data, Vertex* parent);
	Edge* createEdge(Vertex* from, Vertex* to, void* data);
	Vertex* deleteVertex(Vertex* vertex);
	Edge* deleteEdge(Edge* edge);

	static CREATEVERTEXHANDLER defCreateVertexHandler;
	static DELETEVERTEXHANDLER defDeleteVertexHandler;
	static CREATEEDGEHANDLER defCreateEdgeHandler;
	static DELETEEDGEHANDLER defDeleteEdgeHandler;
	static COLLECTION_COMPARE defVertexCompare;
	static COLLECTION_COMPARE defEdgeCompare;

public:
	Graph();
	virtual ~Graph();

	CREATEVERTEXHANDLER* createVertexHandler;
	DELETEVERTEXHANDLER* deleteVertexHandler;
	CREATEEDGEHANDLER* createEdgeHandler;
	DELETEEDGEHANDLER* deleteEdgeHandler;

	// Collection
	void* add(Key key, void* item);
	void* get(Key key);
	void* insert(Key key, void* item);
	void remove(Key key);
	void set(Key key, void* item);
	void set(void** oldItem, void* newItem);

	int apply(COLLECTION_ACTION, ...);
	void fill(void* value);
	void* search(Key key, Key& found, COLLECTION_COMPARE* compare = NULL);

	Vertex* addVertex(Vertex* parent, void* vertexData, void* edgeData);
	void removeVertex(Vertex* vertex);
	Edge* addEdge(Vertex* from, Vertex* to, void* data);
	void removeEdge(Edge* edge);

	void DFS(Vertex* start, VVERTEXHANDLER* preHandler, VVERTEXHANDLER* postHandler, VEDGEHANDLER* edgeHandler, ...);
	void BFS(Vertex* start, VVERTEXHANDLER* preHandler, VVERTEXHANDLER* postHandler, VEDGEHANDLER* edgeHandler, ...);
	PArray* findPath(Vertex* start, Vertex* end, EDGEHANDLER checkEdge);

	static Graph* createComplete(int vertexCount, bool isDirected, VERTEXHANDLER* vertexHandler, EDGEHANDLER* edgeHandler);
};

NS_FW_BASE_END

#endif