#include "core/DirectoryPlaylist.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTest>

#include <algorithm>

namespace {

bool createFile(const QString &filePath) {
    QFile file(filePath);
    return file.open(QIODevice::WriteOnly) && file.write("audio") == 5;
}

} // namespace

class DirectoryPlaylistTest final : public QObject {
    Q_OBJECT

  private slots:
    void acceptsAllowlistedExtensionsCaseInsensitively();
    void ignoresUnsupportedAndSpecialEntries();
    void ignoresSubdirectories();
    void ignoresSymlinks();
    void sortsDeterministicallyAndReversibly();
    void handlesEmptyAndSingleFileDirectories();
    void rejectsMissingDirectory();
};

void DirectoryPlaylistTest::acceptsAllowlistedExtensionsCaseInsensitively() {
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());

    QStringList expectedFileNames;
    for (const QString &extension : radinue::DirectoryPlaylist::supportedExtensions()) {
        const QString fileName = QStringLiteral("recording.%1").arg(extension.toUpper());
        QVERIFY(createFile(temporaryDirectory.filePath(fileName)));
        expectedFileNames.append(fileName);
    }

    radinue::DirectoryPlaylist playlist;
    QVERIFY(playlist.openDirectory(temporaryDirectory.path()));

    QCOMPARE(playlist.fileNames(), expectedFileNames);
}

void DirectoryPlaylistTest::ignoresUnsupportedAndSpecialEntries() {
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());
    QVERIFY(createFile(temporaryDirectory.filePath(QStringLiteral("included.m4a"))));
    QVERIFY(createFile(temporaryDirectory.filePath(QStringLiteral("notes.txt"))));
    QVERIFY(createFile(temporaryDirectory.filePath(QStringLiteral(".radinue-state.json"))));
    QVERIFY(createFile(
        temporaryDirectory.filePath(QStringLiteral(".radinue-state.json.temporary.mp3"))));

    radinue::DirectoryPlaylist playlist;
    QVERIFY(playlist.openDirectory(temporaryDirectory.path()));

    QCOMPARE(playlist.fileNames(), QStringList({QStringLiteral("included.m4a")}));
}

void DirectoryPlaylistTest::ignoresSubdirectories() {
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());
    QVERIFY(QDir(temporaryDirectory.path()).mkdir(QStringLiteral("archive")));
    QVERIFY(createFile(temporaryDirectory.filePath(QStringLiteral("archive/nested.mp3"))));
    QVERIFY(createFile(temporaryDirectory.filePath(QStringLiteral("direct.mp3"))));

    radinue::DirectoryPlaylist playlist;
    QVERIFY(playlist.openDirectory(temporaryDirectory.path()));

    QCOMPARE(playlist.fileNames(), QStringList({QStringLiteral("direct.mp3")}));
}

void DirectoryPlaylistTest::ignoresSymlinks() {
#ifndef Q_OS_UNIX
    QSKIP("Creating a real symbolic link requires extra privileges on this platform.");
#else
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());
    const QString targetPath = temporaryDirectory.filePath(QStringLiteral("target.mp3"));
    QVERIFY(createFile(targetPath));
    QVERIFY(QFile::link(targetPath,
                        temporaryDirectory.filePath(QStringLiteral("linked-recording.mp3"))));

    radinue::DirectoryPlaylist playlist;
    QVERIFY(playlist.openDirectory(temporaryDirectory.path()));

    QCOMPARE(playlist.fileNames(), QStringList({QStringLiteral("target.mp3")}));
#endif
}

void DirectoryPlaylistTest::sortsDeterministicallyAndReversibly() {
    QStringList ascending = {
        QStringLiteral("écho.mp3"), QStringLiteral("alpha.mp3"), QStringLiteral("Äther.mp3"),
        QStringLiteral("Zeta.mp3"), QStringLiteral("Alpha.mp3"),
    };
    radinue::DirectoryPlaylist::sortFileNames(ascending,
                                              radinue::DirectoryPlaylist::SortDirection::Ascending);
    QCOMPARE(ascending, QStringList({QStringLiteral("Alpha.mp3"), QStringLiteral("alpha.mp3"),
                                     QStringLiteral("Zeta.mp3"), QStringLiteral("Äther.mp3"),
                                     QStringLiteral("écho.mp3")}));

    QStringList descending = ascending;
    radinue::DirectoryPlaylist::sortFileNames(
        descending, radinue::DirectoryPlaylist::SortDirection::Descending);
    QStringList reversedAscending = ascending;
    std::reverse(reversedAscending.begin(), reversedAscending.end());
    QCOMPARE(descending, reversedAscending);
}

void DirectoryPlaylistTest::handlesEmptyAndSingleFileDirectories() {
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());

    radinue::DirectoryPlaylist playlist;
    QVERIFY(playlist.openDirectory(temporaryDirectory.path()));
    QVERIFY(playlist.fileNames().isEmpty());

    QVERIFY(createFile(temporaryDirectory.filePath(QStringLiteral("only.opus"))));
    QVERIFY(playlist.openDirectory(temporaryDirectory.path()));
    QCOMPARE(playlist.fileNames(), QStringList({QStringLiteral("only.opus")}));
}

void DirectoryPlaylistTest::rejectsMissingDirectory() {
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());
    const QString missingPath = temporaryDirectory.filePath(QStringLiteral("missing"));

    radinue::DirectoryPlaylist playlist;
    QVERIFY(!playlist.openDirectory(missingPath));
    QVERIFY(!playlist.errorString().isEmpty());
    QVERIFY(playlist.fileNames().isEmpty());
}

QTEST_APPLESS_MAIN(DirectoryPlaylistTest)

#include "DirectoryPlaylistTest.moc"
