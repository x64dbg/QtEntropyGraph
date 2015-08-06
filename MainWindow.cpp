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

static void AddGraph(QGraphicsScene* scene, QRectF rect, const std::vector<double> & points, QColor color = Qt::black, int size = 1, qreal yMin = 0, qreal yMax = 1)
{
    int pointCount = points.size();
    if(!pointCount)
        return;
    qreal intervalX = rect.width() / ((qreal)pointCount - 1);
    qreal intervalY = rect.height() / (yMax - yMin);
    QPolygonF polyLine;
    for(int i = 0; i < pointCount; i++)
    {
        qreal x = i * intervalX;
        qreal y = points[i] * intervalY;
        QPointF point(rect.x() + x, rect.bottom() - y); //y direction is inverted...
        polyLine.append(point);
    }
    QPainterPath path;
    path.addPolygon(polyLine);
    scene->addPath(path, QPen(color, size));
}

static void GetFileEntropyGraph(const QString & fileName, int blockSize, std::vector<double> & points, int pointCount)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return;

    QByteArray fileData = file.readAll();
    file.close();

    unsigned char* data = (unsigned char*)fileData.constData();
    int dataSize = fileData.size();
    if(blockSize > dataSize)
        blockSize = dataSize;

    Entropy::MeasurePoints(data, dataSize, blockSize, points, pointCount);
}

void MainWindow::initializeGraph()
{
    if(_initialized)
        return;
    _initialized = true;

    //initialize scene
    QGraphicsScene* scene = new QGraphicsScene(this);
    qreal width = ui->graphicsView->width() - 5;
    qreal height = ui->graphicsView->height() - 5;
    _rect = QRectF(25, 10, width - 35, height - 20);
    _yMin = 0;
    _yMax = 1;
    _size = 1;

    //draw bounding box
    scene->addRect(QRectF(1, 1, width, height), QPen(Qt::black));

    //draw scale
    scene->addLine(15, _rect.top(), 15, _rect.bottom(), QPen(Qt::black, 2));
    const int xBegin = 10;
    const int xEnd = 20;
    qreal intervalY = _rect.height() / 10;
    for(int i = 0; i < 11; i++)
    {
        qreal y = _rect.top() + i * intervalY;
        scene->addLine(xBegin, y, xEnd, y, QPen(Qt::black, 2));
    }

    //set scene
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);
    ui->graphicsView->setScene(scene);
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
    AddGraph(ui->graphicsView->scene(), _rect, points, QColor(ui->editColor->text()), _size, _yMin, _yMax);

    nextColor();
}

void MainWindow::on_btnGraphEntropy_clicked()
{
    initializeGraph();

    std::vector<double> points;
    int blockSize = ui->editBlockSize->text().toInt();
    int pointCount = ui->editPointCount->text().toInt();
    GetFileEntropyGraph(ui->editPath->text(), blockSize, points, pointCount);
    AddGraph(ui->graphicsView->scene(), _rect, points, QColor(ui->editColor->text()), _size, _yMin, _yMax);

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
