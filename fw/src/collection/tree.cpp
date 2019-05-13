#include <stdarg.h>
#include "collection/tree.h"
#include "base/memory.h"


NS_FW_BASE_BEGIN

Edge::Edge(Node* from, Node* to, void* value) {
	from_ = from;
	to_ = to;
	value_ = value;
}
Edge::~Edge(void) {
}

//int Edge::compare(void* a, void* b) {
//	return (int)(a - b);
//}


//Node::Node(void) {
//	init(NULL, NULL);
//}
//Node::Node(void* value) {
//	init(NULL, value);
//}
Node::Node(Tree* tree, void* value) {
	init(tree, value);
}
Node::~Node() {
	//edges_->cleanUp();
	DEL_(edges_);
}
void Node::init(Tree* tree, void* value) {
	flag_ = 0;
	tree_ = tree;
	value_ = value;
	edges_ = NEW_(Map, MAP_USE_REF, MAP_USE_REF);
	edges_->compare(Collection::compareByRef);
}

Tree::Tree() {
	init();
}
Tree::~Tree() {
	ARRAY_FOREACH(nodes_, deleteNode_(value, 1, this));
	DEL_(nodes_);
	ARRAY_FOREACH(edges_, deleteEdge_(value, 1, this));
	DEL_(edges_);
}
void Tree::init() {
	nodes_ = NEW_(PArray, 64);
	edges_ = NEW_(PArray, 256);
	edges_->compare(Collection::compareByRef);
	root_ = NULL;

	createNodeHandler = Tree::defCreateNodeHandler_;
	deleteNodeHandler = Tree::defDeleteNodeHandler_;
	createEdgeHandler = Tree::defCreateEdgeHandler_;
	deleteEdgeHandler = Tree::defDeleteEdgeHandler_;
}

int Tree::defCreateNodeHandler_(void* node, size_t argc, ...) {
	return 0;
}
int Tree::defDeleteNodeHandler_(void* node, size_t argc, ...) {
	FREE(((Node*)node)->value());
	return 0;
}
int Tree::defCreateEdgeHandler_(void* edge, size_t argc, ...) {
	return 0;
}
int Tree::defDeleteEdgeHandler_(void* edge, size_t argc, ...) {
	FREE(((Edge*)edge)->value());
	return 0;
}

int Tree::createNode_(void* ppNode, size_t argc, ...) {
	va_list args;
	Node* node = NULL;
	if (argc > 0) {
		va_start(args, argc);
		Tree* tree = va_arg(args, Tree*);
		void* value = argc > 1 ? va_arg(args, void*) : NULL;
		node = NEW_(Node, tree, value);
		createNodeHandler(node, 0);
	}
	*(Node**)ppNode = node;
	va_end(args);
	return node != NULL ? 0 : 1;
}
int Tree::deleteNode_(void* node, size_t argc, ...) {
	deleteNodeHandler(node, 0);
	DEL_((Node*)node);
	return 0;
}
int Tree::createEdge_(void* ppEdge, size_t argc, ...) {
	va_list args;
	Edge* edge = NULL;
	if (argc > 1) {
		va_start(args, argc);
		Node* from = va_arg(args, Node*);
		Node* to = va_arg(args, Node*);
		void* value = argc > 2 ? va_arg(args, void*) : NULL;
		edge = NEW_(Edge, from, to, value);
		createEdgeHandler(edge, 0);
	}
	*(Edge**)ppEdge = edge;
	va_end(args);
	return edge != NULL ? 0 : 1;
}
int Tree::deleteEdge_(void* edge, size_t argc, ...) {
	deleteEdgeHandler(edge, 0);
	DEL_(edge);
	return 0;
}

Node* Tree::addNode(Node* parentNode, void* nodeValue, void* edgeValue) {
	Node* node = NULL;
	createNode_(&node, 2, this, nodeValue);
	Node* childNode = node;
	if (parentNode == NULL || root_ == NULL) {
		childNode = root_;
		parentNode = node;
		root_ = node;
	}
	nodes_->add(node);
	if (childNode != NULL) {
		Edge* edge = NULL;
		addEdge(parentNode, childNode, edgeValue);
	}
	return node;
}
Edge* Tree::addEdge(Node* from, Node* to, void* edgeValue) {
	Edge* edge = NULL;
	if (from != NULL && to != NULL) {
		createEdge_(&edge, 3, from, to, edgeValue);
		from->edges()->put(to, edge);
		edges_->add(edge);
	}
	return edge;
}
//void Tree::cleanUp() {
//	for (size_t i = 0; i < nodes_->length(); i++) {
//		DEL_(((Node*)nodes_->get(i))->value());
//	}
//	nodes_->cleanUp();
//	//for (size_t i = 0; i < edges_->keys()->length(); i++) {
//	//	DEL_(((Edge*)edges_->keys()->get(i))->value());
//	//}
//	edges_->keys()->cleanUp();
//	edges_->cleanUp();
//}

//Edge* Tree::removeEdge(Node* a, Node* b) {
//	Edge* res = NULL;
//
//	return res;
//}
//Node* Tree::removeNode(Node* node) {
//	return node;
//}

Node* Tree::get(const int* path) {
	Node* ret = NULL;
	if (path != NULL) {
		int n = path[0];
		if (n != -1) {
			Node* node = (Node*)nodes_->getAt(n);
			int i = 1;
			while (node != NULL) {
				int n = path[i];
				if (n == -1) {
					ret = node;
					break;
				}
				if (n < node->edges()->size()) {
					node = (Node*)node->edges()->keys()->getAt(n);
				} else {
					break;
				}
				i++;
			}
		}
	}
	return ret;
}

Node* Tree::traverseDFS(TreeCallback pre, TreeCallback in, TreeCallback post, void* args, Node *root) {
	Node* res = NULL;
	Node* node = root == NULL ? root_ : root;
	node->flag(node->flag() + 1);
	if (pre == NULL || !pre(node, 1, args)) {
		ArrayBase& edges = *node->edges()->values();
		for (int i = 0; i < (int)edges.length(); i++) {
			Edge* edge = (Edge*)edges.getAt(i);
			if (in == NULL || !in(edge, 1, args)) {
				Node* targetNode = (Node*)edge->to();
				if (targetNode->flag() == 0) {
					if ((res = traverseDFS(pre, in, post, args, targetNode)) != NULL) {
						break;
					}
				}
			}
		}
		if (res == NULL) {
			if (post == NULL || post(node, 1, args)) {
				res = node;
			}
		}
	}
	else {
		res = node;
	}
	return res;
}

NS_FW_BASE_END