#pragma once

// public project headers -------------------------------
#include <mbase/public/allocator.h>

// project headers -------------------------------------
#include "backend_iface.h"

namespace mctrl {

class IBackendWin32 : public IBackend {
public:
    /// Returns an empty pointer on allocation failure.
    static mbase::IAllocator::UniquePtr<IBackendWin32> Create(mbase::IAllocator* allocator);
};

}  // namespace mctrl
