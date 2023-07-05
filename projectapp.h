#ifndef PROJECTAPP_H
#define PROJECTAPP_H

#include <QDialog>
#include "qlistwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ProjectApp; }
QT_END_NAMESPACE

class ProjectApp : public QDialog
{
    Q_OBJECT

public:
    ProjectApp(QWidget *parent = nullptr);
    ~ProjectApp();

    void ConstructGraph(QString graphName, QVector<double> firstColumn, QVector<double> secondColumn);

private slots:
    void on_pushButton_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_itemChanged(QListWidgetItem *item);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_calculateButton_clicked();

    void on_mathSet1_clicked();

    void on_mathSet2_clicked();

    void on_mathSetButton1_clicked();

    void on_mathSetButton2_clicked();

    void on_colourList_itemClicked(QListWidgetItem *item);

private:
    Ui::ProjectApp *ui;
};
#endif // PROJECTAPP_H
