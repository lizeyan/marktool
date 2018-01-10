#include "datarender.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>

namespace {

static const int TP = 0, TN = 1, FP = 2, FN = 3;

static const int Label2Idx[2][2] = {
    { TN, FP },
    { FN, TP },
};

} // namespace

DataPlot::DataPlot() :
  ds_(NULL),
  diff_mode_(false),
  spScale_(10.0),
  spRadius_(5.0)
{
  pen_[0] = QPen(Qt::blue, 2.0);
  pen_[1] = QPen(Qt::red, 2.0);
  pen_[2] = QPen(Qt::black, 1.0);
}

void DataPlot::initDiffMode(float width)
{
  pen_[0] = QPen(Qt::magenta, width);   // TP
  pen_[1] = QPen(Qt::gray, width);      // TN
  pen_[2] = QPen(Qt::red, width);       // FP
  pen_[3] = QPen(Qt::blue, width);      // FN
  pen_[4] = QPen(Qt::black, 1.0);
  diff_mode_ = true;
}

// Map a logical coordinate into plotting system
struct CordMapper
{
  qreal left, top, width, height, xmin, xmax, ymin, ymax, xscale, yscale;
  qreal yPercent;

  CordMapper(qreal left, qreal top, qreal width, qreal height, qreal xmin,
             qreal xmax, qreal ymin, qreal ymax, qreal yPercent) :
    left(left), top(top), width(width), height(height), xmin(xmin),
    xmax(xmax), ymin(ymin), ymax(ymax), xscale(width / (xmax - xmin)),
    yscale(height / ((ymax - ymin) * yPercent)), yPercent(yPercent)
  {
  }

  QPointF operator()(qreal x, qreal y) const {
    x = left + (x - xmin) * xscale;
    y = top + height - (y - ymin) * yscale;
    return QPointF(x, y);
  }
};

void DataPlot::render(QPainter *painter, const QRect &target, qreal start,
                      qreal scale, qreal yPercent)
{
  if (!ds_)
    return;

  const int width = target.width();
  const int height = target.height();
  const int left = target.left();
  const int top = target.top();

  // Determine the start offset, the unit size, and the essetial points
  //
  // Note that `start` is the start timestamp, so we should calculate the
  // start index.
  qint64 desiredStartTime = qint64(start - 1 - 1/scale);
  int startId = ds_->lower_bound(desiredStartTime) - 1;
  qint64 desiredEndTime = int(qCeil(start + width / scale) + 1 + 1/scale);
  int endId = ds_->upper_bound(desiredEndTime) + 1;
  if (startId < 0) startId = 0;
  if (endId > ds_->size()) endId = ds_->size();

  // Get the start & end time stamp according to start & end id
  qint64 startTime = ds_->time(startId);
  qint64 endTime = endId < ds_->size() ? ds_->time(endId) : ds_->max_time();
  qreal startX = (startTime - start) * scale;
  qreal endX = (endTime - start) * scale;

  // Helper function to map points
  CordMapper map(startX + left, top, endX - startX, height, startTime, endTime,
                 ds_->min_pv(), ds_->max_pv(), yPercent);

  // If endId <= startId, we do not draw anything
  if (endTime <= startTime)
    return;

  // If scale >= 1.0, we can draw every related points
  QPen pen[4], spPen[4];
  QPen missing_seg_pen(QBrush(QColor(173,255,477, 80)), 5);

  if (!diff_mode_) {
    // not in diff mode, normal 3 pens schema
    if (scale >= spScale_ / ds_->min_time_span()) {
      pen[0] = pen[1] = pen_[2];
      for (int i=0; i<2; ++i) {
        spPen[i] = pen_[i];
        spPen[i].setWidthF(spRadius_);
      }
    } else {
      pen[0] = pen_[0];
      pen[1] = pen_[1];
    }
  } else {
    // we are now in diff mode, use four pens
    if (scale >= spScale_ / ds_->min_time_span()) {
      for (int i=0; i<4; ++i) {
        pen[i] = pen_[4];
        spPen[i] = pen_[i];
        spPen[i].setWidthF(spRadius_);
      }
    } else {
      for (int i=0; i<4; ++i) {
        pen[i] = pen_[i];
      }
    }
  }

  if (/* scale >= 1.0 */ true) {
    // draw the first point
    painter->setPen(pen[ds_->label(startId)]);
    painter->drawPoint(map(ds_->time(startId), ds_->pv(startId)));

    for (int i=startId+1; i<endId; ++i) {
      bool is_missing_segment = false;
      if (ds_->time(i) - ds_->time(i - 1) > ds_->min_time_span())
          is_missing_segment = true;
      QPointF p1 = map(ds_->time(i-1), ds_->pv(i-1)),
          p2 = map(ds_->time(i), ds_->pv(i));
      QPointF pm = (p1 + p2) * 0.5;
      int label1 = ds_->label(i-1), label1_2 = ds_->label2(i-1),
          label2 = ds_->label(i), label2_2 = ds_->label2(i);

      if (!diff_mode_) {
        // If we are not in diff mode.
        if (label1 == label2) {
          painter->setPen(pen[label1]);
          painter->drawLine(p1, p2);
        } else {
          painter->setPen(pen[label1]);
          painter->drawLine(p1, pm);
          painter->setPen(pen[label2]);
          painter->drawLine(pm, p2);
        }
        if (is_missing_segment)
        {
            painter->save();
            painter->setPen(missing_seg_pen);
            painter->drawLine(p1, p2);
            painter->restore();
        }
      } else {
        // If we are in diff mode
        if (label1 == label2) {
          painter->setPen(pen[Label2Idx[label1][label1_2]]);
          painter->drawLine(p1, p2);
        } else {
          painter->setPen(pen[Label2Idx[label1][label1_2]]);
          painter->drawLine(p1, pm);
          painter->setPen(pen[Label2Idx[label2][label2_2]]);
          painter->drawLine(pm, p2);
        }
      }
    }

    // draw single points
    if (scale > spScale_ / ds_->min_time_span()) {
      for (int i=startId; i<endId; ++i) {
        if (!diff_mode_) {
          // If we are not in diff mode.
          painter->setPen(spPen[ds_->label(i)]);
        } else {
          // If we are in diff mode
          painter->setPen(spPen[Label2Idx[ds_->label(i)][ds_->label2(i)]]);
        }
        painter->drawEllipse(map(ds_->time(i), ds_->pv(i)), spRadius_/2,
                             spRadius_/2);
      }
    }
  }
}
