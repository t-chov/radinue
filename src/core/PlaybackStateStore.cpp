#include "core/PlaybackStateStore.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

#include <cmath>

namespace radinue {

namespace {

constexpr auto playbackStateFileName = ".radinue-state.json";
constexpr double qint64UpperBoundExclusive = 9223372036854775808.0;

bool isValidFileName(const QString &fileName) {
    return !fileName.isEmpty() && fileName != QStringLiteral(".") &&
           fileName != QStringLiteral("..") && !fileName.contains(QLatin1Char('/')) &&
           !fileName.contains(QLatin1Char('\\')) && !QFileInfo(fileName).isAbsolute();
}

std::optional<qint64> readNonNegativeInteger(const QJsonValue &value) {
    if (!value.isDouble()) {
        return std::nullopt;
    }

    const double number = value.toDouble();
    if (!std::isfinite(number) || number < 0.0 || std::trunc(number) != number ||
        number >= qint64UpperBoundExclusive) {
        return std::nullopt;
    }
    return static_cast<qint64>(number);
}

} // namespace

QString PlaybackStateStore::stateFileName() { return QString::fromLatin1(playbackStateFileName); }

PlaybackStateStore::LoadResult PlaybackStateStore::load(const QString &directoryPath) {
    m_errorString.clear();
    m_lastSavedDirectory.clear();
    m_lastSavedState.reset();

    const QString statePath = QDir(directoryPath).filePath(stateFileName());
    QFile file(statePath);
    if (!file.exists()) {
        return {.status = LoadStatus::Missing};
    }
    if (!file.open(QIODevice::ReadOnly)) {
        m_errorString = file.errorString();
        return {.status = LoadStatus::ReadError, .errorString = m_errorString};
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        m_errorString = parseError.error != QJsonParseError::NoError
                            ? parseError.errorString()
                            : QStringLiteral("The state file does not contain a JSON object.");
        return {.status = LoadStatus::Invalid, .errorString = m_errorString};
    }

    const QJsonObject object = document.object();
    const std::optional<qint64> version = readNonNegativeInteger(object.value("version"));
    const std::optional<qint64> positionMs =
        readNonNegativeInteger(object.value("position_ms"));
    const QJsonValue fileValue = object.value("file");
    if (!version || *version != currentVersion || !fileValue.isString() ||
        !isValidFileName(fileValue.toString()) || !positionMs) {
        m_errorString = QStringLiteral("The state file has invalid or unsupported values.");
        return {.status = LoadStatus::Invalid, .errorString = m_errorString};
    }

    PlaybackState state{.fileName = fileValue.toString(), .positionMs = *positionMs};
    m_lastSavedDirectory = QDir(directoryPath).absolutePath();
    m_lastSavedState = state;
    return {.status = LoadStatus::Loaded, .state = state};
}

bool PlaybackStateStore::save(const QString &directoryPath, const PlaybackState &state) {
    m_errorString.clear();
    if (!isValidFileName(state.fileName) || state.positionMs < 0) {
        m_errorString = QStringLiteral("The playback state contains invalid values.");
        return false;
    }

    const QString absoluteDirectory = QDir(directoryPath).absolutePath();
    if (m_lastSavedDirectory == absoluteDirectory && m_lastSavedState == state) {
        return true;
    }

    const QJsonObject object{
        {QStringLiteral("version"), currentVersion},
        {QStringLiteral("file"), state.fileName},
        {QStringLiteral("position_ms"), state.positionMs},
    };
    QSaveFile file(QDir(absoluteDirectory).filePath(stateFileName()));
    if (!file.open(QIODevice::WriteOnly)) {
        m_errorString = file.errorString();
        return false;
    }

    const QByteArray contents = QJsonDocument(object).toJson(QJsonDocument::Indented);
    if (file.write(contents) != contents.size()) {
        m_errorString = file.errorString();
        file.cancelWriting();
        return false;
    }
    if (!file.commit()) {
        m_errorString = file.errorString();
        return false;
    }

    m_lastSavedDirectory = absoluteDirectory;
    m_lastSavedState = state;
    return true;
}

const QString &PlaybackStateStore::errorString() const noexcept { return m_errorString; }

} // namespace radinue
