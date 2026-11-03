#pragma once

// c++ headers ------------------------------------------
#include <cstdint>

// public project headers -------------------------------
#include <mctrl/public/mctrl_types.h>

namespace mctrl {

/// Platform-specific controller backend. Exactly one implementation is compiled per target.
class IBackend {
public:
    virtual ~IBackend() = default;

    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    /// Polls the platform and refreshes controller state.
    virtual bool Update() = 0;

    virtual bool OnEnumerateDevices(uint32_t* inout_device_count, MctrlDeviceHandle* out_device_handles) = 0;

    virtual MctrlResult OnGetDeviceInfo(MctrlDeviceHandle device_handle, MctrlDeviceInfo** out_p_device_info) = 0;
};

}  // namespace mctrl
