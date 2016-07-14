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
    ui->plainTextEditQueryResults->setPlainText(settings.value("QueryResults").toString());
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
    settings.setValue("QueryResults", ui->plainTextEditQueryResults->toPlainText());

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

    // Print out the results of the last query.
    QString resultsText;

    // The column names.
    for (int ii = 0; ii < query.record().count(); ++ii) {
        resultsText.append(query.record().fieldName(ii));
        resultsText.append('\t');
    }
    resultsText.append("\n========\n\n");

    // The results themselves.
    while (query.next()) {
        for (int ii = 0; ii < query.record().count(); ++ii) {
            resultsText.append(query.value(ii).toString());
            resultsText.append('\t');
        }
        resultsText.append('\n');
    }
    ui->plainTextEditQueryResults->setPlainText(resultsText);
}
