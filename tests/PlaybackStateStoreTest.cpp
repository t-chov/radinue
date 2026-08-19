#include "core/PlaybackStateStore.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QTest>

namespace {

bool writeFile(const QString &filePath, const QByteArray &contents) {
    QFile file(filePath);
    return file.open(QIODevice::WriteOnly) && file.write(contents) == contents.size();
}

} // namespace

class PlaybackStateStoreTest final : public QObject {
    Q_OBJECT

  private slots:
    void reportsMissingState();
    void savesAndLoadsState();
    void ignoresUnknownFields();
    void rejectsInvalidState_data();
    void rejectsInvalidState();
    void reportsAtomicSaveFailure();
};

void PlaybackStateStoreTest::reportsMissingState() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());

    radinue::PlaybackStateStore store;
    const auto result = store.load(directory.path());
    QCOMPARE(result.status, radinue::PlaybackStateStore::LoadStatus::Missing);
    QVERIFY(!result.state);
}

void PlaybackStateStoreTest::savesAndLoadsState() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const radinue::PlaybackState expected{.fileName = QStringLiteral("recording.mp3"),
                                         .positionMs = 1234567};

    radinue::PlaybackStateStore store;
    QVERIFY2(store.save(directory.path(), expected), qPrintable(store.errorString()));

    const auto result = store.load(directory.path());
    QCOMPARE(result.status, radinue::PlaybackStateStore::LoadStatus::Loaded);
    QVERIFY(result.state);
    QCOMPARE(*result.state, expected);

    QFile stateFile(directory.filePath(radinue::PlaybackStateStore::stateFileName()));
    QVERIFY(stateFile.open(QIODevice::ReadOnly));
    const QJsonObject object = QJsonDocument::fromJson(stateFile.readAll()).object();
    QCOMPARE(object.value("version").toInt(), 1);
    QCOMPARE(object.value("file").toString(), expected.fileName);
    QCOMPARE(object.value("position_ms").toInteger(), expected.positionMs);
}

void PlaybackStateStoreTest::ignoresUnknownFields() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QJsonObject object{{"version", 1},
                             {"file", QStringLiteral("episode.opus")},
                             {"position_ms", 42},
                             {"future_option", true}};
    QVERIFY(writeFile(directory.filePath(radinue::PlaybackStateStore::stateFileName()),
                      QJsonDocument(object).toJson()));

    radinue::PlaybackStateStore store;
    const auto result = store.load(directory.path());
    QCOMPARE(result.status, radinue::PlaybackStateStore::LoadStatus::Loaded);
    QVERIFY(result.state);
    QCOMPARE(result.state->fileName, QStringLiteral("episode.opus"));
    QCOMPARE(result.state->positionMs, 42);
}

void PlaybackStateStoreTest::rejectsInvalidState_data() {
    QTest::addColumn<QByteArray>("contents");
    QTest::newRow("corrupt") << QByteArray("not json");
    QTest::newRow("truncated") << QByteArray(R"({"version":1,"file":)");
    QTest::newRow("array") << QByteArray("[]");
    QTest::newRow("future version")
        << QJsonDocument(QJsonObject{{"version", 2}, {"file", "a.mp3"}, {"position_ms", 0}})
               .toJson();
    QTest::newRow("negative position")
        << QJsonDocument(QJsonObject{{"version", 1}, {"file", "a.mp3"}, {"position_ms", -1}})
               .toJson();
    QTest::newRow("fractional position")
        << QJsonDocument(
               QJsonObject{{"version", 1}, {"file", "a.mp3"}, {"position_ms", 1.5}})
               .toJson();
    QTest::newRow("out-of-range position")
        << QByteArray(R"({"version":1,"file":"a.mp3","position_ms":9223372036854775808})");
    QTest::newRow("absolute file")
        << QJsonDocument(
               QJsonObject{{"version", 1}, {"file", "/a.mp3"}, {"position_ms", 0}})
               .toJson();
    QTest::newRow("nested file")
        << QJsonDocument(
               QJsonObject{{"version", 1}, {"file", "nested/a.mp3"}, {"position_ms", 0}})
               .toJson();
}

void PlaybackStateStoreTest::rejectsInvalidState() {
    QFETCH(QByteArray, contents);
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    QVERIFY(writeFile(directory.filePath(radinue::PlaybackStateStore::stateFileName()), contents));

    radinue::PlaybackStateStore store;
    const auto result = store.load(directory.path());
    QCOMPARE(result.status, radinue::PlaybackStateStore::LoadStatus::Invalid);
    QVERIFY(!result.state);
    QVERIFY(!result.errorString.isEmpty());
}

void PlaybackStateStoreTest::reportsAtomicSaveFailure() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString missingDirectory = directory.filePath(QStringLiteral("missing"));

    radinue::PlaybackStateStore store;
    QVERIFY(!store.save(missingDirectory,
                        {.fileName = QStringLiteral("recording.mp3"), .positionMs = 1}));
    QVERIFY(!store.errorString().isEmpty());
    QVERIFY(!QFile::exists(
        directory.filePath(QStringLiteral("missing/.radinue-state.json"))));
}

QTEST_APPLESS_MAIN(PlaybackStateStoreTest)

#include "PlaybackStateStoreTest.moc"
