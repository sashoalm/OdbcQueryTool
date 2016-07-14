#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Load settings.
    QSettings settings;
    restoreGeometry(settings.value("window/geometry").toByteArray());
    restoreState(settings.value("window/state").toByteArray());
    ui->splitter->restoreState(settings.value("window/splitter").toByteArray());
    ui->lineEditConnectionString->setText(settings.value("ConnectionString").toString());
    ui->plainTextEditSqlQuery->setPlainText(settings.value("Query").toString());

    if (!ui->lineEditConnectionString->text().isEmpty() && !ui->plainTextEditSqlQuery->toPlainText().isEmpty()) {
        on_actionExecute_Query_triggered();
    }
}

MainWindow::~MainWindow()
{
    // Save settings.
    QSettings settings;
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());
    settings.setValue("window/splitter", ui->splitter->saveState());
    settings.setValue("ConnectionString", ui->lineEditConnectionString->text());
    settings.setValue("Query", ui->plainTextEditSqlQuery->toPlainText());

    delete ui;
}

void MainWindow::on_actionExecute_Query_triggered()
{
    // Connect to the database.
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC3");
    db.setDatabaseName(ui->lineEditConnectionString->text());
    if (!db.open()) {
        QMessageBox::critical(this, "Error connecting", db.lastError().text());
        return;
    }

    // Execute the query.
    QSqlQuery query;
    query.setForwardOnly(true);

    foreach (const QString &ii, ui->plainTextEditSqlQuery->toPlainText().split(QRegExp("[\\r\\n;]"), QString::SkipEmptyParts)) {
        if (!query.exec(ii)) {
            QMessageBox::critical(this, "Error executing query", query.lastError().text());
            return;
        }
    }

    // Populate the QTableWidget with the results.
    // I could have used a QSqlQueryModel but it has the nasty habit
    // of locking the table for editing, and trying to insert items
    // from another program will fail. For small tables, it's better
    // to just retrieve all the items and populate a small table that
    // way.
    QSqlRecord record = query.record();
    ui->tableWidget->setColumnCount(record.count());
    for (int ii = 0; ii < record.count(); ++ii) {
        ui->tableWidget->setHorizontalHeaderItem(ii, new QTableWidgetItem(query.record().fieldName(ii)));
    }

    ui->tableWidget->setRowCount(0);
    while (query.next()) {
        ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
        for (int ii = 0; ii < query.record().count(); ++ii) {
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, ii, new QTableWidgetItem(query.value(ii).toString()));
        }
    }
}
