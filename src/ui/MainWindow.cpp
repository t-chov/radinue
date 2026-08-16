#include "ui/MainWindow.h"

#include <QLabel>

namespace radinue {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_player(this) {
    setWindowTitle(tr("Radinue"));
    setMinimumSize(480, 180);

    auto *statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setText(m_player.isAvailable() ? tr("Open a directory to begin.")
                                                : tr("Audio playback is unavailable."));
    statusLabel->setAccessibleName(tr("Playback status"));
    setCentralWidget(statusLabel);
}

} // namespace radinue
