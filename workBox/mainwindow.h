#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gbdataparse.h"
#include "candatacapture.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    GBdataParse    * m_gbDataParseUI = NULL;
    candatacapture * m_canDataCapUI  = NULL;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
