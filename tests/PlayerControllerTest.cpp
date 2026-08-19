#include "player/PlayerController.h"

#include <QTest>

class PlayerControllerTest final : public QObject {
    Q_OBJECT

  private slots:
    void clampsSeekPositions_data();
    void clampsSeekPositions();
};

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

QTEST_APPLESS_MAIN(PlayerControllerTest)

#include "PlayerControllerTest.moc"
