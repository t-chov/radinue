#include "core/PlaybackSettings.h"

#include <QTest>

class PlaybackSettingsTest final : public QObject {
    Q_OBJECT

  private slots:
    void defaultsAreOneHundredPercent();
    void speedChangesInExactTenPercentSteps();
    void speedTraversesEverySupportedStep();
    void speedIsClampedToSupportedRange();
    void speedCanBeReset();
    void volumeIsClampedToSupportedRange_data();
    void volumeIsClampedToSupportedRange();
};

void PlaybackSettingsTest::defaultsAreOneHundredPercent() {
    const radinue::PlaybackSettings settings;

    QCOMPARE(settings.speedPercent(), 100);
    QCOMPARE(settings.volumePercent(), 100);
}

void PlaybackSettingsTest::speedChangesInExactTenPercentSteps() {
    radinue::PlaybackSettings settings;

    settings.decreaseSpeed();
    QCOMPARE(settings.speedPercent(), 90);

    settings.increaseSpeed();
    settings.increaseSpeed();
    QCOMPARE(settings.speedPercent(), 110);
}

void PlaybackSettingsTest::speedTraversesEverySupportedStep() {
    radinue::PlaybackSettings settings;
    while (settings.speedPercent() > radinue::PlaybackSettings::minimumSpeedPercent) {
        settings.decreaseSpeed();
    }

    int expectedSpeed = radinue::PlaybackSettings::minimumSpeedPercent;
    QCOMPARE(settings.speedPercent(), expectedSpeed);
    while (expectedSpeed < radinue::PlaybackSettings::maximumSpeedPercent) {
        settings.increaseSpeed();
        expectedSpeed += radinue::PlaybackSettings::speedStepPercent;
        QCOMPARE(settings.speedPercent(), expectedSpeed);
    }
}

void PlaybackSettingsTest::speedIsClampedToSupportedRange() {
    radinue::PlaybackSettings settings;

    for (int index = 0; index < 20; ++index) {
        settings.decreaseSpeed();
    }
    QCOMPARE(settings.speedPercent(), radinue::PlaybackSettings::minimumSpeedPercent);

    for (int index = 0; index < 30; ++index) {
        settings.increaseSpeed();
    }
    QCOMPARE(settings.speedPercent(), radinue::PlaybackSettings::maximumSpeedPercent);
}

void PlaybackSettingsTest::speedCanBeReset() {
    radinue::PlaybackSettings settings;
    settings.increaseSpeed();
    settings.increaseSpeed();

    settings.resetSpeed();

    QCOMPARE(settings.speedPercent(), 100);
}

void PlaybackSettingsTest::volumeIsClampedToSupportedRange_data() {
    QTest::addColumn<int>("requestedVolume");
    QTest::addColumn<int>("expectedVolume");

    QTest::newRow("below minimum") << -1 << 0;
    QTest::newRow("minimum") << 0 << 0;
    QTest::newRow("normal") << 75 << 75;
    QTest::newRow("maximum") << 200 << 200;
    QTest::newRow("above maximum") << 201 << 200;
}

void PlaybackSettingsTest::volumeIsClampedToSupportedRange() {
    QFETCH(int, requestedVolume);
    QFETCH(int, expectedVolume);

    radinue::PlaybackSettings settings;
    settings.setVolumePercent(requestedVolume);

    QCOMPARE(settings.volumePercent(), expectedVolume);
}

QTEST_APPLESS_MAIN(PlaybackSettingsTest)

#include "PlaybackSettingsTest.moc"
