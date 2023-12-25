#ifndef __HANDLER_H
#define __HANDLER_H

#include "collection/array.h"

	NS_FW_BASE_BEGIN

		template <typename T> int HandlerFunc(void* context, T value, void* args = NULL);

		template <typename T> struct HandlerItem {
			void* context;
			int(*handler)(void* context, T value, void* args);	// = HandlerFunc<T>
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
					item->handler(item->context, t, item->args);
				}
			}

			void add(void* context, int(*fn)(void* context, T value, void* args), void* args = NULL) {
				HandlerItem<T> item;
				item.context = context;
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
