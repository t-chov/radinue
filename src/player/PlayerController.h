#pragma once

#include <QObject>

struct mpv_handle;

namespace radinue {

class PlayerController final : public QObject {
    Q_OBJECT

  public:
    explicit PlayerController(QObject *parent = nullptr);
    ~PlayerController() override;

    PlayerController(const PlayerController &) = delete;
    PlayerController &operator=(const PlayerController &) = delete;
    PlayerController(PlayerController &&) = delete;
    PlayerController &operator=(PlayerController &&) = delete;

    [[nodiscard]] bool isAvailable() const noexcept;

  signals:
    void errorOccurred(const QString &message);

  private:
    mpv_handle *m_mpv = nullptr;
};

} // namespace radinue
