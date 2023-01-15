#ifndef __GRAPH_H
#define __GRAPH_H

#include "collection/edge.h"
#include "collection/vertex.h"

NS_FW_BASE_BEGIN

typedef Vertex* (CREATEVERTEXHANDLER)(Graph* graph, Vertex* parent, void* data);
typedef Edge* (CREATEEDGEHANDLER)(Graph* graph, Vertex* from, Vertex* to, void* data);

class Graph : public Collection {
protected: PROP_R(Vertex*, root);
protected: PROP_R(PArray, vertices);
protected: PROP_R(PArray, edges);
protected:
	Vertex* createVertex(void* data, Vertex* parent);
	Edge* createEdge(Vertex* from, Vertex* to, void* data);
	void deleteVertex(Vertex* vertex);
	void deleteEdge(Edge* edge);

	static CREATEVERTEXHANDLER defCreateVertexHandler;
	static VERTEXHANDLER defDeleteVertexHandler;
	static CREATEEDGEHANDLER defCreateEdgeHandler;
	static EDGEHANDLER defDeleteEdgeHandler;
	static COLLECTIONCALLBACK defVertexCompare;
	static COLLECTIONCALLBACK defEdgeCompare;

public:
	Graph();
	virtual ~Graph();

	CREATEVERTEXHANDLER* createVertexHandler;
	VERTEXHANDLER* deleteVertexHandler;
	CREATEEDGEHANDLER* createEdgeHandler;
	EDGEHANDLER* deleteEdgeHandler;

	// Collection
	void* add(Key key, void* item);
	void* get(Key key);
	void* insert(Key key, void* item);
	void remove(Key key);
	void set(Key key, void* item);
	void set(void** oldItem, void* newItem);

	int apply(COLLECTIONCALLBACK, ...);
	void fill(void* value);
	void* search(Key key, int& ix, COLLECTIONCALLBACK* compare = NULL);

	Vertex* addVertex(Vertex* parent, void* vertexData, void* edgeData);
	void removeVertex(Vertex* vertex);
	Edge* addEdge(Vertex* from, Vertex* to, void* data);
	void removeEdge(Edge* edge);

	void DFS(Vertex* start, VERTEXHANDLER* preHandler, VERTEXHANDLER* postHandler, EDGEHANDLER* edgeHandler, void* args);
	void BFS(Vertex* start, VERTEXHANDLER* preHandler, VERTEXHANDLER* postHandler, EDGEHANDLER* edgeHandler, void* args);
	PArray* findPath(Vertex* start, Vertex* end, EDGEHANDLER checkEdge);

	static Graph* createComplete(int vertexCount, bool isDirected, VERTEXHANDLER* vertexHandler, EDGEHANDLER* edgeHandler);
};

NS_FW_BASE_END

#endif