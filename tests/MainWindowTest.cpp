#include "core/PlaybackStateStore.h"
#include "ui/MainWindow.h"

#include <QFile>
#include <QLabel>
#include <QListWidget>
#include <QShortcut>
#include <QTemporaryDir>
#include <QTest>

#include <clocale>

namespace {

bool createFile(const QString &filePath) {
    QFile file(filePath);
    return file.open(QIODevice::WriteOnly) && file.write("not audio") == 9;
}

} // namespace

class MainWindowTest final : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase();
    void restoresFirstTrackWhenSavedTrackIsMissing();
    void advancesAtEndOfTrackWithoutWrapping();
    void mapsWindowShortcuts();
};

void MainWindowTest::initTestCase() {
    QVERIFY(std::setlocale(LC_NUMERIC, "C") != nullptr);
}

void MainWindowTest::restoresFirstTrackWhenSavedTrackIsMissing() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    QVERIFY(createFile(directory.filePath(QStringLiteral("a.mp3"))));
    QVERIFY(createFile(directory.filePath(QStringLiteral("b.mp3"))));

    radinue::PlaybackStateStore stateStore;
    QVERIFY(stateStore.save(directory.path(),
                            {.fileName = QStringLiteral("removed.mp3"), .positionMs = 12000}));

    radinue::MainWindow window;
    QVERIFY(window.openDirectory(directory.path()));

    const auto *trackList = window.findChild<QListWidget *>(QStringLiteral("trackList"));
    const auto *nowPlaying = window.findChild<QLabel *>(QStringLiteral("nowPlayingLabel"));
    QVERIFY(trackList != nullptr);
    QVERIFY(nowPlaying != nullptr);
    QCOMPARE(trackList->currentRow(), 0);
    QCOMPARE(nowPlaying->text(), QStringLiteral("a.mp3"));
}

void MainWindowTest::advancesAtEndOfTrackWithoutWrapping() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    QVERIFY(createFile(directory.filePath(QStringLiteral("a.mp3"))));
    QVERIFY(createFile(directory.filePath(QStringLiteral("b.mp3"))));

    radinue::MainWindow window;
    QVERIFY(window.openDirectory(directory.path()));
    const auto *nowPlaying = window.findChild<QLabel *>(QStringLiteral("nowPlayingLabel"));
    QVERIFY(nowPlaying != nullptr);
    QCOMPARE(nowPlaying->text(), QStringLiteral("a.mp3"));

    QVERIFY(QMetaObject::invokeMethod(&window, "handleEndOfFile", Qt::DirectConnection));
    QCOMPARE(nowPlaying->text(), QStringLiteral("b.mp3"));

    QVERIFY(QMetaObject::invokeMethod(&window, "handleEndOfFile", Qt::DirectConnection));
    QCOMPARE(nowPlaying->text(), QStringLiteral("b.mp3"));
}

void MainWindowTest::mapsWindowShortcuts() {
    radinue::MainWindow window;
    const QList<QPair<QString, QKeySequence>> expected{
        {QStringLiteral("seekBackwardShortcut"), QKeySequence(Qt::Key_Z)},
        {QStringLiteral("seekForwardShortcut"), QKeySequence(Qt::Key_X)},
        {QStringLiteral("decreaseSpeedShortcut"), QKeySequence(Qt::Key_S)},
        {QStringLiteral("increaseSpeedShortcut"), QKeySequence(Qt::Key_D)},
        {QStringLiteral("resetSpeedShortcut"), QKeySequence(Qt::Key_G)},
    };

    for (const auto &[objectName, keySequence] : expected) {
        const auto *shortcut = window.findChild<QShortcut *>(objectName);
        QVERIFY2(shortcut != nullptr, qPrintable(objectName));
        QCOMPARE(shortcut->key(), keySequence);
        QCOMPARE(shortcut->context(), Qt::WindowShortcut);
    }
}

QTEST_MAIN(MainWindowTest)

#include "MainWindowTest.moc"
