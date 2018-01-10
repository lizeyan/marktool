#ifndef DATAEDIT_H
#define DATAEDIT_H

#include <QWidget>
#include <QPixmap>
#include "dataset.h"
#include "datarange.h"

class DataEdit : public QWidget
{
  Q_OBJECT
public:
  explicit DataEdit(QWidget *parent = 0);

  void setDataSet(DataSet* ds) {
    ds_ = ds;
    range_.setDataSet(ds);
    dataSetChanged();
  }

  // Get and set the parameters
  DataRange const* range() const { return &range_; }
  DataRange* range() { return &range_; }

  // Get and set the y percentage.
  double yPercent() const { return yPercent_; }
  void set_yPercent(double p);

  // Get and set the diff mode
  bool diffMode() const { return diff_mode_; }
  void set_diffMode(bool diffMode);

signals:
  void dataModified();
  void dataRangeChanged(qreal start, qreal size);

public slots:
  void dataSetChanged();

protected slots:
  void myDataRangeChanged(qreal start, qreal size);

protected:
  friend class MainWindow; // forward event from MainWindow to DataEdit
  // Do the repaint
  void paintEvent(QPaintEvent *);

  // When the widget is resizing
  void resizeEvent(QResizeEvent *);

  // Wheel event to change the size of selection range
  void wheelEvent(QWheelEvent *);

  // Some reaction to the keyboard
  void keyPressEvent(QKeyEvent *);

  // Show the mark of cursor with a straight line
  void mouseMoveEvent(QMouseEvent *);

  // Move mouse with left or right button down, to change the label of
  // given points
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);

private:
  DataSet* ds_;

  // The display range
  DataRange range_;

  // The display percentage
  double yPercent_;

  // Whether we are in diff mode?
  bool diff_mode_;

  // Whether left or right button is down?
  bool leftButtonDown_, rightButtonDown_;

  // The scale of (picWidth / dataWidth)
  double picScale_, pvScale_;

  // recognize the index of mouse position
  qreal indexAtPos(QPointF const& pos) const;

  // Recalculate the size and scales of the components
  void refreshScales();

  // Track the last "x" where we've draw the straight line cursor
  int lastLineCursor_;

  // Cache the background of yticks
  QPixmap yticksCache_;
  void updateYticks();
};

#endif // DATAEDIT_H
