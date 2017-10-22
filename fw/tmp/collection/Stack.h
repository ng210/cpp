class Stack {
	void** buffer;
	size_t capacity;
	size_t size;
public:
	Stack(size_t cap) {
		this->buffer = new void*[cap];
		this->capacity = cap;
		this->size = 0;
	}
	~Stack(void) {
		delete[] this->buffer;
	}
	void push(void* el) {
		if (this->size < this->capacity) {
			this->buffer[this->size] = el;
			this->size++;
		}
	}
	void* pop() {
		void *el = NULL;
		if (this->size > 0) {
			this->size--;
			el = this->buffer[this->size];
		}
		return el;
	}
	void* top() {
		void *el = NULL;
		if (this->size > 0) {
			el = this->buffer[this->size - 1];
		}
		return el;
	}
	bool isEmpty() {
		return (size == 0);
	}
};