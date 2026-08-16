#include "player/PlayerController.h"

#include <QDebug>
#include <QString>

#include <mpv/client.h>

namespace radinue {

PlayerController::PlayerController(QObject *parent) : QObject(parent), m_mpv(mpv_create()) {
    if (m_mpv == nullptr) {
        qWarning() << "Could not create the libmpv context";
        return;
    }

    const int result = mpv_initialize(m_mpv);
    if (result < 0) {
        qWarning() << "Could not initialize libmpv:" << mpv_error_string(result);
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
    }
}

PlayerController::~PlayerController() {
    if (m_mpv != nullptr) {
        mpv_terminate_destroy(m_mpv);
    }
}

bool PlayerController::isAvailable() const noexcept { return m_mpv != nullptr; }

} // namespace radinue
