#include "ui/MainWindow.h"

#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QStandardPaths>
#include <QStyle>
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

    m_nowPlayingLabel = new QLabel(tr("Nothing loaded"), centralWidget);
    m_nowPlayingLabel->setObjectName(QStringLiteral("nowPlayingLabel"));
    m_nowPlayingLabel->setAccessibleName(tr("Current track"));
    m_nowPlayingLabel->setMinimumWidth(0);
    m_nowPlayingLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    mainLayout->addWidget(m_nowPlayingLabel);

    auto *seekLayout = new QHBoxLayout;
    m_elapsedLabel = new QLabel(QStringLiteral("0:00"), centralWidget);
    m_elapsedLabel->setAccessibleName(tr("Elapsed time"));
    seekLayout->addWidget(m_elapsedLabel);

    m_seekSlider = new QSlider(Qt::Horizontal, centralWidget);
    m_seekSlider->setObjectName(QStringLiteral("seekSlider"));
    m_seekSlider->setRange(0, 1000);
    m_seekSlider->setAccessibleName(tr("Playback position"));
    m_seekSlider->setToolTip(tr("Seek within the current track"));
    seekLayout->addWidget(m_seekSlider, 1);

    m_durationLabel = new QLabel(QStringLiteral("0:00"), centralWidget);
    m_durationLabel->setAccessibleName(tr("Track duration"));
    seekLayout->addWidget(m_durationLabel);
    mainLayout->addLayout(seekLayout);

    auto *transportLayout = new QHBoxLayout;
    transportLayout->addStretch();
    m_previousButton = new QPushButton(centralWidget);
    m_seekBackwardButton = new QPushButton(centralWidget);
    m_playPauseButton = new QPushButton(centralWidget);
    m_seekForwardButton = new QPushButton(centralWidget);
    m_nextButton = new QPushButton(centralWidget);

    m_previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    m_seekBackwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    m_playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_seekForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    m_nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    m_playPauseButton->setMinimumSize(48, 36);

    const auto configureButton = [](QPushButton *button, const QString &name,
                                    const QString &toolTip) {
        button->setAccessibleName(name);
        button->setToolTip(toolTip);
        button->setAutoDefault(false);
    };
    configureButton(m_previousButton, tr("Previous track"), tr("Previous track"));
    configureButton(m_seekBackwardButton, tr("Seek backward 10 seconds"),
                    tr("Seek backward 10 seconds (Z)"));
    configureButton(m_playPauseButton, tr("Play"), tr("Play/Pause (Space)"));
    configureButton(m_seekForwardButton, tr("Seek forward 10 seconds"),
                    tr("Seek forward 10 seconds (X)"));
    configureButton(m_nextButton, tr("Next track"), tr("Next track"));

    transportLayout->addWidget(m_previousButton);
    transportLayout->addWidget(m_seekBackwardButton);
    transportLayout->addWidget(m_playPauseButton);
    transportLayout->addWidget(m_seekForwardButton);
    transportLayout->addWidget(m_nextButton);
    transportLayout->addStretch();
    mainLayout->addLayout(transportLayout);

    m_statusLabel = new QLabel(centralWidget);
    m_statusLabel->setAccessibleName(tr("Playlist status"));
    m_statusLabel->setText(m_player.isAvailable() ? tr("Choose a directory to begin.")
                                                  : tr("Audio playback is unavailable."));
    mainLayout->addWidget(m_statusLabel);

    setCentralWidget(centralWidget);

    auto *seekBackwardShortcut = new QShortcut(QKeySequence(Qt::Key_Z), this);
    auto *seekForwardShortcut = new QShortcut(QKeySequence(Qt::Key_X), this);
    auto *playPauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);

    connect(chooseDirectoryButton, &QPushButton::clicked, this, &MainWindow::chooseDirectory);
    connect(m_sortOrderCombo, &QComboBox::currentIndexChanged, this, &MainWindow::reloadDirectory);
    connect(m_trackList, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row >= 0 && row != m_playlist.currentIndex()) {
            selectTrack(row, m_player.isPaused());
        }
    });
    connect(m_trackList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        const qsizetype index = m_trackList->row(item);
        if (index != m_playlist.currentIndex()) {
            selectTrack(index, false);
        } else {
            m_player.setPaused(false);
        }
    });
    connect(m_previousButton, &QPushButton::clicked, this, &MainWindow::previousTrack);
    connect(m_seekBackwardButton, &QPushButton::clicked, this,
            [this] { m_player.seekRelative(-10000); });
    connect(m_playPauseButton, &QPushButton::clicked, this, &MainWindow::playPause);
    connect(m_seekForwardButton, &QPushButton::clicked, this,
            [this] { m_player.seekRelative(10000); });
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::nextTrack);
    connect(m_seekSlider, &QSlider::sliderReleased, this, &MainWindow::seekFromSlider);
    connect(m_seekSlider, &QSlider::sliderMoved, this, &MainWindow::showSliderPreview);
    connect(seekBackwardShortcut, &QShortcut::activated, m_seekBackwardButton,
            &QPushButton::click);
    connect(seekForwardShortcut, &QShortcut::activated, m_seekForwardButton, &QPushButton::click);
    connect(playPauseShortcut, &QShortcut::activated, m_playPauseButton, &QPushButton::click);

    connect(&m_player, &PlayerController::pauseChanged, this, [this](bool paused) {
        m_playPauseButton->setIcon(
            style()->standardIcon(paused ? QStyle::SP_MediaPlay : QStyle::SP_MediaPause));
        m_playPauseButton->setAccessibleName(paused ? tr("Play") : tr("Pause"));
    });
    connect(&m_player, &PlayerController::positionChanged, this, &MainWindow::updatePosition);
    connect(&m_player, &PlayerController::durationChanged, this, &MainWindow::updateDuration);
    connect(&m_player, &PlayerController::endOfFile, this, &MainWindow::handleEndOfFile);
    connect(&m_player, &PlayerController::errorOccurred, this,
            [this](const QString &message) { m_statusLabel->setText(message); });

    updateTransportControls();
}

bool MainWindow::openDirectory(const QString &directoryPath) {
    if (!m_playlist.openDirectory(directoryPath, selectedSortDirection())) {
        QMessageBox::warning(this, tr("Cannot Open Directory"), m_playlist.errorString());
        return false;
    }

    updatePlaylistView();
    if (m_playlist.currentIndex() >= 0) {
        loadCurrentTrack(true);
    }
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
    if (m_playlist.currentIndex() >= 0) {
        m_trackList->setCurrentRow(static_cast<int>(m_playlist.currentIndex()));
    }

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
    updateTransportControls();
}

void MainWindow::loadCurrentTrack(bool paused) {
    const QString filePath = m_playlist.currentFilePath();
    if (filePath.isEmpty() || !m_player.loadFile(filePath, paused)) {
        updateTransportControls();
        return;
    }

    const QString fileName = m_playlist.currentFileName();
    m_nowPlayingLabel->setText(fileName);
    m_nowPlayingLabel->setToolTip(fileName);
    m_trackList->setCurrentRow(static_cast<int>(m_playlist.currentIndex()));
    updateTransportControls();
}

void MainWindow::selectTrack(qsizetype index, bool paused) {
    if (m_playlist.setCurrentIndex(index)) {
        loadCurrentTrack(paused);
    }
}

void MainWindow::playPause() {
    if (!m_player.hasFile()) {
        loadCurrentTrack(false);
    } else {
        m_player.togglePause();
    }
}

void MainWindow::previousTrack() {
    if (m_playlist.movePrevious()) {
        loadCurrentTrack(m_player.isPaused());
    }
}

void MainWindow::nextTrack() {
    if (m_playlist.moveNext()) {
        loadCurrentTrack(m_player.isPaused());
    }
}

void MainWindow::handleEndOfFile() {
    if (m_playlist.moveNext()) {
        loadCurrentTrack(false);
    } else {
        m_player.setPaused(true);
        updateTransportControls();
    }
}

void MainWindow::updateTransportControls() {
    const bool playbackAvailable = m_player.isAvailable();
    const bool hasTrack = m_playlist.currentIndex() >= 0;
    const bool canSeek = playbackAvailable && m_player.hasFile();
    m_previousButton->setEnabled(playbackAvailable && m_playlist.hasPrevious());
    m_seekBackwardButton->setEnabled(canSeek);
    m_playPauseButton->setEnabled(playbackAvailable && hasTrack);
    m_seekForwardButton->setEnabled(canSeek);
    m_nextButton->setEnabled(playbackAvailable && m_playlist.hasNext());
    m_seekSlider->setEnabled(canSeek && m_durationMs > 0);
}

void MainWindow::updatePosition(qint64 positionMs) {
    if (!m_seekSlider->isSliderDown()) {
        const int sliderPosition =
            m_durationMs > 0 ? static_cast<int>((positionMs * 1000) / m_durationMs) : 0;
        m_seekSlider->setValue(qBound(0, sliderPosition, 1000));
        m_elapsedLabel->setText(formatTime(positionMs));
    }
}

void MainWindow::updateDuration(qint64 durationMs) {
    m_durationMs = qMax<qint64>(0, durationMs);
    m_durationLabel->setText(formatTime(m_durationMs));
    updateTransportControls();
}

void MainWindow::seekFromSlider() {
    if (m_durationMs <= 0) {
        return;
    }
    const qint64 positionMs = (m_durationMs * m_seekSlider->value()) / 1000;
    m_player.seekAbsolute(positionMs);
}

void MainWindow::showSliderPreview(int value) {
    if (m_durationMs > 0) {
        m_elapsedLabel->setText(formatTime((m_durationMs * value) / 1000));
    }
}

QString MainWindow::formatTime(qint64 milliseconds) {
    const qint64 totalSeconds = qMax<qint64>(0, milliseconds) / 1000;
    const qint64 hours = totalSeconds / 3600;
    const qint64 minutes = (totalSeconds / 60) % 60;
    const qint64 seconds = totalSeconds % 60;
    if (hours > 0) {
        return QStringLiteral("%1:%2:%3")
            .arg(hours)
            .arg(minutes, 2, 10, QLatin1Char('0'))
            .arg(seconds, 2, 10, QLatin1Char('0'));
    }
    return QStringLiteral("%1:%2").arg(minutes).arg(seconds, 2, 10, QLatin1Char('0'));
}

DirectoryPlaylist::SortDirection MainWindow::selectedSortDirection() const {
    return m_sortOrderCombo->currentIndex() == 0 ? DirectoryPlaylist::SortDirection::Ascending
                                                 : DirectoryPlaylist::SortDirection::Descending;
}

} // namespace radinue
