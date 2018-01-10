#include <QPainter>
#include <QDateTime>
#include <QDebug>
#include <QMouseEvent>
#include "datascroll.h"
#include "datarender.h"

static const qint64 ONE_DAY = 86400;

DataScroll::DataScroll(QWidget *parent) :
  QWidget(parent),
  ds_(NULL),
  outlineSize_(0),
  startTime_(0),
  selectStart_(0),
  selectSize_(0),
  startTimeMove_(false),
  initialStartTime_(0)
{
  setFocusPolicy(Qt::ClickFocus);
}

void DataScroll::dataSetChanged()
{
  pic_ = QPixmap();
  update();
}

void DataScroll::setDataSet(const DataSet *ds)
{
  ds_ = ds;
  startTime_ = ds->min_time();
  selectStart_ = ds_->min_time();
  selectSize_ = 24 * 60 * 60;  // that is 1 day
  dataSetChanged();
  emit startIndexChanged();
}

void DataScroll::set_diffMode(bool diffMode)
{
  diff_mode_ = diffMode;
  pic_ = QPixmap();
  update();
}

void DataScroll::setSelectRange(int selectStart, int selectSize)
{
  selectStart_ = selectStart;
  selectSize_ = selectSize;
  EnsureSelectVisible();
  update();
  emit startIndexChanged();
}

void DataScroll::cachePic()
{
  if (!ds_)
    return;

  const int width = this->width();
  const int height = this->height();

  // This is just an empirical formula
  outlineSize_ = std::min(ONE_DAY * 15, ds_->dataSize());

  // We cache the whole outline in a single picture, not only the displayed
  // part.
  const int picWidth = width * (double(ds_->dataSize()) / outlineSize_);

  // Update other flags.
  picScale_ = double(picWidth) / ds_->dataSize();

  // Test whether we should update the picture?
  if (!pic_.isNull() && pic_.height() == this->height() &&
      pic_.width() == picWidth)
    return;
  pic_ = QPixmap(picWidth, height);

  // Prepare the painter
  QPainter painter(&pic_);
  painter.fillRect(pic_.rect(), Qt::white);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  // Prepare day margin lines
  QPen marginPen(Qt::gray);
  marginPen.setWidth(1);
  painter.setPen(marginPen);

  // Discover the first boundary of day
  int minDay = ds_->min_time() / ONE_DAY;
  int maxDay = (ds_->max_time() + ONE_DAY - 1) / ONE_DAY;

  for (int i=minDay; i<maxDay; ++i) {
    qreal x = qreal(i * ONE_DAY - ds_->min_time()) / ds_->dataSize() * picWidth;
    painter.drawLine(QPointF(x, 0), QPointF(x, height));
  }

  // Add dayTexts (while we temporary restore the painter's window)
  QFont font("Sans Serif", height / 4, QFont::Bold);
  painter.setFont(font);
  painter.setWindow(pic_.rect());

  for (int i=minDay; i<=maxDay; ++i) {
    int iTime = i * ONE_DAY - ds_->min_time();
    QRectF rect(iTime * picScale_, 0, ONE_DAY * picScale_, height);
    QDate dt = QDateTime::fromTime_t(i * ONE_DAY).date();
    QString text = QString::number(dt.day());
    painter.drawText(rect, Qt::AlignCenter, text);
  }

  // Now plot the points on the picture
  DataPlot dataPlot;
  dataPlot.setDataSet(ds_);
  if (!diff_mode_) {
    dataPlot.setPen(0, Qt::blue, 1.0);
    dataPlot.setPen(1, Qt::red, 1.0);
  } else {
    dataPlot.initDiffMode(1.0);
  }
  dataPlot.render(&painter, pic_.rect(), ds_->min_time(), picScale_);
}

void DataScroll::paintEvent(QPaintEvent *e)
{
  if (!ds_)
    return;

  // Try to build the picture cache
  cachePic();
  if (pic_.isNull())
    return;

  // Now paste the picture cache onto screen
  QPainter painter(this);
  QRect paintRect((startTime_-ds_->min_time()) * picScale_,
                  0,
                  outlineSize_ * picScale_,
                  pic_.height());
  painter.drawPixmap(this->rect(), pic_, paintRect);

  // Finally draw the selection range
  QBrush brush(QColor::fromHsv(100, 255, 255, 80));
  painter.fillRect((selectStart_ - startTime_) * picScale_,
                   0,
                   selectSize_ * picScale_,
                   height(),
                   brush);

  e->accept();
}

// Listen to mouse events, so that we can drag the scroll bar around,
// and change the selection range
void DataScroll::mousePressEvent(QMouseEvent* e)
{
  if (!ds_)
    return;

  // start to drag the scroll bar
  if (e->button() == Qt::LeftButton) {
    startTimeMove_ = true;
    initialStartTime_ = startTime_;
    mouseDownPos_ = e->pos();
  }

  // set the new position for selection range
  if (e->button() == Qt::RightButton) {
    selectStart_ = e->pos().x() / picScale_ + startTime_ - selectSize_ / 2;
    selectStart_ = std::max(std::min(selectStart_, ds_->max_time()-selectSize_),
                            ds_->min_time());
    selectStartMove_ = true;
    initialSelectStart_ = selectStart_;
    mouseDownPos_ = e->pos();
    update();
    emit selectionChanged();
  }

  // reset the size of selection range
  if (e->button() == Qt::MiddleButton) {
    int delta = (24 * 60 - selectSize_);
    selectStart_ -= delta / 2;
    selectSize_ = 24 * 60 * 60;
    update();
    emit selectionChanged();
  }

  e->accept();
}

void DataScroll::mouseMoveEvent(QMouseEvent* e)
{
  if (!ds_)
    return;

  // drag the scroll bar
  if (startTimeMove_) {
    startTime_ = initialStartTime_ -
                  (e->pos().x() - mouseDownPos_.x()) / picScale_;
    startTime_ = std::max(std::min(startTime_, ds_->max_time() - outlineSize_),
                          ds_->min_time());
    emit startIndexChanged();
    update();
  }

  // move the selection range
  if (selectStartMove_) {
    selectStart_ = initialSelectStart_ +
                   (e->pos().x() - mouseDownPos_.x()) / picScale_;
    selectStart_ = std::max(std::min(selectStart_, ds_->max_time()-selectSize_),
                            ds_->min_time());
    emit selectionChanged();
    update();
  }

  e->accept();
}

void DataScroll::mouseReleaseEvent(QMouseEvent* e)
{
  if (!ds_)
    return;

  // stop to drag the scroll bar
  if (e->button() == Qt::LeftButton) {
    startTimeMove_ = false;
  }

  // stop to drag the selection range
  if (e->button() == Qt::RightButton) {
    selectStartMove_ = false;
  }

  e->accept();
}

void DataScroll::EnsureSelectVisible()
{
  qint64 minStart = selectStart_ + selectSize_ - outlineSize_;
  qint64 maxStart = selectStart_;
  startTime_ = std::min(std::max(startTime_, minStart), maxStart);
}

// Listen to keyboard events to perform additional actions
void DataScroll::keyPressEvent(QKeyEvent* e)
{
  e->accept();
  return;
//  if (!ds_)
//    return;
//  // Move the selection towards left
//  constexpr qreal moveFactor = 0.9;
//  if (e->key() == Qt::Key_Left) {
//    selectStart_ = std::max(qint64(selectStart_ - selectSize_ * moveFactor),
//                            ds_->min_time());
//  // Move the selection towards right
//  } else if (e->key() == Qt::Key_Right) {
//    selectStart_ = std::min(qint64(selectStart_ + selectSize_ * moveFactor),
//                            ds_->max_time() - selectSize_);
//  } else {
//    return;
//  }

//  EnsureSelectVisible();
//  update();
//  emit startIndexChanged();
//  emit selectionChanged();
//  e->accept();
}

// Listen to wheel event to change the size of selection range
void DataScroll::wheelEvent(QWheelEvent* e)
{
  QWidget::wheelEvent(e);
}
