#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initializeGraph();
    void nextColor();

private slots:
    void on_btnRandomGraph_clicked();
    void on_btnGraphEntropy_clicked();
    void on_btnClear_clicked();
    void on_sliderBlockSize_sliderMoved(int position);
    void on_sliderPointCount_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    bool _initialized;
    QRectF _rect;
    std::vector<double> _entropyData;
    qreal _yMin;
    qreal _yMax;
    int _size;
};

#endif // MAINWINDOW_H
