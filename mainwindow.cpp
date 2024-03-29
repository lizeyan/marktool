#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringListModel>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  modified_(false),
  lockUpdateFeature_(false)
{
  ui->setupUi(this);
  ui->chkLabel2->setVisible(false);

  ui->dataEdit->installEventFilter(this);
  ui->dataScroll->installEventFilter(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

static const char* MonthNames[] = {
  "",
  "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
  "Sep", "Oct", "Nov", "Dec"
};

void MainWindow::on_dataScroll_startIndexChanged()
{
  QDateTime dt;
  dt = QDateTime::fromTime_t(static_cast<uint>(ui->dataScroll->startIndex()));
  ui->lblMonth->setText(tr("%1").arg(MonthNames[dt.date().month()]));
}

void MainWindow::on_dataScroll_selectionChanged()
{
  ui->dataEdit->range()->setRange(ui->dataScroll->selectStart(),
                                  ui->dataScroll->selectSize());
}

void MainWindow::on_btnOpen_clicked()
{
  static QString path = "*.*";
  if (modified_) {
    if (QMessageBox::question(this,
                              tr("Discard Changes?"),
                              tr("File has been modified, discard changes?"))
        != QMessageBox::Yes)
      return;
  }

  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open File ..."),
                                                  path);
  if (!fileName.isNull())
  {
      path = QFileInfo(fileName).path();
  }
  if (!fileName.isEmpty() && !fileName.isNull()) {
    QString errorMessage = "";
      int colValue = -1;
    if (!dataSet_.load(fileName, errorMessage, colValue)) {
      QMessageBox::critical(this,
                            tr("Cannot Load File"),
                            tr("File cannot be loaded.") + errorMessage
                            );
      return;
    }
    modified_ = false;
    filePath_ = fileName;
    setWindowTitle(filePath_);
    if (dataSet_.min_time_span() == 0)
    {
        QMessageBox::warning(this, "Warning", "Repeated timestamp found", QMessageBox::Ok);
    }
    ui->dataScroll->setDataSet(&dataSet_);
    ui->dataEdit->setDataSet(&dataSet_);
    ui->dataEdit->range()->setRange(dataSet_.min_time(), 24 * 60 * 60);
    QDateTime dt;
    dt = dataSet_.at(0).toDateTime();
    ui->lblMonth->setText(tr("%1").arg(MonthNames[dt.date().month()]));
    updateFeatureBox(colValue);
  }
}

void MainWindow::on_dataEdit_dataModified()
{
  modified_ = true;
  setWindowTitle(QString("%1 [Modified]").arg(filePath_));
  ui->dataScroll->invalidate_cache();
  ui->dataScroll->update();
}

void MainWindow::on_btnSave_clicked()
{
  if (!filePath_.isNull() && !filePath_.isEmpty()) {
    if (!dataSet_.save(filePath_)) {
      QMessageBox::critical(this,
                            tr("Cannot Save File"),
                            tr("File cannot be saved."));
      return;
    }
    modified_ = false;
    setWindowTitle(filePath_);
    ui->dataScroll->dataSetChanged();
  }
}

void MainWindow::on_dataEdit_dataRangeChanged(qreal start, qreal size)
{
  ui->dataScroll->setSelectRange(static_cast<int>(start), static_cast<int>(size));
}

void MainWindow::updateFeatureBox(int colValue)
{
  lockUpdateFeature_ = true;
  QStringList featureArray;
  for (std::vector<std::string>::const_iterator it=dataSet_.columns().begin();
       it != dataSet_.columns().end(); ++it) {
    featureArray << QString::fromStdString(*it);
  }
  features_.setStringList(featureArray);
  ui->selFeature->setModel(&features_);
  ui->selFeature->setCurrentIndex(colValue);

  ui->selSupportFeature->clear();
  ui->selSupportFeature->addItems(featureArray);
  ui->selSupportFeature->addItem("None");
  ui->selSupportFeature->setCurrentIndex(dataSet_.columns().size());
  ui->selSupportFeature->show();
  lockUpdateFeature_ = false;

  // set the status of chkLabel2
  ui->chkLabel2->setEnabled(dataSet_.has_label2());
  ui->chkLabel2->setChecked(false);
  ui->dataEdit->set_diffMode(false);
  ui->dataScroll->set_diffMode(false);
}

void MainWindow::on_selFeature_currentIndexChanged(int index)
{
  if (!lockUpdateFeature_ && index >= 0) {
    dataSet_.selectPV(index);
    ui->dataEdit->update();
    ui->dataScroll->dataSetChanged();
  }
}

void MainWindow::on_selSupportFeature_currentIndexChanged(int index)
{
    if (!lockUpdateFeature_ && index >= 0) {
        dataSet_.selectSV(index);
        ui->dataEdit->update();
        ui->dataScroll->dataSetChanged();
    }
}

void MainWindow::on_valueSlider_valueChanged(int value)
{
  ui->dataEdit->set_yPercent(double(value) / 100.0);
}

void MainWindow::on_chkLabel2_clicked()
{
  ui->dataEdit->set_diffMode(ui->chkLabel2->isChecked());
  ui->dataScroll->set_diffMode(ui->chkLabel2->isChecked());
}

void MainWindow::on_sameScaleChk_clicked()
{
    ui->dataEdit->set_sameScale(ui->sameScaleChk->isChecked());
    ui->dataScroll->set_sameScale(ui->sameScaleChk->isChecked());
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress && (watched == ui->dataEdit || watched == ui->dataScroll))
    {
        // forward all keypress event to dataedit and datascroll
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        ui->dataEdit->keyPressEvent(keyEvent);
        return false;
    }
    return QObject::eventFilter(watched, event);
}
