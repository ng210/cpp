#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Array {
	size_t length;
	void* data;

	void* add(void *el) {
		void *res = NULL;
		for (int i = 0; i < this->length; i++) {

		}
	}
	void* remove(void *el) {
		void *res = NULL;
		for (int i = 0; i < this->length; i++) {

		}
	}
};

class Tree;
class TreeEdge;

class TreeNode {
	friend class Tree;
	Tree* parent;
	void* data;
	TreeEdge *edges[64];
	size_t edgeCount;

	TreeNode(Tree *parent) {
		this->parent = parent;
		this->data = NULL;
		memset(this->edges, 0, 64 * sizeof(TreeEdge*));
		this->edgeCount = 0;
	}
	TreeEdge* addEdge(TreeNode *node) {
		TreeEdge *edge = NULL;
		if (this->edgeCount < 64 && node->edgeCount < 64) {
			edge = new TreeEdge(this, node);
			this->edges[this->edgeCount++] = edge;
			node->edges[node->edgeCount++] = edge;
		}
		return edge;
	}
	TreeNode* removeEdge(TreeEdge *edge) {
		TreeNode *node = NULL;
		for (size_t i = 0; i < this->edgeCount; i++) {
			if (this->edges[i]->second == node) {
				this->edgeCount--;
				for (; i < this->edgeCount; i++) {
					this->edges[i] = this->edges[i + 1];
				}
				this->edges[i] = NULL;
				res = node;
				break;
			}
		}
		return node;
	}
public:
	Tree* getParent() { return this->parent; }
	void setData(void *data) { this->data = data; }
	void* getData() { return this->data; }
};

class TreeEdge {
	friend class Tree;
	friend class TreeNode;
	TreeNode *first;
	TreeNode *second;
	void *data;
	void init(TreeNode *first, TreeNode *second, void* data) {
		this->first = first;
		this->second = second;
		this->data = data;
	}
	TreeEdge(TreeNode *first, TreeNode *second, void* data) {
		init(first, second, data);
	}
	TreeEdge(TreeNode *first, TreeNode *second) {
		init(first, second, NULL);
	}
};

class Tree {
	TreeNode *nodes[255];
	size_t nodeCount;
	TreeEdge *edges[1024];
	size_t edgeCount; 
public:
	Tree() {
		nodeCount = 0;
		memset(nodes, 0, 255 * sizeof(TreeNode*));
	}
	TreeNode* addNode() {
		TreeNode *node = NULL;
		if (this->nodeCount < 255) {
			node = new TreeNode(this);
			this->nodeCount++;
		}
		return node;
	}
	TreeNode* removeNode(TreeNode *node) {
		TreeNode *res = NULL;
		if (node->parent == this) {
			for (int i = 0; i < this->nodeCount; i++) {
				TreeNode *node = this->nodes[i];

			}
			node = new TreeNode(this);
			this->nodeCount--;
		}
		return res;
	}
	TreeEdge* addEdge(TreeNode* first, TreeNode* second, void *data) {
		TreeEdge *edge = NULL;
		if (this->edgeCount < 1024) {
			edge = first->addEdge(second);
			if (edge != NULL) {
				this->edges[this->edgeCount++] = edge;
			}
		}
		return edge;
	}
	int removeEdge(TreeEdge *edge) {
		int res = 0;
		for (int i = 0; i < this->edgeCount; i++) {
			if ()
		}
	}
	TreeNode* removeLink(TreeNode* first, TreeNode* second) {
		return first->removeLink(second);
	}
};