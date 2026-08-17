#pragma once

#include <QObject>
#include <QString>

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
    [[nodiscard]] bool hasFile() const noexcept;
    [[nodiscard]] bool isPaused() const noexcept;
    [[nodiscard]] qint64 positionMs() const noexcept;
    [[nodiscard]] qint64 durationMs() const noexcept;
    [[nodiscard]] int speedPercent() const noexcept;

    bool loadFile(const QString &filePath, bool paused = true);
    void togglePause();
    void setPaused(bool paused);
    void seekRelative(qint64 offsetMs);
    void seekAbsolute(qint64 positionMs);
    void setSpeedPercent(int speedPercent);

  signals:
    void errorOccurred(const QString &message);
    void fileLoaded(const QString &filePath);
    void pauseChanged(bool paused);
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void speedChanged(int speedPercent);
    void endOfFile();

  private:
    static void wakeup(void *context);
    void processEvents();
    bool sendCommand(const char *arguments[]);
    void reportMpvError(const QString &operation, int errorCode);

    mpv_handle *m_mpv = nullptr;
    QString m_filePath;
    bool m_paused = true;
    qint64 m_positionMs = 0;
    qint64 m_durationMs = 0;
    int m_speedPercent = 100;
};

} // namespace radinue
