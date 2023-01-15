#include <stdarg.h>
#include "base/memory.h"
#include "collection/tree.h"

NS_FW_BASE_USE

Tree::Tree() : Graph() {
}
Tree::~Tree() {
}

//Node* Tree::addNode(Node* parentNode, void* nodeValue, void* edgeValue) {
//	Node* node = NULL;
//	createNode_(&node, 2, this, nodeValue);
//	Node* childNode = node;
//	if (parentNode == NULL || root_ == NULL) {
//		childNode = root_;
//		parentNode = node;
//		root_ = node;
//	}
//	nodes_->add(node);
//	if (childNode != NULL) {
//		Edge* edge = NULL;
//		addEdge(parentNode, childNode, edgeValue);
//	}
//	return node;
//}
//Edge* Tree::addEdge(Node* from, Node* to, void* edgeValue) {
//	Edge* edge = NULL;
//	if (from != NULL && to != NULL) {
//		createEdge_(&edge, 3, from, to, edgeValue);
//		from->edges()->put(to, edge);
//		edges_->add(edge);
//	}
//	return edge;
//}
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

Vertex* Tree::get(const int* path) {
	Vertex* ret = NULL;
	if (path != NULL) {
		int n = path[0];
		if (n != -1) {
			Vertex* node = (Vertex*)vertices_.get(n);
			int i = 1;
			while (node != NULL) {
				int n = path[i];
				if (n == -1) {
					ret = node;
					break;
				}
				var hasError = true;
				if (n < node->edges().length()) {
					var edge = (Edge*)node->edges().get(n);
					if (edge) {
						node = edge->to();
						hasError = false;
					}
				}
				if (hasError) break;
				i++;
			}
		}
	}
	return ret;
}

//Vertex* Tree::traverseDFS(TreeCallback pre, TreeCallback in, TreeCallback post, void* args, Node *root) {
//	Node* res = NULL;
//	Node* node = root == NULL ? root_ : root;
//	node->flag(node->flag() + 1);
//	if (pre == NULL || !pre(node, 1, args)) {
//		ArrayBase& edges = *node->edges()->values();
//		for (int i = 0; i < (int)edges.length(); i++) {
//			Edge* edge = (Edge*)edges.getAt(i);
//			if (in == NULL || !in(edge, 1, args)) {
//				Node* targetNode = (Node*)edge->to();
//				if (targetNode->flag() == 0) {
//					if ((res = traverseDFS(pre, in, post, args, targetNode)) != NULL) {
//						break;
//					}
//				}
//			}
//		}
//		if (res == NULL) {
//			if (post == NULL || post(node, 1, args)) {
//				res = node;
//			}
//		}
//	}
//	else {
//		res = node;
//	}
//	return res;
//}
