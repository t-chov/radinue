#pragma once

#include "core/DirectoryPlaylist.h"
#include "player/PlayerController.h"

#include <QMainWindow>

class QComboBox;
class QLabel;
class QListWidget;

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
    [[nodiscard]] DirectoryPlaylist::SortDirection selectedSortDirection() const;

    DirectoryPlaylist m_playlist;
    PlayerController m_player;
    QLabel *m_directoryLabel = nullptr;
    QListWidget *m_trackList = nullptr;
    QLabel *m_statusLabel = nullptr;
    QComboBox *m_sortOrderCombo = nullptr;
};

} // namespace radinue
