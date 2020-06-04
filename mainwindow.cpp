#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QDebug"
#include "QtSql/QSqlDriver"
#include "QtSql/QSqlDatabase"
#include "QFileInfo"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

bool MainWindow::createDBConnection(QString dbFile)
{
  bool success = false;
  if (dbFile.isEmpty() == false)
  {
      // check if dbFile is an existant file
      if (QFileInfo::exists(dbFile) && QFileInfo(dbFile).isFile())
      {
          // dbFile exists and is a file (not a directory) so attempt to db connection to it
          qDebug() << "source:" << ui->leSource->text();
          // for some reason, the addDatabase call has to be a static call on the next line
          qsqlDB = QSqlDatabase::addDatabase("QSQLITE");
          qsqlDB.setDatabaseName(ui->leSource->text());
          bool success = qsqlDB.open();
          qDebug() << "db connection was:" << success;
          qsqlDB.close();

      }
  }
  else
  {
      // dbFile argument passed was empty
      qDebug() << "Argument dbFile was empty: '" << dbFile <<"'";
  }
  return success;
}

void MainWindow::on_btnSource_clicked()
{
  QString qstrSource = QFileDialog::getOpenFileName(this,
      tr("Select Source file..."), QDir::toNativeSeparators(QDir::homePath()), tr("Plex Database File (*.plugins.library.db*);;Any File (*.*)"));
  ui->leSource->setText(qstrSource);
  qDebug() << "qstrSource:" << qstrSource;
}

void MainWindow::on_btnDest_clicked()
{
  QString qstrDest = QFileDialog::getOpenFileName(this,
      tr("Select Destination file..."), QDir::toNativeSeparators(QDir::homePath()), tr("Plex Database File (*.plugins.library.db*);;Any File (*.*)"));
  ui->leDest->setText(qstrDest);
  qDebug() << "qstrDest:" << qstrDest;
}

void MainWindow::on_btnLaunch_clicked()
{
    // check the value of ui->coboAction and act accordingly
  if (ui->coboAction->currentText() == "Export from Source to Destination")
  {
  }
  else if (ui->coboAction->currentText() == "Import from Source to Destination")
  {
      qDebug() << "this is the import handling...";
  }
  else
  {
      // this should never happen unless there is a bug
      qDebug() << "some how this happened... ???";
  }
}
