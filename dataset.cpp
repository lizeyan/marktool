#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>
#include <stdio.h>
#include "dataset.h"

typedef std::string String;
typedef std::vector<std::string> StrVector;

namespace {

void SplitInto(String const& s, char delim, StrVector *dst)
{
  String::const_iterator last = s.begin();
  dst->clear();
  for (String::const_iterator it=s.begin(); it!=s.end(); ++it) {
    if (*it == delim) {
      dst->push_back(String(last, it));
      last = it + 1;
    }
  }
  dst->push_back(String(last, s.end()));
}

std::string Join(StrVector const& src, char delim)
{
  std::string ret;
  StrVector::const_iterator it = src.begin();
  if (it != src.end()) {
    ret.append(*it);
    ++it;
  }
  while (it!=src.end()) {
    ret.push_back(delim);
    ret.append(*it);
    ++it;
  }
  return ret;
}

qint64 ToInt64(std::string const& s, bool *success=NULL)
{
  if (s == "") {
    return 0;
  } else {
    return QString::fromStdString(s).toLongLong(success);
  }
}

int ToInt(std::string const& s, bool *success=NULL)
{
  if (s == "") {
    return 0;
  } else {
    return QString::fromStdString(s).toInt(success);
  }
}

qreal ToReal(std::string const& s, bool *success=NULL)
{
  return QString::fromStdString(s).toDouble(success);
}

}  // namespace

void DataRecord::selectPV(int index)
{
  if (index >= 0 && index < (int)values_.size()) {
    bool ok = false;
    pv_ = ToReal(values_.at(index), &ok);
    if (!ok)
      pv_ = 0.0;
  }
}

void DataRecord::selectSV(int index)
{
  if (index >= 0 && index < (int)values_.size()) {
    bool ok = false;
    sv_ = ToReal(values_.at(index), &ok);
    if (!ok)
      sv_ = 0.0;
  }
}

DataSet::DataSet() :
  max_pv_(0),
  min_pv_(0xffffffff),
  max_time_(0),
  min_time_(0xffffffff),
  min_time_span_(1)
{
}

DataSet::~DataSet()
{
}

int DataSet::getColumnIndex(const std::string &name) const
{
  for (StrVector::const_iterator it=columns_.begin(); it!=columns_.end(); ++it)
  {
    if (*it == name)
      return it - columns_.begin();
  }
  return -1;
}

bool DataSet::load(const QString& path)
{
  QFile fin(path);
  if (!fin.open(QIODevice::ReadOnly))
    return false;

  QTextStream stream(&fin);
  stream.setCodec(QTextCodec::codecForName("utf-8"));

  QString line;

  // Check the column header
  if ((line = fin.readLine()).isNull()) {
    return false;
  }
  line = line.trimmed();

  StrVector columns;
  int colTime = -1, colLabel = -1, colLabel2 = -1, colValue = -1;

  SplitInto(line.toStdString(), ',', &columns);
  for (StrVector::const_iterator it=columns.begin(); it!=columns.end(); ++it) {
    QString colName = QString::fromStdString(*it).toLower().replace("\"", "").trimmed();
    if (colName == "timestamp" || colName == "time")
      colTime = it - columns.begin();
    else if (colName == "label")
      colLabel = it - columns.begin();
    else if (colName == "label2")
      colLabel2 = it - columns.begin();
    else if (colName == "value")
      colValue = it - columns.begin();
  }
  if (colTime == -1)
    return false;
  int maxRequired = colTime;

  // Load each record into a temporary array
  std::vector<DataRecord> temp;
  qint64 max_time = 0, min_time = 0xffffffff, min_time_span = 0xffffffff;
  qint64 last_time = 0;

  StrVector values;
  while (!(line = fin.readLine()).isNull()) {
    if ((line = line.trimmed()).isEmpty())
      break;
    SplitInto(line.toStdString(), ',', &values);
    if ((int)values.size() <= maxRequired)
      return false;

    int label = 0;
    int label2 = 0;
    bool ok1 = true, ok2 = true, ok3 = true;
    if (colLabel != -1)
      label = ToInt(values.at(colLabel), &ok2);
    if (colLabel2 != -1)
      label2 = ToInt(values.at(colLabel2), &ok3);

    qint64 timestamp = ToInt64(values.at(colTime), &ok1);
    if (timestamp > 32474608725) {
        timestamp = timestamp / 1000; // convert ms to s
    }
    DataRecord dr(timestamp,
                  label,
                  label2);
    if (!ok1 || !ok2 || !ok3)
      return false;

    dr.swapValues(values);

    // Add this record to temporary array and update local max/min values
    temp.push_back(dr);
    max_time = std::max(max_time, dr.time());
    min_time = std::min(min_time, dr.time());

    // update the min time span
    qint64 time_span = dr.time() - last_time;
    if (time_span < min_time_span) {
      min_time_span = time_span;
    }
    last_time = dr.time();
  }

  // If all records are read successfully, we then replace the temporary
  // array with the existence one, and update instance max/min values
  temp.swap(items_);
  max_time_ = max_time;
  min_time_ = min_time;
  min_time_span_ = min_time_span;
  has_label2_ = (colLabel2 != -1);
  columns_ = columns;

  // If "value" presents, select it as the main pv value
  if (colValue) {
    selectPV(colValue);
  } else {
    min_pv_ = 0;
    max_pv_ = 1;
  }

  return true;
}

void DataSet::selectPV(int index)
{
  qreal minValue = 1e10, maxValue = -1e10;

  for (std::vector<DataRecord>::iterator it = items_.begin();
       it != items_.end(); ++it) {
    it->selectPV(index);
    minValue = std::min(it->pv(), minValue);
    maxValue = std::max(it->pv(), maxValue);
  }

  min_pv_ = minValue - 1;
  max_pv_ = maxValue + 1;
}

void DataSet::selectSV(int index)
{
  sv_index_ = index;
  qreal minValue = 1e10, maxValue = -1e10;
  if (index >= columns().size())
      return;
  for (std::vector<DataRecord>::iterator it = items_.begin();
       it != items_.end(); ++it) {
    it->selectSV(index);
    minValue = std::min(it->sv(), minValue);
    maxValue = std::max(it->sv(), maxValue);
  }
  min_sv_ = minValue;
  max_sv_ = maxValue;
}

bool DataSet::save(const QString& path)
{
  QFile fout(path);
  if (!fout.open(QIODevice::WriteOnly)) {
    return false;
  }

  QTextStream stream(&fout);
  stream.setCodec(QTextCodec::codecForName("utf-8"));

  int colLabel = getColumnIndex("label");
  if (colLabel != -1) {
      stream << QString::fromStdString(Join(columns_, ',')) << "\n";
  } else {
      stream << QString::fromStdString(Join(columns_, ',')) << ",label\n";
  }
  foreach (DataRecord const& dr, items_) {
    StrVector::const_iterator it = dr.values().begin();
    int cid = 1;

    if (it != dr.values().end()) {
      if (colLabel == 0)
        stream << dr.label();
      else
        stream << QString::fromStdString(*it);
      ++it;
    }

    while (it != dr.values().end()) {
      stream << ",";
      if (colLabel == cid)
        stream << dr.label();
      else
        stream << QString::fromStdString(*it);
      ++cid; ++it;
    }

    if (colLabel == -1) {
        stream << "," << dr.label();
    }

    stream << "\n";
  }

  return true;
}

namespace {
bool IsTimeLess(DataRecord const& lhs, DataRecord const& rhs) {
  return lhs.time() < rhs.time();
}
}

int DataSet::lower_bound(qint64 tm) const
{
  DataRecord tmdr(tm, 0);
  return std::distance(items_.begin(),
                       std::lower_bound(items_.begin(), items_.end(),
                                        tmdr, IsTimeLess));
}

int DataSet::upper_bound(qint64 tm) const
{
  DataRecord tmdr(tm, 0);
  return std::distance(items_.begin(),
                       std::upper_bound(items_.begin(), items_.end(),
                                        tmdr, IsTimeLess));
}
