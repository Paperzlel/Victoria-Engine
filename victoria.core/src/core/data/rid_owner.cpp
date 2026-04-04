#include "core/data/rid_owner.h"

VAPI AtomicCounter<uint64_t> RIDAllocatorBase::global_counter{1};