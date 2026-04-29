#pragma once

#include "core/object/callable_method.h"
#include "core/typedefs.h"

/**
 * @brief A class that can have methods pushed to it and called later on. All events are called when `flush()` occurs.
 */
class VAPI CommandQueue {
	// A given message pushed to the queue.
	struct Message {
		CallableMethod callable; // The actual function to call
		int32_t argcount;		 // The number of arguments passed to the function
	};

	enum {
		PAGE_SIZE = 4096 // Number of bytes to a page. Aligned with the regular 4 KiB
	};

	// Kind of a paged allocator - memory is given in 4 KiB chunks to the user when it runs out of space.
	struct PageAllocator {
		uint8_t *mem;		  // A pointer to the beginning of the memory.
		uint32_t page_count;  // The number of pages allocated
		uint32_t page_offset; // The offset into the current page
	};

	PageAllocator allocated_memory;	 // The allocated memory struct
	Vector<uint32_t> bytes_per_page; // The bytes allocated to this specific page.

public:
	/**
	 * @brief Pushes a command onto the command heap. The arguments are either immediately following the command, or on
	 * the next available page from the method. It is advised to not call this method directly and instead call
	 * `call_deferred()` instead.
	 * @param p_method The callable to queue up
	 * @param p_args An array of argument pointers which are then pushed onto the stack.
	 * @param p_argcount The number of elements in the argument pointer array to push onto the stack
	 * @return `OK` on success, and `ERR_OUT_OF_MEMORY` if no more memory can be allocated.
	 */
	Error push_commandp(const CallableMethod &p_method, const Variant **p_args, int p_argcount);

	/**
	 * @brief Pushes a command onto the command heap, to be later executed by a call to `flush()`. Commands on the list
	 * cannot have return values as the value itself is likely to be ignored.
	 * @param p_method The callable to queue for being fired.
	 * @param p_args Variadic list of arguments that can be converted to `Variant`.
	 * @return `OK` on success, and `ERR_OUT_OF_MEMORY` if no more memory can be allocated.
	 */
	template <typename... Args>
	FORCE_INLINE Error push_command(const CallableMethod &p_method, Args... p_args) {
		Variant args[sizeof...(p_args) + 1] = {p_args..., Variant()};
		const Variant *argptrs[sizeof...(p_args) + 1];

		for (uint32_t i = 0; i < sizeof...(p_args); i++) {
			argptrs[i] = &args[i];
		}

		return push_commandp(p_method, sizeof...(p_args) == 0 ? nullptr : argptrs, sizeof...(p_args));
	}

	/**
	 * @brief Flushes all commands pending on the queue by calling the respective functions.
	 */
	void flush();

	/**
	 * @brief Clears and destructs all data left on the queue. Does not flush the queue, and any commands left being
	 * awaited will be destructed without being called. Call `flush()` before running this command to ensure that all
	 * commands are sent properly.
	 */
	void clear();

	CommandQueue();
	virtual ~CommandQueue(); // mark as virtual so it's called first
};

/**
 * @brief Global command queue that can be called from anywhere in the engine. Is initialized in `core_initialize` but
 * can be initialized elsewhere in the engine as well.
 */
class VAPI GlobalCommandQueue : public CommandQueue {
	static GlobalCommandQueue *singleton; // Singleton object.

public:
	/**
	 * @brief Obtains the global singleton object.
	 */
	static GlobalCommandQueue *get_singleton();

	GlobalCommandQueue();
	~GlobalCommandQueue();
};
