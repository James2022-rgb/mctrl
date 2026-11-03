#include "backend_win32.h"

namespace mctrl {

namespace {

class BackendWin32 final : public IBackendWin32 {
public:
    bool Initialize() override {
        return true;
    }

    void Shutdown() override {
    }

    bool Update() override {
        return true;
    }

    bool OnEnumerateDevices(uint32_t* inout_device_count, [[maybe_unused]] MctrlDeviceHandle* out_device_handles) override {
        *inout_device_count = 0;
        return true;
    }

    MctrlResult OnGetDeviceInfo([[maybe_unused]] MctrlDeviceHandle device_handle, MctrlDeviceInfo** out_p_device_info) override {
        *out_p_device_info = nullptr;
        return MCTRL_RESULT_NOT_FOUND;
    }
};

}  // namespace

mbase::IAllocator::UniquePtr<IBackendWin32> IBackendWin32::Create(mbase::IAllocator* allocator) {
    return allocator->MakeUnique<BackendWin32>();
}

}  // namespace mctrl
