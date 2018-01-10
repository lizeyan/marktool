#include "datarange.h"
#include <QDebug>

DataRange::DataRange(QObject *parent) :
  QObject(parent)
{
}

void DataRange::setDataSet(const DataSet* ds, qreal start, qreal size)
{
  ds_ = ds;
  setRange(start, size);
}

void DataRange::setRange(qreal start, qreal size)
{
  if (!ds_)
    return;
  size = std::max(std::min((qreal)ds_->dataSize(), size), 2.0);
  start = std::max(std::min(start, ds_->max_time() - size),
                   (qreal)ds_->min_time());
  if (start_ != start || size_ != size) {
    start_ = start;
    size_ = size;
    emit rangeChanged(start, size);
  }
}

void DataRange::zoom(qreal scale, qreal mousePos)
{
  setRange(start_ + size_ * mousePos * (1.0 - scale),
           size_ * scale);
}

void DataRange::resetZoom(qreal mousePos)
{
  qreal mid = start_ + size_ * mousePos;
  qreal size = 24 * 60;
  qreal start = mid - size / 2;
  setRange(start, size);
}

void DataRange::shift(qreal factor)
{
  setRange(start_ + factor * size_, size_);
}
