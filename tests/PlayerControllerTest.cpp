#include "player/PlayerController.h"

#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <clocale>

class PlayerControllerTest final : public QObject {
    Q_OBJECT

  private slots:
    void initTestCase();
    void clampsSeekPositions_data();
    void clampsSeekPositions();
    void reportsPlaybackFailureForUnreadableFile();
};

void PlayerControllerTest::initTestCase() {
    QVERIFY(std::setlocale(LC_NUMERIC, "C") != nullptr);
}

void PlayerControllerTest::clampsSeekPositions_data() {
    QTest::addColumn<qint64>("requestedPositionMs");
    QTest::addColumn<qint64>("durationMs");
    QTest::addColumn<qint64>("expectedPositionMs");

    QTest::newRow("before zero") << qint64{-10000} << qint64{60000} << qint64{0};
    QTest::newRow("inside known duration") << qint64{30000} << qint64{60000} << qint64{30000};
    QTest::newRow("at known duration") << qint64{60000} << qint64{60000} << qint64{60000};
    QTest::newRow("past known duration") << qint64{70000} << qint64{60000} << qint64{60000};
    QTest::newRow("unknown duration") << qint64{70000} << qint64{0} << qint64{70000};
}

void PlayerControllerTest::clampsSeekPositions() {
    QFETCH(qint64, requestedPositionMs);
    QFETCH(qint64, durationMs);
    QFETCH(qint64, expectedPositionMs);

    QCOMPARE(radinue::PlayerController::clampedSeekPosition(requestedPositionMs, durationMs),
             expectedPositionMs);
}

void PlayerControllerTest::reportsPlaybackFailureForUnreadableFile() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString filePath = directory.filePath(QStringLiteral("missing.mp3"));

    radinue::PlayerController player;
    QVERIFY(player.isAvailable());
    QSignalSpy failureSpy(&player, &radinue::PlayerController::playbackFailed);
    QVERIFY(player.loadFile(filePath, false));

    QTRY_COMPARE_WITH_TIMEOUT(failureSpy.count(), 1, 5000);
    QCOMPARE(failureSpy.first().at(0).toString(), filePath);
    QVERIFY(!failureSpy.first().at(1).toString().isEmpty());
    QVERIFY(!player.hasFile());
}

QTEST_GUILESS_MAIN(PlayerControllerTest)

#include "PlayerControllerTest.moc"
