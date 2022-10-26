#pragma once
// only supporting vulkan for quite some time

namespace beet::gfx {
class IDevice {
   public:
    IDevice() = default;
    virtual bool init() { return false; }
};
}  // namespace beet::gfx
