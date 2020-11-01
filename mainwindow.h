#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_btnSource_clicked();

  void on_btnDest_clicked();

  void on_btnLaunch_clicked();

	void on_actionAbout_triggered();

	void on_actionHelp_triggered();

private:
  Ui::MainWindow *ui;
  QSqlDatabase qsqlDB;

  // private methods
  bool createDBConnection(QString dbFile);
};
#endif // MAINWINDOW_H
