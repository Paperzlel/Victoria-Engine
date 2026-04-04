#pragma once

#include "core/typedefs.h"

class RIDAllocatorBase;

class RID {
	friend class RIDAllocatorBase;
	uint64_t _id = 0;

public:
	FORCE_INLINE bool operator==(const RID &p_other) const {
		return _id == p_other._id;
	}
	FORCE_INLINE bool operator!=(const RID &p_other) const {
		return _id != p_other._id;
	}

	FORCE_INLINE bool is_valid() const {
		return _id != 0;
	}

	FORCE_INLINE bool is_null() const {
		return _id == 0;
	}

	FORCE_INLINE uint64_t get_id() const {
		return _id;
	}

	FORCE_INLINE RID() {}
};
