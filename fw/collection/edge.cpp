#include "base/memory.h"
#include "collection/edge.h"

NS_FW_BASE_USE

Edge::Edge(Vertex* from, Vertex* to, void* data) {
	graph_ = NULL;
	from_ = from;
	to_ = to;
	data_ = data;
}

Edge::~Edge() {
	if (data_ != NULL) FREE(data_);
}

int Edge::defaultHandler(Graph* graph, Edge* edge, ...) {
	return 1;
}