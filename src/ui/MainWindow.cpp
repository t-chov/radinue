#include "ui/MainWindow.h"

#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QWidget>

namespace radinue {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_player(this) {
    setWindowTitle(tr("Radinue"));
    setMinimumSize(600, 400);

    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    auto *directoryLayout = new QHBoxLayout;

    auto *chooseDirectoryButton = new QPushButton(tr("Open Directory…"), centralWidget);
    chooseDirectoryButton->setAccessibleName(tr("Open playlist directory"));
    chooseDirectoryButton->setToolTip(tr("Choose a directory containing audio files"));
    directoryLayout->addWidget(chooseDirectoryButton);

    m_directoryLabel = new QLabel(tr("No directory selected"), centralWidget);
    m_directoryLabel->setAccessibleName(tr("Playlist directory"));
    m_directoryLabel->setMinimumWidth(0);
    m_directoryLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    directoryLayout->addWidget(m_directoryLabel, 1);

    auto *sortLabel = new QLabel(tr("Order:"), centralWidget);
    directoryLayout->addWidget(sortLabel);

    m_sortOrderCombo = new QComboBox(centralWidget);
    m_sortOrderCombo->addItem(tr("A to Z"));
    m_sortOrderCombo->addItem(tr("Z to A"));
    m_sortOrderCombo->setAccessibleName(tr("Filename order"));
    m_sortOrderCombo->setToolTip(tr("Sort the playlist by filename"));
    directoryLayout->addWidget(m_sortOrderCombo);

    mainLayout->addLayout(directoryLayout);

    m_trackList = new QListWidget(centralWidget);
    m_trackList->setObjectName(QStringLiteral("trackList"));
    m_trackList->setAccessibleName(tr("Audio files"));
    m_trackList->setTextElideMode(Qt::ElideMiddle);
    mainLayout->addWidget(m_trackList, 1);

    m_statusLabel = new QLabel(centralWidget);
    m_statusLabel->setAccessibleName(tr("Playlist status"));
    m_statusLabel->setText(m_player.isAvailable() ? tr("Choose a directory to begin.")
                                                  : tr("Audio playback is unavailable."));
    mainLayout->addWidget(m_statusLabel);

    setCentralWidget(centralWidget);

    connect(chooseDirectoryButton, &QPushButton::clicked, this, &MainWindow::chooseDirectory);
    connect(m_sortOrderCombo, &QComboBox::currentIndexChanged, this, &MainWindow::reloadDirectory);
}

bool MainWindow::openDirectory(const QString &directoryPath) {
    if (!m_playlist.openDirectory(directoryPath, selectedSortDirection())) {
        QMessageBox::warning(this, tr("Cannot Open Directory"), m_playlist.errorString());
        return false;
    }

    updatePlaylistView();
    return true;
}

void MainWindow::chooseDirectory() {
    QString initialDirectory = m_playlist.directoryPath();
    if (initialDirectory.isEmpty()) {
        initialDirectory = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    }

    const QString directoryPath = QFileDialog::getExistingDirectory(
        this, tr("Choose Playlist Directory"), initialDirectory, QFileDialog::ShowDirsOnly);
    if (!directoryPath.isEmpty()) {
        openDirectory(directoryPath);
    }
}

void MainWindow::reloadDirectory() {
    if (!m_playlist.directoryPath().isEmpty()) {
        const QString directoryPath = m_playlist.directoryPath();
        openDirectory(directoryPath);
    }
}

void MainWindow::updatePlaylistView() {
    m_trackList->clear();
    m_trackList->addItems(m_playlist.fileNames());

    const QDir directory(m_playlist.directoryPath());
    m_directoryLabel->setText(directory.dirName());
    m_directoryLabel->setToolTip(m_playlist.directoryPath());

    const qsizetype fileCount = m_playlist.fileNames().size();
    if (fileCount == 0) {
        m_statusLabel->setText(tr("No supported audio files found."));
    } else if (fileCount == 1) {
        m_statusLabel->setText(tr("1 audio file"));
    } else {
        m_statusLabel->setText(tr("%1 audio files").arg(fileCount));
    }
}

DirectoryPlaylist::SortDirection MainWindow::selectedSortDirection() const {
    return m_sortOrderCombo->currentIndex() == 0 ? DirectoryPlaylist::SortDirection::Ascending
                                                 : DirectoryPlaylist::SortDirection::Descending;
}

} // namespace radinue
