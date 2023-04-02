#include "mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QSize>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), info({-5 / 2., -5 / 2.}, {1000, 1000}, 1/200., 1)
{
  resize(1000, 1000);
  connect(&m_worker, &worker::image_is_done, this, &MainWindow::update_image);
}

void MainWindow::update_image() noexcept
{
  update();
}

void MainWindow::paintEvent(QPaintEvent *event) noexcept
{
  QPainter(this).drawImage(0, 0, m_worker.get_image());
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) noexcept
{
  if (!pressed_mouse) {
    event->ignore();
  }
  info.m_start.first += (last_click - event->globalPos()).x() * info.m_scale;
  info.m_start.second += (last_click - event->globalPos()).y() * info.m_scale;
  last_click = event->globalPos();
  info.m_block_size = 32;
  m_worker.request(info);
  event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event) noexcept
{
  if (event->button() == Qt::LeftButton) {
    pressed_mouse = true;
    last_click = event->globalPos();
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) noexcept
{
  if (event->button() == Qt::LeftButton) {
    pressed_mouse = false;
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::wheelEvent(QWheelEvent *event) noexcept
{
  QPointF real_pos = event->position() * info.m_scale;
  double next_scale = event->angleDelta().y() > 0 ? info.m_scale * 0.9 : info.m_scale / 0.9;
  real_pos -= event->position() * next_scale;

  info.m_start.first  += real_pos.x();
  info.m_start.second += real_pos.y();
  info.m_scale = next_scale;
  info.m_block_size = 32;

  m_worker.request(info);
  event->accept();
}

void MainWindow::resizeEvent(QResizeEvent *event) noexcept
{
  info.m_size.first = event->size().width();
  info.m_size.second = event->size().height();
  info.m_block_size = 32;
  m_worker.request(info);
  QMainWindow::resize(event->size());
  event->accept();
}

