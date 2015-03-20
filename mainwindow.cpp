#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTreeView>
#include <QString>
#include <QFileInfo>
#include <QDebug>
#include <QTime>
#include <QTreeWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStringList>
#include <QFileIconProvider>
#include <QIcon>

QT_CHARTS_USE_NAMESPACE

DonutBreakdownChart *donutBreakdown;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    time =QTime::currentTime();
    qsrand((uint)time.msec());
    model = new QFileSystemModel(this);
    //model->setReadOnly(true);
    model->setRootPath("/");
    ui->treeView->setModel(model);
    //QModelIndex index = model->index("/");
    //ui->treeView->setRootIndex(index);
    ui->treeView->resizeColumnToContents(0);
    //setCentralWidget(ui->treeView);

    QStringList labels;
    labels << "Name" << "Size" << "Date Modified" << "Path";
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(labels);

    ui->aORd->setText("Set to Descending");
    AscendingORDescending = true;
    ui->SizeORDate->setText("Set to Date");
    SizeororDate = true;
    donutBreakdown = new DonutBreakdownChart;
    chartView = new QChartView(donutBreakdown);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->horizontalLayout_4->addWidget(chartView);
    //setCentralWidget(ui->horizontalLayout_4);

}

MainWindow::~MainWindow()
{
    delete ui;
}

quint64 MainWindow::getDirectorySize(QString path){
    QFileInfo pathInfo(path);
    if(!pathInfo.exists() || !pathInfo.isReadable()){
        qDebug() << "Not readable" << path;
        return 0;
    }
    if(pathInfo.isFile())
    {
        filevector.push_back(pathInfo); qDebug() << "Item Popped";
        return pathInfo.size();
    }
    if (!pathInfo.isDir())
        return 0;
    QDir directory(path);
    quint64 totalSize = pathInfo.size();
    QFileInfoList dirList = directory.entryInfoList(QDir::Hidden | QDir::Files | QDir::Dirs |  QDir::NoDotAndDotDot |  QDir::AllEntries | QDir::AllEntries);
    for(int i = 0; i < dirList.size(); i++){
        if(!dirList[i].isReadable() || dirList[i].canonicalFilePath() == pathInfo.canonicalFilePath())
            continue;
        if(dirList[i].isSymLink())
            totalSize+= dirList[i].size();
        else if(dirList[i].isFile())
        {totalSize += dirList[i].size(); filevector.push_back(dirList[i]); qDebug() << "Item Pushed";}
        else if(dirList[i].isDir())
            totalSize += getDirectorySize(dirList[i].canonicalFilePath());

    }
    return totalSize;
}
void MainWindow::on_treeView_expanded(const QModelIndex &)
{
    ui->treeView->resizeColumnToContents(0);
}
void MainWindow::on_treeView_collapsed(const QModelIndex &)
{
    ui->treeView->resizeColumnToContents(0);
}
void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    children.clear();
    if(!index.isValid())
        return;
    donutBreakdown = new DonutBreakdownChart;
    QFileInfo fileInfo = model->fileInfo(index);
    theparent = fileInfo;
    QDir directory(fileInfo.canonicalFilePath());
    QFileInfoList folder = directory.entryInfoList(QDir::Files | QDir::Dirs |  QDir::NoDotAndDotDot | QDir::AllEntries);

    QFileInfoList subFolder;
    qDebug()<<"Folder";
    for ( int i =0; i <folder.size();i++)
        qDebug()<<folder[i].canonicalFilePath();

    donutBreakdown->legend()->setVisible(true);
    donutBreakdown->legend()->setAlignment(Qt::AlignRight);
    donutBreakdown->setTitle(fileInfo.baseName());

    //int discard = 0;
    if(fileInfo.isDir()){
        children.resize(folder.size());
        for(int i = 0; i < folder.size(); i++)
        {
            if(!folder[i].isDir())
            {
                children[i].resize(1);
                children[i][0].parentname = folder[i].baseName();
                children[i][0].absPath = folder[i].canonicalFilePath();
                children[i][0].fname = folder[i].baseName();
                children[i][0].fsize = getDirectorySize(folder[i].canonicalFilePath());
                continue;
            }
            directory.cd(folder[i].canonicalFilePath());
            subFolder = directory.entryInfoList(QDir::Files | QDir::Dirs |  QDir::NoDotAndDotDot | QDir::AllEntries);
            for ( int i =0; i <subFolder.size();i++)
                qDebug()<<"hell2 "<<subFolder[i].canonicalFilePath();
            children[i].resize(subFolder.size());
            for(int j = 0; j < subFolder.size(); j++)
            {
                //if(folder[i].isDir())
                children[i][j].parentname = folder[i].baseName();
                children[i][j].absPath = subFolder[j].canonicalFilePath();
                children[i][j].fname = subFolder[j].baseName();
                children[i][j].fsize = getDirectorySize(subFolder[j].canonicalFilePath());
                qDebug()<<i<<" "<<j<<" "<<children[i][j].fname<<"  "<<children[i][j].fsize<<"   "<<children[i][j].parentname;
            }
        }
    }
    QVector<QPieSeries*> series(children.size());
    for(int i=0; i<children.size();i++)
    {
        series[i] = new QPieSeries;
        series[i]->setName(children[i][0].parentname);
        for(int j=0; j< children[i].size();j++)
        {

            if (theparent.canonicalFilePath()!=children[i][j].absPath)
                series[i]->append(children[i][j].fname, children[i][j].fsize);

        }
        donutBreakdown->addBreakdownSeries(series[i],color());
    }
    chartView->setChart(donutBreakdown);

    sortFileVector (0, filevector.size()-1,SizeororDate);
    updateTable(AscendingORDescending);
}

void MainWindow::on_actionZoom_In_triggered()
{
    donutBreakdown->setScale(donutBreakdown->scale() + 0.1);
}

void MainWindow::on_actionZoom_Out_triggered()
{
    donutBreakdown->setScale(donutBreakdown->scale() - 0.1);
}

QColor MainWindow:: color()
{
    QColor *c =new QColor(qrand()%256,qrand()%256,qrand()%256,255 );
    return *c;
}


void MainWindow::sortFileVector (int s, int e, bool sizeORDate)
{
    if (s < e)
    {
        int q = Partition(s, e, sizeORDate);
        sortFileVector(s, q, sizeORDate);
        sortFileVector(q+1, e, sizeORDate);
    }
}
int MainWindow::Partition (int s, int e, bool sizeORDate)
{
    QFileInfo pivot = filevector[s];
    int i = s-1; int j = e+1;
    if (sizeORDate)
        while (true)
        {
            do j--; while (filevector[j].size() > pivot.size());
            do i++; while (filevector[i].size() < pivot.size());
            if (i<j) filevector[i].swap(filevector[j]);
            else return j;
        }
    else
        while (true)
        {
            do j--; while (filevector[j].lastModified().toMSecsSinceEpoch() > pivot.lastModified().toMSecsSinceEpoch());
            do i++; while (filevector[i].lastModified().toMSecsSinceEpoch() < pivot.lastModified().toMSecsSinceEpoch());
            if (i<j) filevector[i].swap(filevector[j]);
            else return j;
        }
}
void MainWindow::updateTable(bool ascORDes)
{
    while (ui->tableWidget->rowCount()>0) ui->tableWidget->removeRow(0);
    for (int j = 0; (j<20); j++)
    {
        int i;
        if (j<filevector.size())
        {
            if (ascORDes) i = j;
            else i = filevector.size() - 1 - j;
            QString x = filevector[i].lastModified().toString(Qt::ISODate);
            x.insert(x.indexOf("T")," ");
            x.remove(x.indexOf("T"),1);
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            //ui->tableWidget->setIcon()
            QFileIconProvider ip;
            QIcon Icon = ip.icon(filevector[i]);
            QTableWidgetItem* q= new QTableWidgetItem(filevector[i].fileName());
            q->setIcon(Icon);
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,q);
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(sizetoHuman(filevector[i].size())));
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,2,new QTableWidgetItem(x));
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,3,new QTableWidgetItem(filevector[i].absoluteFilePath()));
            ui->tableWidget->resizeColumnsToContents();
        }
    }
}
QString MainWindow::sizetoHuman(qint64 n)
{
    QString suffix = "BKMG";
    double N = n;
    QString suff;
    int i = 0;
    while (N>1023 && i <3)
    {
        N/=1024;
        i++;
    }
    suff = QString::number(N,'f',1)+" "+suffix[i];
    return suff;
}

void MainWindow::on_aORd_clicked()
{
    AscendingORDescending = !AscendingORDescending;
    if (!AscendingORDescending) ui->aORd->setText("Set to Ascending");
    else ui->aORd->setText("Set to Descending");
    updateTable(AscendingORDescending);

}

void MainWindow::on_SizeORDate_clicked()
{
    SizeororDate = !SizeororDate;
    if (!SizeororDate) ui->SizeORDate->setText("Set to Size");
    else ui->SizeORDate->setText("Set to Date Modified");
    sortFileVector(0, filevector.size()-1, SizeororDate);
    updateTable(AscendingORDescending);

}
