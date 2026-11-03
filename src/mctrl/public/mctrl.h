#pragma once

// public project headers -------------------------------
#include <mbase/public/bool.h>
#include <mbase/public/call.h>

#include <mctrl/public/mctrl_types.h>

/// Calling convention for the C API (reserved; empty for now).
#define MCTRL_CALL

/// `MCTRL_SHARED`: defined when mctrl is built or consumed as a shared library.
/// `MCTRL_BUILDING`: defined only while building mctrl itself.
#if defined(MCTRL_SHARED)
# if defined(_WIN32)
#  if defined(MCTRL_BUILDING)
#   define MCTRL_EXPORT __declspec(dllexport)
#  else
#   define MCTRL_EXPORT __declspec(dllimport)
#  endif
# elif defined(__GNUC__)
#  define MCTRL_EXPORT __attribute__((visibility("default")))
# else
#  define MCTRL_EXPORT
# endif
#else
# define MCTRL_EXPORT
#endif

#define MCTRL_API MCTRL_EXPORT MCTRL_CALL

#if defined(__cplusplus)
extern "C" {
#endif

MbBool32 MCTRL_API MctrlCreateContext(MctrlContextHandle* out_context);
MbBool32 MCTRL_API MctrlDestroyContext(MctrlContextHandle context);

MbBool32 MCTRL_API MctrlUpdate(MctrlContextHandle context);

MctrlResult MCTRL_API MctrlEnumerateDevices(MctrlContextHandle context, uint32_t* inout_device_count, MctrlDeviceHandle* out_device_handles);

MctrlResult MCTRL_API MctrlGetDeviceInfo(MctrlContextHandle context, MctrlDeviceHandle device_handle, MctrlDeviceInfo** out_p_device_info);

#if defined(__cplusplus)
} // extern "C"
#endif

#if defined(__cplusplus)

namespace mctrl {

// Inline so that only the C API crosses the shared library boundary.

inline bool CreateContext(ContextHandle* out_context) {
    if (out_context == nullptr) {
        return false;
    }
    MctrlContextHandle raw_context;
    if (!MctrlCreateContext(&raw_context)) {
        return false;
    }
    *out_context = ContextHandle(raw_context);
    return true;
}

inline bool DestroyContext(ContextHandle context) {
    return MctrlDestroyContext(context.Get()) != MBASE_FALSE;
}

}  // namespace mctrl

#endif // defined(__cplusplus)
