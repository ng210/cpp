#include "base/memory.h"
#include "collection/vertex.h"

NS_FW_BASE_USE

Vertex::Vertex(int id, Vertex* parent, void* data) {
	id_ = id;
	graph_ = NULL;
	parent_ = parent;
	edges_.init(4);
	flag_ = VertexFlags::none;
	data_ = data;
}

Vertex::~Vertex() {
	if (data_) FREE(data_);
}

void* Vertex::setData(void* data) {
	var d = data_;
	data_ = data;
	return d;
}