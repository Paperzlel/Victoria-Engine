#include "core/os/memory.h"

#include <stdlib.h>
#include <string.h>

AtomicCounter<u64> Memory::current_mem_usage;
AtomicCounter<u64> Memory::max_mem_usage;

u64 Memory::get_memory_usage() {
	return current_mem_usage.get();
}
u64 Memory::get_mem_max_usage() {
	return max_mem_usage.get();
}

void *Memory::vallocate(u64 p_size) {
	u8 *data = (u8 *)malloc(p_size + DATA_OFFSET);

	u64 *s = (u64 *)data;
	*s = p_size;

	u64 max = current_mem_usage.add(p_size);
	max_mem_usage.exchange_if_greater(max + p_size);

	return data + DATA_OFFSET;
}

void *Memory::vallocate_zeroed(u64 p_size) {
	void *ret = vallocate(p_size);
	vzero(ret, p_size);
	return ret;
}

void *Memory::vreallocate(void *p_block, u64 p_new_size) {
	if (!p_block) {
		return vallocate(p_new_size);
	}

	u8 *data = (u8 *)p_block;
	data -= DATA_OFFSET;

	data = (u8 *)realloc(data, p_new_size + DATA_OFFSET);

	u64 *s = (u64 *)data;

	u64 max = current_mem_usage.add(p_new_size - *s);
	max_mem_usage.exchange_if_greater(max + p_new_size - *s);

	*s = p_new_size;

	return data + DATA_OFFSET;
}

void Memory::vfree(void *p_block) {
	// If the block and/or its size are 0 then there shouldn't need to be a reason to free it
	if (!p_block) {
		return;
	}

	u64 *data = (u64 *)p_block;
	data -= (DATA_OFFSET / sizeof(size_t));

	current_mem_usage.sub(*data);

	free(data);
}

void Memory::vzero(void *p_block, u64 p_size) {
	memset(p_block, 0, p_size);
}

void Memory::vset_memory(void *p_dest, int p_value, u64 p_size) {
	memset(p_dest, p_value, p_size);
}

void *Memory::vcopy_memory(void *p_dest, void *p_source, u64 p_size) {
	return memcpy(p_dest, p_source, p_size);
}

void *operator new(size_t p_size, const char *p_description) {
	return Memory::vallocate(p_size);
}
