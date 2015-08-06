#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QGraphicsItem>
#include <QFile>
#include "Entropy.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _initialized = false;
}

class PointItem : public QGraphicsItem
{
public:
    explicit PointItem(QPointF point, QColor color, int size)
        : _point(point),
          _color(color),
          _size(size)
    {
        _rect = QRectF(_point.x() - _size / 2, _point.y() - _size / 2, _size, _size);
    }

    QRectF boundingRect() const
    {
        return _rect;
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setPen(_color);
        painter->drawRect(_rect);
    }

private:
    QPointF _point;
    QColor _color;
    int _size;
    QRectF _rect;
};

static void AddPoint(QGraphicsScene* scene, QPointF point, QColor color = Qt::black, int size = 4)
{
    scene->addItem(new PointItem(point, color, size));
}

static void AddGraph(QGraphicsScene* scene, QRectF rect, const QList<double> & points, QColor color = Qt::black, int size = 1, qreal yMin = 0, qreal yMax = 1)
{
    int pointCount = points.size();
    if(!pointCount)
        return;
    qreal intervalX = rect.width() / (qreal)pointCount;
    qreal intervalY = rect.height() / yMax - yMin;
    QPolygonF polyLine;
    for(int i = 0; i < pointCount; i++)
    {
        qreal x = i * intervalX + rect.x();
        qreal y = points[i] * intervalY + rect.y();
        QPointF point(x, y);
        polyLine.append(point);
        //AddPoint(scene, point, color, size * 2);
    }
    QPainterPath path;
    path.addPolygon(polyLine);
    scene->addPath(path, QPen(color, size));
}

static void GetFileEntropy(const QString & fileName, std::vector<double> & entropyData, int blockSize)
{
    entropyData.clear();

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return;

    QByteArray fileData = file.readAll();
    file.close();

    unsigned char* data = (unsigned char*)fileData.constData();
    int dataSize = fileData.size();
    unsigned char* block = new unsigned char[blockSize];

    entropyData.reserve(dataSize);
    for(int i=0; i<dataSize; i++)
        entropyData.push_back(Entropy::MeasureByte(data, dataSize, i, block, blockSize));

    delete[] block;
}

static void MakeEnropyGraph(const std::vector<double> & entropyData, QList<double> & points, int pointCount)
{
    if(pointCount < (int)entropyData.size())
    {
        double* pointData = new double[pointCount];
        Entropy::Average(entropyData.data(), entropyData.size(), pointData, pointCount);

        for(int i=0; i<pointCount; i++)
            points.append(pointData[i]);
        delete[] pointData;
    }
    else
    {
        for(int i=0; i<(int)entropyData.size(); i++)
            points.append(entropyData[i]);
    }
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
    _rect = QRectF(10, 10, width-10, height-10);

    //draw bounding box
    QRectF rect(1, 1, width, height);
    scene->addRect(rect, QPen(Qt::black));

    //set scene
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);
    ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static double fRand(double fMin = 0, double fMax = 1)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

static QColor randomColor(QColor mix = Qt::white)
{
    /*int r = (rand() % 256 + mix.red()) / 2;
    int g = (rand() % 256 + mix.green()) / 2;
    int b = (rand() % 256 + mix.blue()) / 2;
    return QColor(r, g, b);*/
    return QColor(rand() % 256, rand() % 256, rand() % 256);
}

void MainWindow::nextColor()
{
    ui->editColor->setText(randomColor().name());
}

void MainWindow::on_btnRandomGraph_clicked()
{
    initializeGraph();

    QList<double> points;
    int pointCount = ui->editPointCount->text().toInt();

    for(int i=0; i<pointCount; i++)
        points.append(fRand(fRand(), fRand()));
    AddGraph(ui->graphicsView->scene(), _rect, points, QColor(ui->editColor->text()));

    nextColor();
}

void MainWindow::on_btnGraphEntropy_clicked()
{
    initializeGraph();

    QList<double> points;
    int blockSize = ui->editBlockSize->text().toInt();
    int pointCount = ui->editPointCount->text().toInt();

    GetFileEntropy(ui->editPath->text(), _entropyData, blockSize);
    MakeEnropyGraph(_entropyData, points, pointCount);
    AddGraph(ui->graphicsView->scene(), _rect, points, QColor(ui->editColor->text()));
    ui->sliderPointCount->setMaximum(_entropyData.size());

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
    QList<double> points;
    ui->editPointCount->setText(QString("%1").arg(position));
    MakeEnropyGraph(_entropyData, points, position);
    on_btnClear_clicked();
    AddGraph(ui->graphicsView->scene(), _rect, points, QColor(ui->editColor->text()));
}
