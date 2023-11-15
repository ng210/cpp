#ifndef __HANDLER_H
#define __HANDLER_H

#include "collection/array.h"

	NS_FW_BASE_BEGIN

		template <typename T> int HandlerFunc(void*, T, void* args = NULL);

		template <typename T> struct HandlerItem {
			void* obj;
			int(*handler)(void*, T, void*);
			void* args;
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
					item->handler(item->obj, t, item->args);
				}
			}

			template<typename T> void add(void* obj, int(fn)(void*, T, void*), void* args = NULL) {
				HandlerItem<T> item;
				item.obj = obj;
				item.handler = fn;
				item.args = args;
				items_.push(&item);
			};
			void clear() {
				items_.clear();
			}
		};

	NS_FW_BASE_END

#endif
