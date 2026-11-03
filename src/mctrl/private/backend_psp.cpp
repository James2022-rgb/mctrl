#include "backend_psp.h"


// public project headers -------------------------------

// platform headers -------------------------------------
#include <pspctrl.h>

namespace mctrl {

namespace {

MctrlDeviceHandle s_device_handle = 1;

MctrlDeviceInfo s_device_info {
    .usb_id = { .vendor_id = 0, .product_id = 0 },
    .input_layout = {
        .hat_count = 0,
        .button_count = 12,
        .axis_count = 2,
    },
    .friendly_name = "PSP Controller",
    .name = "PSP Controller",
};

struct Device final {
    
};

class BackendPsp final : public IBackendPsp {
public:
    bool Initialize() override {
        return true;
    }

    void Shutdown() override {
    }

    bool Update() override {
        return true;
    }

    bool OnEnumerateDevices(uint32_t* inout_device_count, MctrlDeviceHandle* out_device_handles) override {
        *inout_device_count = 1;
        if (out_device_handles != nullptr) {
            out_device_handles[0] = s_device_handle;
        }
        return true;
    }

    MctrlResult OnGetDeviceInfo(MctrlDeviceHandle device_handle, MctrlDeviceInfo** out_p_device_info) override {
        if (device_handle != s_device_handle) {
            return MCTRL_RESULT_NOT_FOUND;
        }
        if (out_p_device_info != nullptr) {
            *out_p_device_info = &s_device_info;
        }
        return MCTRL_RESULT_SUCCESS;
    }

private:
};

}  // namespace

mbase::IAllocator::UniquePtr<IBackendPsp> IBackendPsp::Create(mbase::IAllocator* allocator) {
    return allocator->MakeUnique<BackendPsp>();
}

}  // namespace mctrl
