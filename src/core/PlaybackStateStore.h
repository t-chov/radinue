#pragma once

#include <QString>

#include <optional>

namespace radinue {

struct PlaybackState {
    QString fileName;
    qint64 positionMs = 0;

    friend bool operator==(const PlaybackState &, const PlaybackState &) = default;
};

class PlaybackStateStore final {
  public:
    enum class LoadStatus {
        Loaded,
        Missing,
        Invalid,
        ReadError,
    };

    struct LoadResult {
        LoadStatus status = LoadStatus::Missing;
        std::optional<PlaybackState> state;
        QString errorString;
    };

    static constexpr int currentVersion = 1;

    [[nodiscard]] static QString stateFileName();
    [[nodiscard]] LoadResult load(const QString &directoryPath);
    bool save(const QString &directoryPath, const PlaybackState &state);

    [[nodiscard]] const QString &errorString() const noexcept;

  private:
    QString m_errorString;
    QString m_lastSavedDirectory;
    std::optional<PlaybackState> m_lastSavedState;
};

} // namespace radinue
