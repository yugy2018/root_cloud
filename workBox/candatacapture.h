#ifndef CANDATACAPTURE_H
#define CANDATACAPTURE_H

#include <QMainWindow>
#include <QColor>
#include <QVector>
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>


#include "canframestruct.h"

namespace Ui {
class candatacapture;
}

typedef enum{
    TB_WDG_COL_SEQ      = 0x00, /**> */
    TB_WDG_COL_DIRECT   = 0x01, /**>传输方向 */
    TB_WDG_COL_DATE     = 0x02, /**>时间标识 */
    TB_WDG_COL_FRAME_ID = 0x03, /**>帧ID */
    TB_WDG_COL_DATA_LEN = 0x04, /**>数据长度 */
    TB_WDG_COL_DATA     = 0x05, /**>数据 */
    TB_WDG_COL_NULL             /**>最后空一列 */
}table_widget_show_windows_colum_e;

typedef struct {
    int  columNumber;  /**>多少列 */
    int  columWidth;   /**>列宽 */
    char columName[64];/**>对应列的名字 */
}table_widget_title_t;/**>tableWidget列结构 */

class candatacapture : public QMainWindow
{
    Q_OBJECT

public:
    explicit candatacapture(QWidget *parent = 0);
    ~candatacapture();

    void  tableWidgetSendWindowsInit();
    void  tableWidgetShowWindowsInit();

    void enable_connect_pushButton();
    void enable_disconnect_pushButton();
    void print_frame(canFrameStruct *canframe);

    QTcpSocket *m_skt_client; //tcp socket client

protected:
    QColor m_columnBackGroundColor;
    int    m_rowCount;                 //发送帧数据tablewidget行数
    int    m_tableWidgCurRow =0;       //当前写入行
    unsigned int m_tableWidgseq =1;    //表格序号

//protected:
//    QVector<canFrameStruct> rcvCanDataFrame;

//    void run() override;

private slots:
    void on_pushButton_sendFrame_clicked();

    void on_pushButton_connect_clicked();

    void on_pushButton_disconnect_clicked();

public slots:
    void on_recvsoketMsg();


private:
    Ui::candatacapture *ui;
};

#endif // CANDATACAPTURE_H
