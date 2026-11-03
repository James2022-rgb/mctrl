#pragma once

// standard headers -------------------------------------
#if defined(__cplusplus)
# include <cstdint>
#else
# include <stdint.h>
#endif

// public project headers -------------------------------
#if defined(__cplusplus)
# include <mbase/public/type_safety.h>
#endif

#define MCTRL_INVALID_CONTEXT_HANDLE 0
#define MCTRL_INVALID_DEVICE_HANDLE 0

#if defined(__cplusplus)
extern "C" {
#endif

typedef uint32_t MctrlContextHandle;
typedef uint32_t MctrlDeviceHandle;

typedef enum MctrlResult {
    MCTRL_RESULT_SUCCESS = 0,
    MCTRL_RESULT_INCOMPLETE = 1,
    MCTRL_RESULT_FAILURE = -1,
    MCTRL_RESULT_INVALID_ARGUMENT = -2,
    MCTRL_RESULT_NOT_FOUND = -3,
} MctrlResult;

typedef struct MctrlUsbId {
    uint16_t vendor_id;
    uint16_t product_id;
} MctrlUsbId;

typedef struct MctrlDeviceInputLayout {
    uint32_t hat_count;
    uint32_t button_count;
    uint32_t axis_count;
} MctrlDeviceInputLayout;

typedef struct MctrlDeviceInfo {
    MctrlUsbId usb_id;
    MctrlDeviceInputLayout input_layout;
    char const* friendly_name;
    char const* name;
} MctrlDeviceInfo;

#if defined(__cplusplus)
} // extern "C"
#endif

#if defined(__cplusplus)

namespace mctrl {

using ContextHandle = mbase::TypesafeHandle<struct ContextHandleTag, MctrlContextHandle, MCTRL_INVALID_CONTEXT_HANDLE>;
using DeviceHandle = mbase::TypesafeHandle<struct DeviceHandleTag, MctrlDeviceHandle, MCTRL_INVALID_DEVICE_HANDLE>;

}  // namespace mctrl

#endif // defined(__cplusplus)
