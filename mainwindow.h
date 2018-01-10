#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>
#include "dataset.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_dataScroll_selectionChanged();

  void on_dataScroll_startIndexChanged();

  void on_btnOpen_clicked();

  void on_dataEdit_dataModified();

  void on_btnSave_clicked();

  void on_dataEdit_dataRangeChanged(qreal start, qreal size);

  void on_selFeature_currentIndexChanged(int index);

  void on_valueSlider_valueChanged(int value);

  void on_chkLabel2_clicked();

private:
  Ui::MainWindow *ui;

  bool modified_;
  DataSet dataSet_;
  QString filePath_;
  QStringListModel features_;

  bool lockUpdateFeature_;
  void updateFeatureBox();
};

#endif // MAINWINDOW_H
