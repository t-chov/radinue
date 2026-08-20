#include "player/PlayerController.h"

#include <QDebug>
#include <QMetaObject>
#include <QString>

#include <mpv/client.h>

namespace radinue {

namespace {

constexpr uint64_t positionProperty = 1;
constexpr uint64_t durationProperty = 2;
constexpr uint64_t pauseProperty = 3;
constexpr uint64_t speedProperty = 4;
constexpr uint64_t volumeProperty = 5;

qint64 secondsToMilliseconds(double seconds) {
    return qMax<qint64>(0, qRound64(seconds * 1000.0));
}

} // namespace

PlayerController::PlayerController(QObject *parent) : QObject(parent), m_mpv(mpv_create()) {
    if (m_mpv == nullptr) {
        qWarning() << "Could not create the libmpv context";
        return;
    }

    const int videoResult = mpv_set_option_string(m_mpv, "video", "no");
    if (videoResult < 0) {
        qWarning() << "Could not disable video output:" << mpv_error_string(videoResult);
    }

    const int pitchCorrectionResult =
        mpv_set_option_string(m_mpv, "audio-pitch-correction", "yes");
    if (pitchCorrectionResult < 0) {
        qWarning() << "Could not enable pitch correction:"
                   << mpv_error_string(pitchCorrectionResult);
    }

    const int volumeMaxResult = mpv_set_option_string(m_mpv, "volume-max", "200");
    if (volumeMaxResult < 0) {
        qWarning() << "Could not configure maximum volume:" << mpv_error_string(volumeMaxResult);
    }

    const int result = mpv_initialize(m_mpv);
    if (result < 0) {
        qWarning() << "Could not initialize libmpv:" << mpv_error_string(result);
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
        return;
    }

    const auto observe = [this](uint64_t identifier, const char *name, mpv_format format) {
        const int observeResult = mpv_observe_property(m_mpv, identifier, name, format);
        if (observeResult < 0) {
            qWarning() << "Could not observe mpv property" << name << ':'
                       << mpv_error_string(observeResult);
        }
    };
    observe(positionProperty, "time-pos", MPV_FORMAT_DOUBLE);
    observe(durationProperty, "duration", MPV_FORMAT_DOUBLE);
    observe(pauseProperty, "pause", MPV_FORMAT_FLAG);
    observe(speedProperty, "speed", MPV_FORMAT_DOUBLE);
    observe(volumeProperty, "volume", MPV_FORMAT_DOUBLE);
    mpv_set_wakeup_callback(m_mpv, &PlayerController::wakeup, this);
}

PlayerController::~PlayerController() {
    if (m_mpv != nullptr) {
        mpv_set_wakeup_callback(m_mpv, nullptr, nullptr);
        mpv_terminate_destroy(m_mpv);
    }
}

bool PlayerController::isAvailable() const noexcept { return m_mpv != nullptr; }

bool PlayerController::hasFile() const noexcept { return !m_filePath.isEmpty(); }

bool PlayerController::isPaused() const noexcept { return m_paused; }

qint64 PlayerController::positionMs() const noexcept { return m_positionMs; }

qint64 PlayerController::durationMs() const noexcept { return m_durationMs; }

int PlayerController::speedPercent() const noexcept { return m_speedPercent; }

int PlayerController::volumePercent() const noexcept { return m_volumePercent; }

qint64 PlayerController::clampedSeekPosition(qint64 requestedPositionMs,
                                             qint64 durationMs) noexcept {
    const qint64 upperBound = durationMs > 0 ? durationMs : qMax<qint64>(0, requestedPositionMs);
    return qBound<qint64>(0, requestedPositionMs, upperBound);
}

bool PlayerController::loadFile(const QString &filePath, bool paused) {
    if (m_mpv == nullptr || filePath.isEmpty()) {
        return false;
    }

    setPaused(paused);
    const QByteArray encodedPath = filePath.toUtf8();
    const char *arguments[] = {"loadfile", encodedPath.constData(), "replace", nullptr};
    if (!sendCommand(arguments)) {
        return false;
    }

    m_filePath = filePath;
    m_positionMs = 0;
    m_durationMs = 0;
    emit positionChanged(0);
    emit durationChanged(0);
    return true;
}

void PlayerController::togglePause() { setPaused(!m_paused); }

void PlayerController::setPaused(bool paused) {
    if (m_mpv == nullptr) {
        return;
    }
    int value = paused ? 1 : 0;
    const int result = mpv_set_property(m_mpv, "pause", MPV_FORMAT_FLAG, &value);
    if (result < 0) {
        reportMpvError(tr("change pause state"), result);
        return;
    }
    m_paused = paused;
    emit pauseChanged(m_paused);
}

void PlayerController::seekRelative(qint64 offsetMs) {
    if (!hasFile()) {
        return;
    }
    seekAbsolute(m_positionMs + offsetMs);
}

void PlayerController::seekAbsolute(qint64 positionMs) {
    if (m_mpv == nullptr || !hasFile()) {
        return;
    }

    const qint64 clampedPosition = clampedSeekPosition(positionMs, m_durationMs);
    const QByteArray seconds = QByteArray::number(static_cast<double>(clampedPosition) / 1000.0,
                                                  'f', 3);
    const char *arguments[] = {"seek", seconds.constData(), "absolute+exact", nullptr};
    if (sendCommand(arguments)) {
        m_positionMs = clampedPosition;
        emit positionChanged(m_positionMs);
        emit seekCompleted(m_positionMs);
    }
}

void PlayerController::setSpeedPercent(int speedPercent) {
    if (m_mpv == nullptr || speedPercent <= 0) {
        return;
    }

    double speed = static_cast<double>(speedPercent) / 100.0;
    const int result = mpv_set_property(m_mpv, "speed", MPV_FORMAT_DOUBLE, &speed);
    if (result < 0) {
        reportMpvError(tr("change playback speed"), result);
        return;
    }

    m_speedPercent = speedPercent;
    emit speedChanged(m_speedPercent);
}

void PlayerController::setVolumePercent(int volumePercent) {
    if (m_mpv == nullptr || volumePercent < 0) {
        return;
    }

    double volume = static_cast<double>(volumePercent);
    const int result = mpv_set_property(m_mpv, "volume", MPV_FORMAT_DOUBLE, &volume);
    if (result < 0) {
        reportMpvError(tr("change volume"), result);
        return;
    }

    m_volumePercent = volumePercent;
    emit volumeChanged(m_volumePercent);
}

void PlayerController::wakeup(void *context) {
    auto *controller = static_cast<PlayerController *>(context);
    QMetaObject::invokeMethod(controller, &PlayerController::processEvents, Qt::QueuedConnection);
}

void PlayerController::processEvents() {
    if (m_mpv == nullptr) {
        return;
    }

    while (true) {
        mpv_event *event = mpv_wait_event(m_mpv, 0.0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }

        if (event->event_id == MPV_EVENT_FILE_LOADED) {
            emit fileLoaded(m_filePath);
            continue;
        }
        if (event->event_id == MPV_EVENT_END_FILE) {
            const auto *endFile = static_cast<mpv_event_end_file *>(event->data);
            if (endFile->reason == MPV_END_FILE_REASON_EOF) {
                emit endOfFile();
            } else if (endFile->reason == MPV_END_FILE_REASON_ERROR) {
                const QString failedFilePath = m_filePath;
                const QString message =
                    tr("Could not play the audio file: %1")
                        .arg(QString::fromUtf8(mpv_error_string(endFile->error)));
                m_filePath.clear();
                m_positionMs = 0;
                m_durationMs = 0;
                qWarning().noquote() << message;
                emit positionChanged(0);
                emit durationChanged(0);
                emit errorOccurred(message);
                emit playbackFailed(failedFilePath, message);
            }
            continue;
        }
        if (event->event_id != MPV_EVENT_PROPERTY_CHANGE) {
            continue;
        }

        const auto *property = static_cast<mpv_event_property *>(event->data);
        if (property->data == nullptr) {
            continue;
        }
        if (event->reply_userdata == positionProperty && property->format == MPV_FORMAT_DOUBLE) {
            m_positionMs = secondsToMilliseconds(*static_cast<double *>(property->data));
            emit positionChanged(m_positionMs);
        } else if (event->reply_userdata == durationProperty &&
                   property->format == MPV_FORMAT_DOUBLE) {
            m_durationMs = secondsToMilliseconds(*static_cast<double *>(property->data));
            emit durationChanged(m_durationMs);
        } else if (event->reply_userdata == pauseProperty && property->format == MPV_FORMAT_FLAG) {
            m_paused = *static_cast<int *>(property->data) != 0;
            emit pauseChanged(m_paused);
        } else if (event->reply_userdata == speedProperty && property->format == MPV_FORMAT_DOUBLE) {
            const double speed = *static_cast<double *>(property->data);
            m_speedPercent = qRound(speed * 100.0);
            emit speedChanged(m_speedPercent);
        } else if (event->reply_userdata == volumeProperty &&
                   property->format == MPV_FORMAT_DOUBLE) {
            const double volume = *static_cast<double *>(property->data);
            m_volumePercent = qRound(volume);
            emit volumeChanged(m_volumePercent);
        }
    }
}

bool PlayerController::sendCommand(const char *arguments[]) {
    const int result = mpv_command(m_mpv, arguments);
    if (result < 0) {
        reportMpvError(tr("control playback"), result);
        return false;
    }
    return true;
}

void PlayerController::reportMpvError(const QString &operation, int errorCode) {
    const QString message = tr("Could not %1: %2")
                                .arg(operation, QString::fromUtf8(mpv_error_string(errorCode)));
    qWarning().noquote() << message;
    emit errorOccurred(message);
}

} // namespace radinue
