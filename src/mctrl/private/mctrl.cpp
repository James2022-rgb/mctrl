#include "mctrl/public/mctrl.h"

// c++ headers ------------------------------------------
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mutex>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

// public project headers -------------------------------
#include <mbase/public/allocator.h>
#include <mbase/public/generational_handle.h>
#include <mbase/public/platform.h>
#include <mbase/public/tsa.h>

// project headers -------------------------------------
#include "backend_iface.h"
#if MBASE_PLATFORM_PSP
# include "backend_psp.h"
#elif MBASE_PLATFORM_WEB
# include "backend_web.h"
#elif MBASE_PLATFORM_WINDOWS
# include "backend_win32.h"
#endif

namespace {

struct Context final {
    mbase::IAllocator::UniquePtr<mctrl::IBackend> backend;
};

class ContextTable final {
public:
    ContextTable() :
        arena_(mbase::DefaultAllocator::GetInstance(), mbase::DefaultAllocator::GetInstance())
    {
    }

    /// `context` is moved from only on success; on failure the caller still owns it.
    /// Returns an invalid handle when the table is full or out of memory.
    mctrl::ContextHandle Add(Context&& context) {
        mbase::LockGuard lock(mutex_);

        mctrl::ContextHandle handle;
        try {
            arena_.Emplace(&handle, std::move(context));
        } catch (std::bad_alloc const&) {
            return mctrl::ContextHandle::Invalid();
        }
        return handle;
    }

    /// Returns `std::nullopt` if `handle` is invalid or stale.
    std::optional<Context> Remove(mctrl::ContextHandle const& handle) {
        mbase::LockGuard lock(mutex_);

        return arena_.Extract(handle);
    }

    /// Calls `f(Context&)` under the table lock, so the context can't be destroyed meanwhile.
    /// Returns `invalid_result` if `handle` is invalid or stale.
    /// `f` must not call back into the table (the mutex is not recursive).
    template<class F>
    std::invoke_result_t<F, Context&> WithContext(mctrl::ContextHandle const& handle, std::invoke_result_t<F, Context&> invalid_result, F&& f) {
        mbase::LockGuard lock(mutex_);

        Context* context = arena_.Get(handle);
        if (!context) {
            return invalid_result;
        }
        return std::invoke(std::forward<F>(f), *context);
    }

private:
    mbase::Lockable<std::mutex> mutex_;
    mbase::GenerationalArena<Context, mctrl::ContextHandle> arena_ MBASE_GUARDED_BY(mutex_);
};

ContextTable& GetContextTable() {
    // Placement-new into static storage and never destroyed: no heap use, and contexts destroyed by
    // other static destructors still find a live table.
    alignas(ContextTable) static unsigned char storage[sizeof(ContextTable)];
    static ContextTable* const table = new (storage) ContextTable();
    return *table;
}

}

#if defined(__cplusplus)
extern "C" {
#endif

MbBool32 MCTRL_API MctrlCreateContext(MctrlContextHandle* out_context) {
    if (out_context == nullptr) {
        return MBASE_FALSE;
    }

    mbase::IAllocator* const allocator = mbase::DefaultAllocator::GetInstance();

    Context context;
#if MBASE_PLATFORM_PSP
    context.backend = mctrl::IBackendPsp::Create(allocator);
#elif MBASE_PLATFORM_WEB
    context.backend = mctrl::IBackendWeb::Create(allocator);
#elif MBASE_PLATFORM_WINDOWS
    context.backend = mctrl::IBackendWin32::Create(allocator);
#endif
    if (!context.backend || !context.backend->Initialize()) {
        return MBASE_FALSE;
    }

    mctrl::ContextHandle const handle = GetContextTable().Add(std::move(context));
    if (!handle.IsValid()) {
        context.backend->Shutdown();
        return MBASE_FALSE;
    }

    *out_context = handle.Get();
    return MBASE_TRUE;
}

MbBool32 MCTRL_API MctrlDestroyContext(MctrlContextHandle context) {
    // Shut down outside the table lock; the backend is freed with the context.
    std::optional<Context> ctx = GetContextTable().Remove(mctrl::ContextHandle(context));
    if (!ctx) {
        return MBASE_FALSE;
    }

    ctx->backend->Shutdown();
    return MBASE_TRUE;
}

MbBool32 MCTRL_API MctrlUpdate(MctrlContextHandle context) {
    return GetContextTable().WithContext(mctrl::ContextHandle(context), MBASE_FALSE, [](Context& ctx) {
        return ctx.backend->Update() ? MBASE_TRUE : MBASE_FALSE;
    });
}

MctrlResult MCTRL_API MctrlEnumerateDevices(MctrlContextHandle context, uint32_t* inout_device_count, MctrlDeviceHandle* out_device_handles) {
    if (inout_device_count == nullptr || out_device_handles == nullptr) {
        return MCTRL_RESULT_INVALID_ARGUMENT;
    }

    return GetContextTable().WithContext(mctrl::ContextHandle(context), MCTRL_RESULT_FAILURE, [&](Context& ctx) {
        return ctx.backend->OnEnumerateDevices(inout_device_count, out_device_handles) ? MCTRL_RESULT_SUCCESS : MCTRL_RESULT_FAILURE;
    });
}

MctrlResult MCTRL_API MctrlGetDeviceInfo(MctrlContextHandle context, MctrlDeviceHandle device_handle, MctrlDeviceInfo** out_p_device_info) {
    if (out_p_device_info == nullptr) {
        return MCTRL_RESULT_INVALID_ARGUMENT;
    }

    return GetContextTable().WithContext(mctrl::ContextHandle(context), MCTRL_RESULT_FAILURE, [&](Context& ctx) {
        return ctx.backend->OnGetDeviceInfo(device_handle, out_p_device_info);
    });
}

#if defined(__cplusplus)
} // extern "C"
#endif
