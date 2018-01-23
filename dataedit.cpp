#include "dataedit.h"
#include "datarender.h"
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QWheelEvent>
#include <QtMath>

DataEdit::DataEdit(QWidget *parent)
    : QWidget(parent), ds_(NULL), yPercent_(1.0), leftButtonDown_(false),
      rightButtonDown_(false), picScale_(1.0), pvScale_(1.0),
      lastLineCursor_(0) {
  setFocusPolicy(Qt::ClickFocus);
  setMouseTracking(true);
  connect(&range_, SIGNAL(rangeChanged(qreal, qreal)), this,
          SLOT(myDataRangeChanged(qreal, qreal)));
}

void DataEdit::set_yPercent(double p) {
  yPercent_ = p;
  updateYticks();
  update();
}

void DataEdit::dataSetChanged() {
  refreshScales();
  updateYticks();
  update();
}

void DataEdit::refreshScales() {
  if (!ds_)
    return;
  picScale_ = double(this->width()) / range_.size();
  pvScale_ = double(this->height()) / (ds_->max_pv() - ds_->min_pv());
}

void DataEdit::myDataRangeChanged(qreal start, qreal size) {
  emit dataRangeChanged(start, size);
  update();
}

void DataEdit::resizeEvent(QResizeEvent *) { refreshScales(); }

void DataEdit::updateYticks() {
  yticksCache_ = QPixmap(width(), height());
  QPainter painter(&yticksCache_);
  painter.fillRect(yticksCache_.rect(), Qt::white);

  // Draw the lines
  QPen linePen(Qt::gray);
  painter.setPen(linePen);
  for (int i = 0; i < 10; ++i) {
    int y = (10 - i) * height() * 0.1;
    painter.drawLine(QPoint(0, y), QPoint(width(), y));
  }

  // Draw the texts
  QFont labelFont("Sans Serif", 10);
  painter.setFont(labelFont);

  for (int i = 0; i < 10; ++i) {
    qreal val =
        i * (ds_->max_pv() - ds_->min_pv()) * yPercent_ * 0.1 + ds_->min_pv();
    QString s = QLocale(QLocale::English).toString(val, 'f', 2);
    int y = (10 - i) * height() * 0.1;
    painter.drawText(5, y - 10, s);
  }
}

namespace {

const QPen lastWeekPen[2] = {
    QColor::fromHsv(100, 128, 200),
    QColor::fromHsv(0, 128, 200),
};

const QPen lastDayPen[2] = {
    QColor::fromHsv(50, 128, 200),
    QColor::fromHsv(0, 128, 200),
};
const QPen dayPen[2] = {QPen(Qt::blue, 2.0), QPen(Qt::red, 2.0)};

} // namespace

void DataEdit::set_diffMode(bool diffMode) {
  diff_mode_ = diffMode;
  refreshScales();
  updateYticks();
  update();
}

void DataEdit::paintEvent(QPaintEvent *e) {
  if (!ds_)
    return;
  refreshScales();

  // Initialize the painter
  QPainter painter(this);
  QRect clip = e->region().boundingRect();

  painter.setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform);
  painter.setClipRegion(clip);
  painter.fillRect(clip, Qt::white);

  //=================
  // Draw the Y-ticks
  //-----------------
  if (yticksCache_.isNull() || yticksCache_.width() != width() ||
      yticksCache_.height() != height()) {
    updateYticks();
  }
  painter.drawPixmap(clip, yticksCache_, clip);

  //=========================
  // Now draw the data points
  //-------------------------

  DataPlot dataPlot;
  dataPlot.setDataSet(ds_);

  if (!diff_mode_) {
    //    // The last day data
    //    qreal dayShift = - 24 * 60 * 60;
    //    for (int i=0; i<2; ++i)
    //      dataPlot.setPen(i, lastDayPen[i]);
    //    dataPlot.render(&painter,
    //                    clip,
    //                    clip.left() / picScale_ + range_.start() + dayShift,
    //                    picScale_,
    //                    yPercent_);

    //    // The last week data
    //    qreal weekShift = - 24 * 60 * 7 * 60;
    //    for (int i=0; i<2; ++i)
    //      dataPlot.setPen(i, lastWeekPen[i]);
    //    dataPlot.render(&painter,
    //                    clip,
    //                    clip.left() / picScale_ + range_.start() + weekShift,
    //                    picScale_,
    //                    yPercent_);

    // Current day data
    for (int i = 0; i < 2; ++i)
      dataPlot.setPen(i, dayPen[i]);
    dataPlot.render(&painter, clip, clip.left() / picScale_ + range_.start(),
                    picScale_, yPercent_);
    dataPlot.renderSupportLine(&painter, clip,
                               clip.left() / picScale_ + range_.start(),
                               picScale_, yPercent_);
  } else {
    // Current day data
    dataPlot.initDiffMode(2.0);
    dataPlot.render(&painter, clip, clip.left() / picScale_ + range_.start(),
                    picScale_, yPercent_);
  }

  //================
  // Show the Legend
  /*
    if (!diff_mode_) {
      // If we are not in diff mode
      QRect legendBox(width() - 160, 20, 140, 66);
      painter.setPen(QPen(Qt::black, 1.0));
      painter.fillRect(legendBox, Qt::white);
      painter.drawRect(legendBox);

      QString labels[3] = {
        "Today",
        "Last Day",
        "Last Week"
      };
      QPen labelPens[3] = {
        dayPen[0],
        lastDayPen[0],
        lastWeekPen[0]
      };
      QFont labelFont("Sans Serif", 10);
      painter.setFont(labelFont);
      for (int i=0; i<3; ++i) {
        painter.setPen(labelPens[i]);

        QRect textBox(legendBox.left() + 70, legendBox.top() + 3 + i * 20, 60,
    20); painter.drawText(textBox, Qt::AlignVCenter, labels[i]);

        QPointF p1(legendBox.left() + 15, textBox.top() + textBox.height() *
    0.5), p2(p1.x() + 40, p1.y()); painter.drawLine(p1, p2);
      }
    } else {
      // If we are in diff mode
      QRect legendBox(width() - 130, 20, 110, 86);
      painter.setPen(QPen(Qt::black, 1.0));
      painter.fillRect(legendBox, Qt::white);
      painter.drawRect(legendBox);

      QString labels[4] = {
        "TP",
        "TN",
        "FP",
        "FN",
      };
      QFont labelFont("Sans Serif", 10);
      painter.setFont(labelFont);
      for (int i=0; i<4; ++i) {
        QPen pen = dataPlot.pen(i);
        pen.setWidth(1);
        painter.setPen(pen);

        QRect textBox(legendBox.left() + 70, legendBox.top() + 3 + i * 20, 30,
    20); painter.drawText(textBox, Qt::AlignVCenter, labels[i]);

        QPointF p1(legendBox.left() + 15, textBox.top() + textBox.height() *
    0.5), p2(p1.x() + 40, p1.y()); painter.drawLine(p1, p2);
      }
    }
    */

  //=======================================
  // Add the edit cursor if picScale >= 1.0

  if (picScale_ >= 1.0 / ds_->min_time_span()) {
    QPen penCursor(Qt::black);
    QPoint pos = mapFromGlobal(QCursor::pos());

    penCursor.setWidth(1.0);
    painter.setPen(penCursor);
    painter.drawLine(QPoint(pos.x(), 0), QPoint(pos.x(), height()));
  }

  e->accept();
}

// Listen to wheel event to change the size of selection range
void DataEdit::wheelEvent(QWheelEvent *e)
{
    int delta = e->angleDelta().y();
    constexpr int factor = 120;
    constexpr qreal zf = 0.8;
    if (delta > 0)
    {
        int times = delta / factor;
        for (int i = 0; i < times; ++i)
        {
            range_.zoom(1.0 / zf);
        }
    }
    else if (delta < 0)
    {
        int times = -delta / factor;
        for (int i = 0; i < times; ++i)
        {
            range_.zoom(zf);
        }
    }
    e->accept();
}

// React to keyboard actions
void DataEdit::keyPressEvent(QKeyEvent *e) {
  if (!ds_)
    return;

  const qreal zf = 0.8;
  const qreal sf = 0.1;

  // Strike "left", move the data towards left
  if (e->key() == Qt::Key_Left || e->key() == Qt::Key_A) {
    range_.shift(-sf);
  }

  // Srike "right", move the data towards right
  else if (e->key() == Qt::Key_Right || e->key() == Qt::Key_D) {
    range_.shift(sf);
  }

  // Strike "up", zoom in the data
  else if (e->key() == Qt::Key_Up || e->key() == Qt::Key_W) {
    range_.zoom(zf);
  }

  // Strike "down", zoom out the data
  else if (e->key() == Qt::Key_Down || e->key() == Qt::Key_S) {
    range_.zoom(1.0 / zf);
  }

  // Strike "0", reset the zoom
  else if (e->key() == Qt::Key_0) {
    range_.resetZoom();
  }
}

// Start or end the marking process
void DataEdit::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton)
    leftButtonDown_ = true;
  if (e->button() == Qt::RightButton)
    rightButtonDown_ = true;

  e->accept();
}

void DataEdit::mouseReleaseEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton)
    leftButtonDown_ = false;
  if (e->button() == Qt::RightButton)
    rightButtonDown_ = false;

  e->accept();
}

qreal DataEdit::indexAtPos(const QPointF &pos) const {
  qint64 ret = ds_->lower_bound(range_.indexAt(pos.x() / width()));
  return ret;
}

// Track the mouse to show a straight line
void DataEdit::mouseMoveEvent(QMouseEvent *e) {
  if (!ds_)
    return;

  // Only care about the clicks in the widget
  if (e->pos().x() < 0 || e->pos().x() >= width() || e->pos().y() < 0 ||
      e->pos().y() >= height())
    return;

  // Ignore if picScale < 1.0
  if (picScale_ < 1.0 / ds_->min_time_span())
    return;

  if (leftButtonDown_ || rightButtonDown_) {
    // change the label of points
    qint64 startIdx = indexAtPos(QPointF(lastLineCursor_, 0));
    qint64 endIdx = indexAtPos(QPointF(e->pos().x(), 0));

    if (startIdx > endIdx)
      std::swap(startIdx, endIdx);

    int label = (leftButtonDown_ ? 1 : 0);
    for (int i = startIdx; i < endIdx; ++i) {
      if (i >= 0 && i < ds_->size())
        ds_->items()[i].setLabel(label);
    }

    emit dataModified();
  }

  // Wrap around lastLineCusor and currentX
  int regionX1 = std::min(lastLineCursor_ - 2, e->pos().x() - 2);
  int regionX2 = std::max(lastLineCursor_ + 2, e->pos().x() + 2);
  update(QRect(regionX1, 0, regionX2 - regionX1 + 1, height()));
  lastLineCursor_ = e->pos().x();

  e->accept();
}
