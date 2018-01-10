#ifndef DATARENDER_H
#define DATARENDER_H

#include <QPen>
#include <QColor>
#include "dataset.h"

// This class plots the line of the data
class DataPlot
{
public:
  DataPlot();

  // Get or set the data set
  void setDataSet(DataSet const* ds) { ds_ = ds; }
  DataSet const* dataSet() const { return ds_; }

  // Style of the line
  QPen const& pen(int label) const { return pen_[label]; }
  void setPen(int label, QPen const& pen) { pen_[label] = pen; }
  void setPen(int label, QColor const& color) {
    pen_[label] = QPen(color);
  }
  void setPen(int label, QColor const& color, qreal width) {
    pen_[label] = QPen(color, width);
  }

  // Parameters for single point mode?
  qreal spScale() const { return spScale_; }
  void setSpScale(qreal spScale) { spScale_ = spScale; }
  qreal spRadius() const { return spRadius_; }
  void setSpWidth(qreal spRadius) { spRadius_ = spRadius; }

  // Do the rendering (scale = picWidth / dataWidth)
  void render(QPainter *painter, QRect const& target, qreal start, qreal scale,
              qreal yPercent = 1.0);
  void renderSupportLine(QPainter *painter, QRect const& target, qreal start, qreal scale,
              qreal yPercent = 1.0);

  // Initialize the pens with default diff mode
  void initDiffMode(float width);

protected:
  DataSet const *ds_;

  // The pen of the plotted line (label0, label1, spline)
  QPen pen_[5];

  // Whether we are at diff mode?
  bool diff_mode_;

  // At which scale we shall use single point mode to plot?
  qreal spScale_;

  // The width of single points
  qreal spRadius_;
};

#endif // DATARENDER_H
