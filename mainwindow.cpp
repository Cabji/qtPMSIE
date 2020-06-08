#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QDebug"
#include "QtSql/QSqlDriver"
#include "QtSql/QSqlDatabase"
#include "QtSql/QSqlError"
#include "QtSql/QSqlQuery"
#include "QFileInfo"

const QList<QString> QLIST_DB_FIELD_NAMES = { "id", "account_id", "guid", "rating",
																							"view_offset", "view_count", "last_viewed_at",
																							"created_at", "updated_at", "skip_count",
																							"last_skipped_at", "changed_at", "extra_data",
																							"last_rated_at" };

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
					return success;
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
	// dev-note: QFileDialog::getSaveFileName can be used to 'select' a non-existant file using a standard file selection dialog
	// reference: https://stackoverflow.com/questions/29907625/qfiledialog-using-getopenfilename-allow-for-non-existent-files/29907690#29907690
	QString qstrDest = QFileDialog::getSaveFileName(this,
			tr("Select Destination file..."),
			QDir::toNativeSeparators(QDir::homePath()),
			tr("Plex Database File (*.plugins.library.db*);;Any File (*.*)"),
			nullptr,
			QFileDialog::DontConfirmOverwrite);
  ui->leDest->setText(qstrDest);
  qDebug() << "qstrDest:" << qstrDest;
}

void MainWindow::on_btnLaunch_clicked()
{
	// check the value of ui->coboAction and act accordingly
  if (ui->coboAction->currentText() == "Export from Source to Destination")
  {
			// exporting from database, attempt connection
			if (MainWindow::createDBConnection(ui->leSource->text()))
			{
					int iRowsTotal = 0;
					QSqlQuery query;
					query.prepare("SELECT * FROM metadata_item_settings");
					if (query.exec())
					{
						QFile qfileOutFile(ui->leDest->text());
						if (qfileOutFile.open(QIODevice::WriteOnly | QIODevice::Text))
						{
							QTextStream qtstOut(&qfileOutFile);
							qDebug() << "db query succeeded, numRowsAffected:" << query.numRowsAffected();
							// loop through the database query results
							query.first();
							do
							{
								 bool okCheck;
								 iRowsTotal++;
								 QString o = "INSERT INTO metadata_item_settings VALUES (";
								 // loop through the fieldNames
								 for (int i = 0; i < QLIST_DB_FIELD_NAMES.size(); ++i)
								 {
									 QString qstrFieldValue = query.value(i).toString();
									 if (qstrFieldValue.toInt(&okCheck, 10))
									 {
											 o.append(qstrFieldValue + ", ");
									 }
									 else if (qstrFieldValue.isEmpty())
									 {
										 o.append("NULL, ");
									 }
									 else
									 {
										 o.append("'" + qstrFieldValue + "', ");
									 }
								 }
								 o.chop(2);
								 o.append(");");
								 qtstOut << o << "\n";
							} while (query.next());
							qfileOutFile.close();
						}
						else
						{
							qDebug() << "Oops! Could not open output file '" << ui->leDest->text() << "' for writing.";
						}
					}
					else
					{
							qDebug() << "db query failed, error was: " << query.lastError();
					}
					qDebug() << "Total Rows Processed:" << iRowsTotal;
			}
			else
			{
				// database connection failed - should prolly let the user nkow about that?
			}
  }
  else if (ui->coboAction->currentText() == "Import from Source to Destination")
  {
			if (MainWindow::createDBConnection(ui->leDest->text()))
			{
				// open the sourceFile and loop it, pushing contents into destFile
				QFile qfileInFile(ui->leSource->text());
				if (qfileInFile.open(QIODevice::ReadOnly | QIODevice::Text))
				{
					qDebug() << "we are reading the source file...";
					while (!qfileInFile.atEnd())
					{
						QByteArray line = qfileInFile.readLine();
						line.chop(1);
						qDebug() << line;
					}
				}
				else
				{
					// file open for reading failed - should prolly let user know about that?
				}
			}
			else
			{
				// database connection failed - should prolly let the user nkow about that?
			}
  }
  else
  {
      // this should never happen unless there is a bug
      qDebug() << "some how this happened... ???";
  }
	qsqlDB.close();
}
