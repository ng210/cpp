#include "collection/Tree.h"
#include "base/MemoryMgr.h"

NS_FW_BASE_BEGIN

const Type* Edge::classType_;

Edge::Edge(Node* a, Node* b, Object* value) {
	a_ = a;
	b_ = b;
	value_ = value;
}

Edge::~Edge(void) {

}

const Type* Node::classType_;

Node::Node(void) {
	init(NULL, NULL);
}
Node::Node(Object* value) {
	init(NULL, value);
}
Node::Node(Tree* tree, Object* value) {
	init(tree, value);
}
Node::~Node() {
	edges_->cleanUp();
	DEL_(edges_);
}

void Node::init(Tree* tree, Object* value) {
	flag_ = 0;
	tree_ = tree;
	value_ = value;
	edges_ = NEW_(Map);
	edges_->compare(Edge::compare);
}

const Type* Tree::classType_;

int Edge::compare(Object* a, Object* b) {
	return (int)(a - b);
}

Tree::Tree() {
	init();
}
Tree::~Tree() {
	DEL_(nodes_);
	DEL_(edges_);
}

void Tree::init() {
	nodes_ = NEW_(Array);
	edges_ = NEW_(Map);
	edges_->compare(Edge::compare);
	root_ = NULL;
}
Node* Tree::addNode(Node* parentNode, Object* nodeValue, Object* edgeValue) {
	Node* node = NEW_(Node, this, nodeValue);
	Node* childNode = node;
	if (parentNode == NULL || root_ == NULL) {
		childNode = root_;
		parentNode = node;
		root_ = node;
	}
	nodes_->push(node);
	if (childNode != NULL) {
		addEdge(parentNode, childNode, edgeValue);
	}
	return node;
}
Edge* Tree::addEdge(Node* a, Node* b, Object* edgeValue) {
	Edge* edge = NULL;
	if (a != NULL && b != NULL) {
		edge = NEW_(Edge, a, b, edgeValue);
		a->edges()->put(b, edge);
		edges_->put(edge, edge);
	}
	return edge;
}
void Tree::cleanUp() {
	for (size_t i = 0; i < nodes_->length(); i++) {
		DEL_(((Node*)nodes_->get(i))->value());
	}
	nodes_->cleanUp();
	//for (size_t i = 0; i < edges_->keys()->length(); i++) {
	//	DEL_(((Edge*)edges_->keys()->get(i))->value());
	//}
	edges_->keys()->cleanUp();
	edges_->cleanUp();
}
//Edge* Tree::removeEdge(Node* a, Node* b) {
//	Edge* res = NULL;
//
//	return res;
//}
//Node* Tree::removeNode(Node* node) {
//	return node;
//}

Node* Tree::traverseDFS(Function pre, Function in, Function post, Object* args, Node *root) {
	Node* res = NULL;
	Node* node = root == NULL ? root_ : root;
	node->flag(node->flag() + 1);
	if (pre == NULL || !pre(2, node, args)) {
		Array& edges = *node->edges()->values();
		for (size_t i = 0; i < edges.length(); i++) {
			Edge& edge = *(Edge*)edges.get(i);
			if (in == NULL || !in(2, edge, args)) {
				Node* targetNode = (Node*)edge.b();
				if (targetNode->flag() == 0) {
					if ((res = traverseDFS(pre, in, post, args, targetNode)) != NULL) {
						break;
					}
				}
			}
		}
		if (res == NULL) {
			if (post == NULL || post(2, node, args)) {
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