#include "core/PlaybackSettings.h"

#include <algorithm>

namespace radinue {

int PlaybackSettings::speedPercent() const noexcept { return m_speedPercent; }

int PlaybackSettings::volumePercent() const noexcept { return m_volumePercent; }

void PlaybackSettings::decreaseSpeed() noexcept {
    m_speedPercent = std::max(minimumSpeedPercent, m_speedPercent - speedStepPercent);
}

void PlaybackSettings::increaseSpeed() noexcept {
    m_speedPercent = std::min(maximumSpeedPercent, m_speedPercent + speedStepPercent);
}

void PlaybackSettings::resetSpeed() noexcept { m_speedPercent = 100; }

void PlaybackSettings::setVolumePercent(int volumePercent) noexcept {
    m_volumePercent = std::clamp(volumePercent, minimumVolumePercent, maximumVolumePercent);
}

} // namespace radinue
