#include "core/DirectoryPlaylist.h"

#include <QDir>
#include <QFileInfo>

#include <algorithm>

namespace radinue {

namespace {

constexpr auto stateFileName = ".radinue-state.json";

bool isStateFile(const QString &fileName) {
    return fileName == QString::fromLatin1(stateFileName) ||
           fileName.startsWith(QString::fromLatin1(stateFileName) + QLatin1Char('.'));
}

bool filenameLessThan(const QString &left, const QString &right) {
    const int caseInsensitiveOrder = QString::compare(left, right, Qt::CaseInsensitive);
    if (caseInsensitiveOrder != 0) {
        return caseInsensitiveOrder < 0;
    }

    // The case-sensitive tie-breaker makes case-only differences deterministic.
    return QString::compare(left, right, Qt::CaseSensitive) < 0;
}

} // namespace

const QStringList &DirectoryPlaylist::supportedExtensions() {
    // Keep this explicit: accepting every format recognized by a particular mpv build would make
    // playlist contents dependent on the machine where Radinue happens to run.
    static const QStringList extensions = {
        QStringLiteral("aac"),  QStringLiteral("flac"), QStringLiteral("m4a"),
        QStringLiteral("mka"),  QStringLiteral("mp3"),  QStringLiteral("ogg"),
        QStringLiteral("opus"), QStringLiteral("wav"),  QStringLiteral("wma"),
    };
    return extensions;
}

void DirectoryPlaylist::sortFileNames(QStringList &fileNames, SortDirection sortDirection) {
    std::sort(fileNames.begin(), fileNames.end(), filenameLessThan);
    if (sortDirection == SortDirection::Descending) {
        std::reverse(fileNames.begin(), fileNames.end());
    }
}

bool DirectoryPlaylist::openDirectory(const QString &directoryPath, SortDirection sortDirection) {
    m_errorString.clear();

    const QFileInfo directoryInfo(directoryPath);
    if (!directoryInfo.exists() || !directoryInfo.isDir()) {
        m_errorString = QStringLiteral("The selected directory does not exist.");
        return false;
    }
    if (!directoryInfo.isReadable()) {
        m_errorString = QStringLiteral("The selected directory cannot be read.");
        return false;
    }

    const QDir directory(directoryInfo.absoluteFilePath());
    const QFileInfoList entries = directory.entryInfoList(
        QDir::Files | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot | QDir::NoSymLinks,
        QDir::NoSort);

    QStringList fileNames;
    for (const QFileInfo &entry : entries) {
        if (!entry.isFile() || entry.isSymLink() || isStateFile(entry.fileName())) {
            continue;
        }

        const QString extension = entry.suffix().toCaseFolded();
        if (supportedExtensions().contains(extension)) {
            fileNames.append(entry.fileName());
        }
    }

    sortFileNames(fileNames, sortDirection);

    m_directoryPath = directory.absolutePath();
    m_fileNames = fileNames;
    return true;
}

const QString &DirectoryPlaylist::directoryPath() const noexcept { return m_directoryPath; }

const QStringList &DirectoryPlaylist::fileNames() const noexcept { return m_fileNames; }

const QString &DirectoryPlaylist::errorString() const noexcept { return m_errorString; }

} // namespace radinue
