#include <stdarg.h>
#include <math.h>
#include "base/memory.h"
#include "base/string.h"
#include "collection/array.h"
#include "collection/graph.h"

NS_FW_BASE_USE

Graph::Graph() {
	root_ = NULL;
	vertices_.init(4);
	vertices_.compare(Graph::defVertexCompare);
	edges_.init(16);
	vertices_.compare(Graph::defEdgeCompare);
	createVertexHandler = Graph::defCreateVertexHandler;
	deleteVertexHandler = Graph::defDeleteVertexHandler;
	createEdgeHandler = Graph::defCreateEdgeHandler;
	deleteEdgeHandler = Graph::defDeleteEdgeHandler;
	compare_ = vertices_.compare();
}

Graph::~Graph() {
	vertices_.apply([](COLLECTION_ARGUMENTS) {
		DEL_((Vertex*)value);
		return value;
	});
	edges_.apply([](COLLECTION_ARGUMENTS) {
		DEL_((Edge*)value);
		return value;
	});
}

Vertex* Graph::createVertex(void* data, Vertex* parent) {
	Vertex* vertex = createVertexHandler(this, parent, data);
	if (vertex != NULL) {
		vertex->graph(this);
		if (root_ == NULL) {
			root_ = vertex;
		}
		vertices_.push(vertex);
	}
	return vertex;
}

Edge* Graph::createEdge(Vertex* from, Vertex* to, void* data) {
	var edge = createEdgeHandler(this, from, to, data);
	if (edge != NULL) {
		edge->graph(this);
		edges_.push(edge);
	}
	return edge;
}

Vertex* Graph::deleteVertex(Vertex* vertex) {
	vertex = deleteVertexHandler(this, vertex);
	if (vertex != NULL) {
		removeVertex(vertex);
	}
	return vertex;
}

Edge* Graph::deleteEdge(Edge* edge) {
	edge = deleteEdgeHandler(this, edge);
	if (edge) {
		removeEdge(edge);
		DEL_(edge);
	}
	return edge;
}


#pragma region Collection
void* Graph::add(Key key, void* item) {
	return addVertex((Vertex*)key.p, item, NULL);
}
void* Graph::get(Key key) {
	Key found = -1;
	return vertices_.search(key, found);
}
void* Graph::insert(Key key, void* item) {
	return add(key, item);
}
void Graph::remove(Key key) {
	Key found = -1;
	var vertex = (Vertex*)search(key, found);
	if (vertex) {
		removeVertex(vertex);
	}
}
void Graph::set(Key key, void* item) {
	Key found = -1;
	var vertex = (Vertex*)search(key, found);
	if (vertex) {
		vertex->setData(item);
	}
}
void Graph::set(void** oldItem, void* newItem) {
	var vertex = (Vertex*)oldItem;
	vertex->setData(newItem);
}

int Graph::apply(COLLECTION_ACTION action, ...) {
	va_list args;
	va_start(args, action);
	var res = -1;
	for (var ix = 0; ix < vertices_.length(); ix++) {
		if (!action(vertices_.data()[ix], ix, this, args)) {
			res = ix;
			break;
		}
	}
	va_end(args);
	return res;
}
void Graph::fill(void* value) {
	for (var i = 0; i < vertices_.length(); i++) {
		vertices_.set(i, value);
	}
}
void* Graph::search(Key key, Key& found, COLLECTION_COMPARE* compare) {
	if (compare == NULL) compare = compare_;
	found.i = -1;
	return vertices_.search(key, found, compare);
}
#pragma endregion


Vertex* Graph::addVertex(Vertex* parent, void* vertexData, void* edgeData) {
	if (!parent) parent = root_;
	var vertex = createVertex(vertexData, parent);
	if (vertex != root_) {
		addEdge(parent, vertex, edgeData);
	}
	return vertex;
}

void Graph::removeVertex(Vertex* vertex) {
	var id = vertex->id();
	if (vertices_.get(id) == vertex && !deleteVertexHandler(this, vertex)) {
		vertices_.remove(id);
	}
}

Edge* Graph::addEdge(Vertex* from, Vertex* to, void* data) {
	var edge = createEdge(from, to, data);
	from->edges().push(edge);
	return edge;
}

void Graph::removeEdge(Edge* edge) {
	var ix = edges_.findIndex(edge);
	if (ix != -1 && !deleteEdgeHandler(this, edge)) {
		edges_.remove(ix);
		ix = edge->from()->edges().findIndex(edge);
		edge->from()->edges().remove(ix);
		ix = edge->to()->edges().findIndex(edge);
		edge->to()->edges().remove(ix);
	}
}

void Graph::DFS(Vertex* start, VVERTEXHANDLER preHandler, VVERTEXHANDLER postHandler, VEDGEHANDLER edgeHandler, ...) {
	va_list args;
	va_start(args, edgeHandler);
	// reset flags
	for (var i = 0; i < vertices_.length(); i++) {
		((Vertex*)vertices_.get(i))->flag() = VertexFlags::none;
	}

	if (!start) start = root_;
	PArray remaining(vertices_.length());
	remaining.add(start);
	var stop = false;
	start->flag(VertexFlags::queued);
	while (!stop && remaining.length() > 0) {
		var ix = remaining.length() - 1;
		var vertex = (Vertex*)remaining.get(ix);
		if (vertex->flag() == VertexFlags::queued) {
			if (preHandler != NULL) {
				if (preHandler(this, vertex, args)) break;
			}
			vertex->flag(VertexFlags::processing);
			for (var i = vertex->edges().length() - 1; i >= 0; i--) {
				var edge = (Edge*)vertex->edges().get(i);
				if (edge) {
					var child = edge->to();
					child->parent(vertex);
					if (child->flag() == VertexFlags::none) {
						child->flag(VertexFlags::queued);
						remaining.add(child);
						if (edgeHandler != NULL) {
							if (edgeHandler(this, edge, args)) {
								stop = true;
								break;
							}
						}
					}
				}
			}
		}
		else if (vertex->flag() == VertexFlags::processing) {
			if (postHandler != NULL) {
				if (postHandler(this, vertex, args)) break;
			}
			vertex->flag(VertexFlags::complete);
			remaining.remove(ix);
		}
	}
	va_end(args);
}

void Graph::BFS(Vertex* start, VVERTEXHANDLER preHandler, VVERTEXHANDLER postHandler, VEDGEHANDLER edgeHandler, ...) {
	va_list args;
	va_start(args, edgeHandler);
	// reset flags
	for (var i = 0; i < vertices_.length(); i++) {
		((Vertex*)vertices_.get(i))->flag(VertexFlags::none);
	}
	if (!start) start = root_;
	var level = 0;
	PArray remaining(vertices_.length());
	remaining.add(start);
	start->flag(VertexFlags::queued);
	if (preHandler != NULL && preHandler(this, start, args)) return;		// level

	var isStopped = false;
	while (!isStopped && remaining.length() > 0) {
		var vertex = (Vertex*)remaining.pop();
		if (postHandler != NULL && postHandler(this, start, args)) break;	// level
		for (var i = 0; i < vertex->edges().length(); i++) {
			var edge = (Edge*)vertex->edges().get(i);
			if (edge) {
				var child = edge->to();
				child->parent(vertex);
				if (child->flag() == VertexFlags::none) {
					if (edgeHandler != NULL) {
						if (!edgeHandler(this, edge, args)) {
							child->flag(VertexFlags::queued);
							if (preHandler != NULL) {
								if (!preHandler(this, child, args)) {		// level + 1
									remaining.insert(0, child);
								}
							}
							else {
								isStopped = true;
								break;
							}
						}
					}
				}
			}
		}
		level++;
	}
	va_end(args);
}

PArray* Graph::findPath(Vertex* start, Vertex* end, EDGEHANDLER checkEdge) {
	// distance(start, v)
	var links = NEW_(Array, vertices_.length());
	links->fill(NULL);
	if (checkEdge == NULL) checkEdge = Edge::defaultHandler;
	void* args[3] = { links, end, checkEdge };
	BFS(start,
		[](Graph* graph, Vertex* vertex, va_list args) {
			var links = va_arg(args, Array*);	// skip links
			var end = va_arg(args, Vertex*);
			var res = (int)(vertex == end);
			return res;
		},
		NULL,
		[](Graph* graph, Edge* edge, va_list args) {
			var links = va_arg(args, Array*);
			var end = va_arg(args, Vertex*);	// skip end
			var checkEdge = va_arg(args, EDGEHANDLER*);
			var res = 0;
			if (checkEdge(graph, edge)) {
				links->set(edge->to()->id(), edge);
			}
			else res = 1;
			return res;
		},
		links, end, checkEdge
	);
	PArray* path = NULL;
	var link = (Edge*)links->get(end->id());
	if (link != NULL) {
		var arr = NEW_(PArray);
		while (link != NULL) {
			arr->add(link);
			link = (Edge*)links->get(link->from()->id());
		}
		// reverse the array
		path = NEW_(PArray, arr->length());
		var j = arr->length() - 1;
		for (var i = 0; i <= j; i++) {
			path->add(arr->get(j));
			j--;
		}
		DEL_(path);
	}
	return path;
}

#pragma region Statics

Vertex* Graph::defCreateVertexHandler(Graph* graph, Vertex* parent, void* data) {
	return NEW_(Vertex, graph->vertices().length(), parent, data);
}
Vertex* Graph::defDeleteVertexHandler(Graph* graph, Vertex* vertex) {
	return vertex;
}
Edge* Graph::defCreateEdgeHandler(Graph* graph, Vertex* from, Vertex* to, void* data) {
	return NEW_(Edge, from, to, data);
}
Edge* Graph::defDeleteEdgeHandler(Graph* graph, Edge* edge) {
	return edge;
}

int Graph::defVertexCompare(COLLECTION_ARGUMENTS) {
	return ((Vertex*)value)->id() - key.i;
}
int Graph::defEdgeCompare(COLLECTION_ARGUMENTS) {
	var edge = (Edge*)value;
	return (int)((size_t)edge->data() - (size_t)key.p);
}

Graph* Graph::createComplete(int vertexCount, bool isDirected, VERTEXHANDLER vertexHandler, EDGEHANDLER edgeHandler) {
	var graph = NEW_(Graph);
	// add vertices
	var digits = (int)ceil(log10(vertexCount));
	var formatStr = str_format("v%%0%dd", digits);
	for (var i = 0; i < vertexCount; i++) {
		var label = str_format(formatStr, i);
		var vertex = graph->createVertex(label, NULL);
		if (vertexHandler != NULL) {
			vertexHandler(graph, vertex, i);
		}
	}
	// add edges
	var ei = 0;
	for (var i = 0; i < vertexCount; i++) {
		var from = (Vertex*)graph->vertices().get(i);
		for (var j = i + 1; j < vertexCount; j++) {
			var to = (Vertex*)graph->vertices().get(j);
			var label = str_format("%s->%s", from->data(), to->data());
			var edge = graph->addEdge(from, to, label);
			if (edgeHandler != NULL) {
				edgeHandler(graph, edge, ei);
			}
			ei++;
			if (!isDirected) {
				var label = str_format("%s->%s", to->data(), from->data());
				edge = graph->addEdge(to, from, label);
				if (edgeHandler != NULL) {
					edgeHandler(graph, edge, ei);
				}
				ei++;
			}
		}
	}
	return graph;
}

#pragma endregion
