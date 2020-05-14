#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "FisheyeDistortionCorrection.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow              *ui;
    FisheyeDistortionCorrection *mCorrection;
    QImage                      mOriginalImage;
public Q_SLOTS:
    void openFileOnClicked();
    void processOnClicked();
    void generateBinOutput();
    void checkBinData();
};

#endif // MAINWINDOW_H
