#include "core/os/memory.h"

#include <stdlib.h>
#include <string.h>

AtomicCounter<uint64_t> Memory::current_mem_usage;
AtomicCounter<uint64_t> Memory::max_mem_usage;

uint64_t Memory::get_memory_usage() {
	return current_mem_usage.get();
}
uint64_t Memory::get_mem_max_usage() {
	return max_mem_usage.get();
}

void *Memory::vallocate(uint64_t p_size) {
	uint8_t *data = (uint8_t *)malloc(p_size + DATA_OFFSET);

	uint64_t *s = (uint64_t *)data;
	*s = p_size;

	uint64_t max = current_mem_usage.add(p_size);
	max_mem_usage.exchange_if_greater(max + p_size);

	return data + DATA_OFFSET;
}

void *Memory::vallocate_zeroed(uint64_t p_size) {
	void *ret = vallocate(p_size);
	vzero(ret, p_size);
	return ret;
}

void *Memory::vreallocate(void *p_block, uint64_t p_new_size) {
	if (!p_block) {
		return vallocate(p_new_size);
	}

	uint8_t *data = (uint8_t *)p_block;
	data -= DATA_OFFSET;

	data = (uint8_t *)realloc(data, p_new_size + DATA_OFFSET);

	uint64_t *s = (uint64_t *)data;

	uint64_t max = current_mem_usage.add(p_new_size - *s);
	max_mem_usage.exchange_if_greater(max + p_new_size - *s);

	*s = p_new_size;

	return data + DATA_OFFSET;
}

void Memory::vfree(void *p_block) {
	// If the block and/or its size are 0 then there shouldn't need to be a reason to free it
	if (!p_block) {
		return;
	}

	uint64_t *data = (uint64_t *)p_block;
	data -= (DATA_OFFSET / sizeof(size_t));

	current_mem_usage.sub(*data);

	free(data);
}

void Memory::vzero(void *p_block, uint64_t p_size) {
	memset(p_block, 0, p_size);
}

void Memory::vset_memory(void *p_dest, int p_value, uint64_t p_size) {
	memset(p_dest, p_value, p_size);
}

void *Memory::vcopy_memory(void *p_dest, const void *p_source, uint64_t p_size) {
	return memcpy(p_dest, p_source, p_size);
}

void *Memory::vmemmove(void *p_dest, const void *p_source, size_t p_count) {
	return memmove(p_dest, p_source, p_count);
}

void *operator new(size_t p_size, const char *p_description) {
	return Memory::vallocate(p_size);
}

#ifdef _MSC_VER
void operator delete(void *p_mem, const char *p_description) {
	ERR_COND_FATAL(true); // TODO: Give a reason
}
#endif