#ifndef DATASCROLL_H
#define DATASCROLL_H

#include <QWidget>
#include <QPixmap>
#include "dataset.h"

class DataScroll : public QWidget
{
  Q_OBJECT
public:
  explicit DataScroll(QWidget *parent = 0);

  void setDataSet(DataSet const* ds);

  // Get the value of parameters
  int startIndex() const { return startTime_; }
  int outlineSize() const { return outlineSize_; }
  int selectStart() const { return selectStart_; }
  int selectSize() const { return selectSize_; }

  // Set the selection range from outside
  void setSelectRange(int selectStart, int selectSize);

  // Get and set the diff mode
  bool diffMode() const { return diff_mode_; }
  void set_diffMode(bool diffMode);

signals:
  void startIndexChanged();
  void selectionChanged();

public slots:
  // Should be notified by someone else when dataset is updated
  void dataSetChanged();

protected:
  // Do the repaint
  void paintEvent(QPaintEvent *);

  // Some reaction to the keyboard
  void keyPressEvent(QKeyEvent *);

  // Handle the mouse event
  // We may drag the display range of scrollbar, or move the
  // selection range.
  void mousePressEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);

  // Wheel event to change the size of selection range
  void wheelEvent(QWheelEvent *);

private:
  DataSet const* ds_;

  // Whether we are in diff mode?
  bool diff_mode_;

  // How many points of outline can the widget display at one time?
  qint64 outlineSize_;

  // Which point should we start to display?
  qint64 startTime_;

  // The selection range?
  qint64 selectStart_, selectSize_;
  void EnsureSelectVisible();

  // Track the last point where the mouse is down
  QPoint mouseDownPos_;

  // Track the initial value of startIndex when we left click
  bool startTimeMove_;  // whether we are moving start index?
  qint64 initialStartTime_;

  // Track the initial value of selectStart when we right click
  bool selectStartMove_;  // whether we are moving select start?
  qint64 initialSelectStart_;

  // Cached picture of the dataset outline
  QPixmap pic_;
  double picScale_;  // Store the factor of (picWidth / ds_->size())

  void cachePic();
};

#endif // DATASCROLL_H
