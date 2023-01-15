#ifndef GBDATAPARSE_H
#define GBDATAPARSE_

#include <QMainWindow>
#include <QCloseEvent>
#include <QColor>
//#include <iostream>

#define FEI4_ITEM_PLATFORM  "非道路国四报文_平台转发格式解析"
#define FEI4_ITEM_DEVICE    "非道路国四报文_终端格式解析"
#define OTHER_ITEM          "其他，暂不支持"

namespace Ui {
class GBdataParse;
}

class GBdataParse : public QMainWindow
{
    Q_OBJECT

public:
    explicit GBdataParse(QWidget *parent = 0);
    ~GBdataParse();

protected:
    void closeEvent(QCloseEvent *event);
    void comboBoxDataTypeInit();
    void tableWidgetShowResultInit();
    void tableWidgetDelAllRow();

    QString getEncryptType(int index);
    QString getCmdUnitType(int index);

    void tableWidgetInsertData(int insertRow, QString fieldName, int dataValue, int nationalPlatFormValue, QString comment);
    void tableWidgetInsertData(int insertRow, QString fieldName, int dataValue, QString nationalPlatFormValue, QString comment);
    void tableWidgetInsertData(int insertRow, QString fieldName, QString value, QString comment);

    void showHeaderDeviceFormat(unsigned char *data);
    void showTakeDownWarnDataUnit(unsigned char *data);
    void showLoginDataUnit(unsigned char *data);
    void showLogoutDataUnit(unsigned char *data);
    void showRealtimeECDDataUnit(unsigned char *data);
    void showRealtimeFlowDataUnit(unsigned char *data);
    void showRealtimeHeaderDataUnit(unsigned char *data);

    unsigned int m_tableWidget_row;
    QColor m_backGroundColor;
    char   m_vin[17+1];
    int    m_tableWidgetRows = 0;

signals:
    void back();

private slots:
    void on_pushBtn_dataParse_clicked();

private:
    Ui::GBdataParse *ui;
};

#endif // GBDATAPARSE_H
