#ifndef __HANDLER_H
#define __HANDLER_H

#include "collection/array.h"

	NS_FW_BASE_USE
		namespace SYNTH {

		// Handler definitions for class T and with argument A1
		// - struct HandlerItem<T, A1>
		// - function type int(void*, A1)
		// - function type int(T, A1)
	////#define CREATEHANDLER1(name, A1) \
	////		typedef int(name##Func)(void*, A1); \
	////		typedef struct name##Item { void *obj; name##Func *fn; name##Item(void *o, name##Func *f) : obj(o), fn(f) {} } name##Item; \
	////		class name##Handler{ \
	////			Array items_;\
	////		public: \
	////			name##Handler() { items_.init(sizeof(name##Item), 4); }\
	////			void operator()(A1 a1) { \
	////				for (var i = 0; i < items_.length(); i++) { \
	////						var item = (name##Item*)items_.get(i); \
	////						item->fn(item->obj, a1); \
	////				}\
	////			}\
	////			void add(void* o, name##Func* f) { name##Item item(o, f); items_.push(&item); }\
	////			void clear() { items_.clear(); }\
	////		};
	////	}


		template <typename T> int HandlerFunc(void*, T);

		template <typename T> struct HandlerItem {
			void* obj;
			int(*handler)(void*, T);
		};

		template <typename T> class Handler {

			Array items_;
		public:
			Handler() {
				items_.init(sizeof(HandlerItem<T>), 4);
			}
			~Handler() {
			}

			void operator()(T t) {
				for (var i = 0; i < items_.length(); i++) {
					var item = (HandlerItem<T>*)items_.get(i);
					item->handler(item->obj, t);
				}
			}

			template<typename U> void add(void* obj, int(fn)(void*, U)) {
				HandlerItem<U> item;
				item.obj = obj;
				item.handler = fn;
				items_.push(&item);
			};
			void clear() {
				items_.clear();
			}
		};

	}

#endif
