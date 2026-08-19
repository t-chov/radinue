#pragma once

#include "core/DirectoryPlaylist.h"
#include "core/PlaybackSettings.h"
#include "core/PlaybackStateStore.h"
#include "player/PlayerController.h"

#include <QMainWindow>

class QComboBox;
class QLabel;
class QListWidget;
class QPushButton;
class QSlider;
class QCloseEvent;
class QTimer;

namespace radinue {

class MainWindow final : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    bool openDirectory(const QString &directoryPath);

  protected:
    void closeEvent(QCloseEvent *event) override;

  private:
    void chooseDirectory();
    void reloadDirectory();
    void updatePlaylistView();
    void loadCurrentTrack(bool paused, qint64 restorePositionMs = -1,
                          bool persistAfterLoad = true);
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
    void persistCurrentState();
    [[nodiscard]] DirectoryPlaylist::SortDirection selectedSortDirection() const;
    [[nodiscard]] static QString formatTime(qint64 milliseconds);

    DirectoryPlaylist m_playlist;
    PlaybackSettings m_playbackSettings;
    PlaybackStateStore m_stateStore;
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
    QTimer *m_checkpointTimer = nullptr;
    qint64 m_durationMs = 0;
    qint64 m_pendingRestorePositionMs = -1;
    bool m_loadingTrack = false;
    bool m_restoringPosition = false;
};

} // namespace radinue
