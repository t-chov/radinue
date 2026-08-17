#pragma once

#include "core/DirectoryPlaylist.h"
#include "core/PlaybackSettings.h"
#include "player/PlayerController.h"

#include <QMainWindow>

class QComboBox;
class QLabel;
class QListWidget;
class QPushButton;
class QSlider;

namespace radinue {

class MainWindow final : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    bool openDirectory(const QString &directoryPath);

  private:
    void chooseDirectory();
    void reloadDirectory();
    void updatePlaylistView();
    void loadCurrentTrack(bool paused);
    void selectTrack(qsizetype index, bool paused);
    void playPause();
    void previousTrack();
    void nextTrack();
    void handleEndOfFile();
    void updateTransportControls();
    void updatePosition(qint64 positionMs);
    void updateDuration(qint64 durationMs);
    void seekFromSlider();
    void showSliderPreview(int value);
    void decreaseSpeed();
    void increaseSpeed();
    void resetSpeed();
    void applySpeed();
    void updateSpeedDisplay(int speedPercent);
    void applyVolume();
    void updateVolumeDisplay(int volumePercent);
    [[nodiscard]] DirectoryPlaylist::SortDirection selectedSortDirection() const;
    [[nodiscard]] static QString formatTime(qint64 milliseconds);

    DirectoryPlaylist m_playlist;
    PlaybackSettings m_playbackSettings;
    PlayerController m_player;
    QLabel *m_directoryLabel = nullptr;
    QListWidget *m_trackList = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_nowPlayingLabel = nullptr;
    QLabel *m_elapsedLabel = nullptr;
    QLabel *m_durationLabel = nullptr;
    QComboBox *m_sortOrderCombo = nullptr;
    QPushButton *m_previousButton = nullptr;
    QPushButton *m_seekBackwardButton = nullptr;
    QPushButton *m_playPauseButton = nullptr;
    QPushButton *m_seekForwardButton = nullptr;
    QPushButton *m_nextButton = nullptr;
    QPushButton *m_decreaseSpeedButton = nullptr;
    QPushButton *m_increaseSpeedButton = nullptr;
    QLabel *m_speedLabel = nullptr;
    QSlider *m_seekSlider = nullptr;
    QSlider *m_volumeSlider = nullptr;
    QLabel *m_volumeValueLabel = nullptr;
    qint64 m_durationMs = 0;
};

} // namespace radinue
