#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "worker.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  void paintEvent(QPaintEvent* event) noexcept override;
  void mouseMoveEvent(QMouseEvent *event) noexcept override;
  void mousePressEvent(QMouseEvent *event) noexcept override;
  void mouseReleaseEvent(QMouseEvent *event) noexcept override;
  void wheelEvent(QWheelEvent *event) noexcept override;
  void resizeEvent(QResizeEvent *event) noexcept override;

private:
  info_render info;
  bool pressed_mouse = false;
  QPoint last_click;
  worker m_worker;

public slots:
  void update_image() noexcept;
};
#endif // MAINWINDOW_H
