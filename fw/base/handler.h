#ifndef __HANDLER_H
#define __HANDLER_H

#include "collection/array.h"

	NS_FW_BASE_BEGIN

		#define HANDLER_FUNC(h, T) int(h)(void* context, T value, void* args)

		template <typename T> struct HandlerItem {
			void* context;
			HANDLER_FUNC(*handler, T);
			void* args;
		};

		template <typename T> class Handler {

			Array items_;
		public:
			Handler() {
				items_.init(sizeof(HandlerItem<T>), 4);
			}
			~Handler() {
				items_.clear();
			}

			void operator()(T t) {
				for (var i = 0; i < items_.length(); i++) {
					var item = (HandlerItem<T>*)items_.get(i);
					item->handler(item->context, t, item->args);
				}
			}

			void add(void* context, HANDLER_FUNC(*fn, T), void* args = NULL) {
				HandlerItem<T> item;
				item.context = context;
				item.handler = fn;
				item.args = args;
				items_.push(&item);
			};
			void removeAt(int ix) {
				return items_.remove(ix);
			}
			void clear() {
				items_.clear();
			}
		};

	NS_FW_BASE_END

#endif
