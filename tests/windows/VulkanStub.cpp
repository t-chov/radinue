#include <cstdint>

namespace {

constexpr std::int32_t initializationFailed = -3;

} // namespace

// The Windows libmpv build links to Vulkan even when Radinue disables video. CI runners do not
// have a graphics driver, so these test-only exports let the loader report Vulkan as unavailable.
extern "C" {

std::int32_t vkCreateDisplayPlaneSurfaceKHR(void *, const void *, const void *, void **) {
    return initializationFailed;
}

std::int32_t vkCreateWin32SurfaceKHR(void *, const void *, const void *, void **) {
    return initializationFailed;
}

void vkDestroySurfaceKHR(void *, void *, const void *) {}

std::int32_t vkEnumeratePhysicalDevices(void *, std::uint32_t *, void **) {
    return initializationFailed;
}

std::int32_t vkGetDisplayModePropertiesKHR(void *, void *, std::uint32_t *, void *) {
    return initializationFailed;
}

std::int32_t vkGetDisplayPlaneSupportedDisplaysKHR(void *, std::uint32_t, std::uint32_t *, void *) {
    return initializationFailed;
}

void *vkGetInstanceProcAddr(void *, const char *) { return nullptr; }

std::int32_t vkGetPhysicalDeviceDisplayPlanePropertiesKHR(void *, std::uint32_t *, void *) {
    return initializationFailed;
}

std::int32_t vkGetPhysicalDeviceDisplayPropertiesKHR(void *, std::uint32_t *, void *) {
    return initializationFailed;
}

void vkGetPhysicalDeviceProperties(void *, void *) {}

void vkGetPhysicalDeviceProperties2(void *, void *) {}

void vkGetPhysicalDeviceQueueFamilyProperties2(void *, std::uint32_t *, void *) {}

} // extern "C"
