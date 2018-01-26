#ifndef DATARANGE_H
#define DATARANGE_H

#include <QObject>
#include "dataset.h"

class DataRange : public QObject
{
  Q_OBJECT
public:
  explicit DataRange(QObject *parent = 0);

  DataSet const* dataSet() const { return ds_; }
  void setDataSet(DataSet const* ds, qreal start=0, qreal size=24*60);

  // The range start and size.
  qreal start() const { return start_; }
  void setStart(qreal start) {
    setRange(start, size_);
  }
  qreal size() const { return size_; }
  void setSize(qreal size) {
    setRange(start_, size);
  }

  // Zoom in and out according to scale factor and mouse position
  void zoom(qreal scale, qreal mousePos = 0.5);

  // Reset the zoom so that size == 26 * 60
  void resetZoom(qreal mousePos = 0.5);

  // Move left and right by scale factor
  void shift(qreal factor);

  // Get the index at given position
  qreal indexAt(qreal pos) const {
    return start_ + pos * size_;
  }

signals:
  void rangeChanged(qreal start, qreal size);

public slots:
  void setRange(qreal start, qreal size);

private:
  DataSet const* ds_;
  qreal start_, size_;
};

#endif // DATARANGE_H
