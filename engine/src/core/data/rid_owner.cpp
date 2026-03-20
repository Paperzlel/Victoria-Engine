#include "core/data/rid_owner.h"

AtomicCounter<u64> RIDAllocatorBase::global_counter{ 1 };