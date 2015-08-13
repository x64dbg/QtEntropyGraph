#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QGraphicsItem>
#include <QFile>
#include "Entropy.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _initialized = false;
}

void MainWindow::initializeGraph()
{
    if(_initialized)
        return;
    _initialized = true;
    ui->entropyView->InitializeGraph();
}

MainWindow::~MainWindow()
{
    delete ui;
}

static QColor randomColor()
{
    return QColor(rand() % 256, rand() % 256, rand() % 256);
}

void MainWindow::nextColor()
{
    ui->editColor->setText(randomColor().name());
}

static double fRand(double fMin = 0, double fMax = 1)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void MainWindow::on_btnRandomGraph_clicked()
{
    initializeGraph();

    std::vector<double> points;
    int pointCount = ui->editPointCount->text().toInt();

    for(int i = 0; i < pointCount; i++)
        points.push_back(fRand(fRand(), fRand()));
    points[0] = 0;
    points[points.size() - 1] = 1;
    ui->entropyView->AddGraph(points, QColor(ui->editColor->text()));

    nextColor();
}

void MainWindow::on_btnGraphEntropy_clicked()
{
    initializeGraph();

    int blockSize = ui->editBlockSize->text().toInt();
    int pointCount = ui->editPointCount->text().toInt();
    ui->entropyView->GraphFile(ui->editPath->text(), blockSize, pointCount, QColor(ui->editColor->text()));

    nextColor();
}

void MainWindow::on_btnClear_clicked()
{
    _initialized = false;
    initializeGraph();
}

void MainWindow::on_sliderBlockSize_sliderMoved(int position)
{
    ui->editBlockSize->setText(QString("%1").arg(position));
    on_btnClear_clicked();
    on_btnGraphEntropy_clicked();
}

void MainWindow::on_sliderPointCount_sliderMoved(int position)
{
    ui->editPointCount->setText(QString("%1").arg(position));
    on_btnClear_clicked();
    on_btnGraphEntropy_clicked();
}
