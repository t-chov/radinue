#pragma once

#include "player/PlayerController.h"

#include <QMainWindow>

namespace radinue {

class MainWindow final : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);

  private:
    PlayerController m_player;
};

} // namespace radinue
