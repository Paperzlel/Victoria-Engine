#pragma once

#include "core/object/callable_method.h"
#include "core/typedefs.h"

class CommandQueue {
	// A given message pushed to the queue.
	struct Message {
		CallableMethod callable; // The actual function pointer that was called.
		uint32_t argcount;		 // The number of arguments passed
	};

	struct PageAllocator {
		static constexpr uint32_t PAGE_SIZE = 4096;
		uint8_t *mem;
		uint32_t page_count;
		uint32_t page_offset;
	};

	PageAllocator allocated_memory;
	Vector<uint32_t> bytes_per_page;

public:
	Error push_commandp(const CallableMethod &p_method, const Variant **p_args, int p_argcount);

	template <typename... Args>
	FORCE_INLINE Error push_command(const CallableMethod &p_method, Args... p_args) {
		Variant args[sizeof...(p_args) + 1] = {p_args..., Variant()};
		const Variant *argptrs[sizeof...(p_args) + 1];

		for (uint32_t i = 0; i < sizeof...(p_args); i++) {
			argptrs[i] = &args[i];
		}

		return push_commandp(p_method, argptrs, sizeof...(p_args));
	}

	void flush();
	void clear();

	CommandQueue();
	~CommandQueue();
};
