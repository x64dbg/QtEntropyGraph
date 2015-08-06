#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
    bool _initialized;
    QRectF _rect;
};

#endif // MAINWINDOW_H
