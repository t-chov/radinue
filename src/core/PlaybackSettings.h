#pragma once

namespace radinue {

class PlaybackSettings final {
  public:
    static constexpr int minimumSpeedPercent = 50;
    static constexpr int maximumSpeedPercent = 200;
    static constexpr int speedStepPercent = 10;
    static constexpr int minimumVolumePercent = 0;
    static constexpr int maximumVolumePercent = 200;

    [[nodiscard]] int speedPercent() const noexcept;
    [[nodiscard]] int volumePercent() const noexcept;

    void decreaseSpeed() noexcept;
    void increaseSpeed() noexcept;
    void resetSpeed() noexcept;
    void setVolumePercent(int volumePercent) noexcept;

  private:
    int m_speedPercent = 100;
    int m_volumePercent = 100;
};

} // namespace radinue
