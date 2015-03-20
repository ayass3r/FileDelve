#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtWidgets/QStatusBar>
#include <QtCharts/QChartView>
#include "mainslice.h"
#include "donutbreakdownchart.h"
#include <QColor>
#include <QFileInfo>
#include <QVector>
#include <QTableWidget>

QT_CHARTS_USE_NAMESPACE

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

    void on_treeView_expanded(const QModelIndex &index);

    void on_treeView_collapsed(const QModelIndex &index);

    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_aORd_clicked();

    void on_SizeORDate_clicked();

    QColor color();
private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
    quint64 getDirectorySize(QString path);
    struct strdou{
        QString fname;
        QString absPath;
        quint64 fsize;
        QString parentname;
    };
    QVector<QVector<strdou> >children;
    QGraphicsScene *scene;
    QChartView *chartView;
    QTime time;
    QFileInfo theparent;
    void sortFileVector (int s, int e, bool sizeORDate);
    void updateTable(bool ascORDes);
    int Partition (int s, int e, bool sizeORDate);
    QString sizetoHuman(qint64 n);
    QVector <QFileInfo> filevector;
    QTableWidget table_widget;
    bool AscendingORDescending;
    bool SizeororDate;
};

#endif // MAINWINDOW_H
