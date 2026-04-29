#include "command_queue.h"

#include "core/os/memory.h"
#include "core/os/os.h"

#include <stdlib.h>

Error CommandQueue::push_commandp(const CallableMethod &p_method, const Variant **p_args, int p_argcount) {
	// Allocate a new page if needed
	if (PAGE_SIZE - bytes_per_page[allocated_memory.page_count - 1] < sizeof(Message)) {
		uint8_t *new_memory =
			(uint8_t *)Memory::vreallocate(allocated_memory.mem, ++allocated_memory.page_count * PAGE_SIZE);
		ERR_COND_NULL_R(new_memory, ERR_OUT_OF_MEMORY);
		allocated_memory.mem = new_memory;
		allocated_memory.page_offset = 0;
		if (bytes_per_page.size() < allocated_memory.page_count) {
			bytes_per_page.push_back(0);
		}
	}
	// Get next possible location on the heap
	uint32_t &offset = bytes_per_page[allocated_memory.page_count - 1];
	uint8_t *mem = allocated_memory.mem + (allocated_memory.page_count - 1) * PAGE_SIZE;

	Message *mptr = vnew_placement(mem + offset, Message);
	offset += sizeof(Message);
	mptr->callable = p_method;
	mptr->argcount = p_argcount;

	if (PAGE_SIZE - offset < sizeof(Variant) * p_argcount) {
		uint8_t *new_memory =
			(uint8_t *)Memory::vreallocate(allocated_memory.mem, ++allocated_memory.page_count * PAGE_SIZE);
		ERR_COND_NULL_R(new_memory, ERR_OUT_OF_MEMORY);
		allocated_memory.mem = new_memory;
		allocated_memory.page_offset = 0;
		if (bytes_per_page.size() < allocated_memory.page_count) {
			bytes_per_page.push_back(0);
		}
		mem = allocated_memory.mem + PAGE_SIZE * (allocated_memory.page_count - 1);
		offset = bytes_per_page[allocated_memory.page_count - 1];
	}

	if (p_args && p_argcount > 0) {
		for (int i = 0; i < p_argcount; i++) {
			Variant *v = vnew_placement(mem + offset, Variant);
			offset += sizeof(Variant);
			*v = *p_args[i];
		}
	}

	return OK;
}

void CommandQueue::flush() {
	uint32_t page = 0;
	uint32_t offset = 0;
	while (page < allocated_memory.page_count && offset < bytes_per_page[page]) {
		Message *m = (Message *)(allocated_memory.mem + (page * PAGE_SIZE) + offset);
		offset += sizeof(Message);
		// Find arguments
		uint32_t varmem = m->argcount * sizeof(Variant);
		if (PAGE_SIZE - offset < varmem) {
			page++;
			offset = 0;
		}

		Variant *args = (Variant *)(allocated_memory.mem + (PAGE_SIZE * page) + offset);

		// Construct argptrs
		Variant **argptrs = nullptr;
		if (m->argcount > 0) {
			argptrs = (Variant **)alloca(sizeof(Variant *) * m->argcount);
			for (int i = 0; i < m->argcount; i++) {
				argptrs[i] = &args[i];
			}
		}

		Error err;
		m->callable.callp((const Variant **)argptrs, err);
		if (err != OK) {
			OS::get_singleton()->printerr("Callable returned error to queue of \'%s\'.", get_error_message(err));
		}

		// Destruct elements now they've been unused.
		for (int i = 0; i < m->argcount; i++) {
			args[i].~Variant();
		}
		m->~Message();

		// Reach end of page
		if (offset == bytes_per_page[page]) {
			page++;
			offset = 0;
		}
	}

	bytes_per_page[0] = 0;
	allocated_memory.page_count = 1;
}

void CommandQueue::clear() {
	uint32_t page = 0;
	uint32_t offset = 0;
	while (page < allocated_memory.page_count && offset < bytes_per_page[page]) {
		Message *m = (Message *)(allocated_memory.mem + (page * PAGE_SIZE) + offset);
		offset += sizeof(Message);
		if (offset < sizeof(Variant) * m->argcount) {
			page++;
			offset = 0;
		}
		Variant *args = (Variant *)(allocated_memory.mem + (page * PAGE_SIZE) + offset);
		for (int i = 0; i < m->argcount; i++) {
			args[i].~Variant();
			offset += sizeof(Variant);
		}

		m->~Message();

		if (offset == bytes_per_page[page]) {
			page++;
			offset = 0;
		}
	}

	bytes_per_page[0] = 0;
	allocated_memory.page_count = 1;
}

CommandQueue::CommandQueue() {
	allocated_memory.mem = (uint8_t *)Memory::vallocate_zeroed(PAGE_SIZE);
	allocated_memory.page_count = 1;
	bytes_per_page.push_back(0);
}

CommandQueue::~CommandQueue() {
	clear();

	if (allocated_memory.mem) {
		Memory::vfree(allocated_memory.mem);
	}
}

GlobalCommandQueue *GlobalCommandQueue::singleton = nullptr;

GlobalCommandQueue *GlobalCommandQueue::get_singleton() {
	return singleton;
}

GlobalCommandQueue::GlobalCommandQueue() {
	singleton = this;
}

GlobalCommandQueue::~GlobalCommandQueue() {
	singleton = nullptr;
}
