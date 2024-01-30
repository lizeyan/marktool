#ifndef DATASET_H
#define DATASET_H

#include <vector>
#include <string>
#include <QDateTime>
#include <QString>
#include <iostream>

struct DataRecord
{
public:
  DataRecord(qint64 time, int label=0, int label2=0) :
    time_(time), pv_(0), label_(label), label2_(label2)
  {
  }

  qint64 time() const { return time_; }
  qreal pv() const { return pv_; }
  qreal sv() const {return sv_;}
  int label() const { return label_; }
  int label2() const { return label2_; }

  void setLabel(int label) { label_ = label; }
  void selectPV(int index);
  void selectSV(int index);

  QDateTime toDateTime() const {
    return QDateTime::fromTime_t(time_);
  }

  std::vector<std::string> const& values() const {
    return values_;
  }
  void swapValues(std::vector<std::string>& values) {
    values.swap(values_);
  }

  std::string const& getValue(int index) const {
    return values_.at(index);
  }
  void setValue(int index, std::string const& value) {
    values_[index] = value;
  }

private:
  qint64 time_;
  qreal pv_, sv_;
  int label_, label2_;

  // The original values of this record are stored here, for later output
  std::vector<std::string> values_;
};

class DataSet
{
public:
  DataSet();
  ~DataSet();

  bool load(QString const& path, QString& errorMessage, int& colValue);
  bool save(QString const& path);

  // Expose directly the items_ container
  std::vector<DataRecord>& items() {
    return items_;
  }
  std::vector<DataRecord> const& items() const {
    return items_;
  }

  // Some simple wrappers around the items_ container
  int size() const { return (int)items_.size(); }
  qint64 dataSize() const { return qint64(time(items_.size()-1) - time(0)); }
  DataRecord const& at(int idx) const {
    return items_.at(idx);
  }

  qreal max_pv() const { return max_pv_; }
  qreal min_pv() const { return min_pv_; }
  qreal max_sv() const { return max_sv_; }
  qreal min_sv() const { return min_sv_; }
  qint64 max_time() const { return max_time_; }
  qint64 min_time() const { return min_time_; }
  qint64 min_time_span() const { return min_time_span_; }

  // Fast access to item properties
  qreal pv(int idx) const { return items_.at(idx).pv(); }
  qreal sv(int idx) const { return items_.at(idx).sv(); }
  qint64 time(int idx) const { return items_.at(idx).time(); }
  QDateTime dateTime(int idx) const { return items_.at(idx).toDateTime(); }
  int label(int idx) const { return items_.at(idx).label(); }
  int label2(int idx) const { return items_.at(idx).label2(); }
  void setLabel(int idx, int label) { items_[idx].setLabel(label); }

  // Get all columns
  std::vector<std::string> const& columns() const { return columns_; }
  bool has_label2() const { return has_label2_; }

  // Get the column of certain column
  int getColumnIndex(std::string const& name) const;

  // Select the given column as main pv column
  void selectPV(int index);

  void selectSV(int index);
  bool legalSV() const {return sv_index_ < int(columns().size()) && sv_index_ >= 0;}

  // Get the lower bound of the idx for given timestamp.
  // Return the smallest idx where time(idx) >= tm.
  int lower_bound(qint64 tm) const;
  int upper_bound(qint64 tm) const;

private:
  std::vector<DataRecord> items_;

  // The original column headers
  std::vector<std::string> columns_;

  // Whether we have the column "label2"?
  bool has_label2_;

  // Record the ever-seen max & min pv value.
  qreal max_pv_, min_pv_;
  qreal max_sv_, min_sv_;

  // Record the ever-seen max & min time value.
  qint64 max_time_, min_time_;

  // Record the min span of timestamp.
  qint64 min_time_span_;

  int sv_index_ = -1;
};

#endif // DATASET_H
