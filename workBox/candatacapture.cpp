#include "candatacapture.h"
#include "ui_candatacapture.h"

#include <QDebug>
#include <QDateTime>

table_widget_title_t tableWidgetShowWindowTitle[] =
{
    {TB_WDG_COL_SEQ     , 90 , "序号"},
    {TB_WDG_COL_DIRECT  , 90 , "传输方向"},
    {TB_WDG_COL_DATE    , 200, "时间标识"},
    {TB_WDG_COL_FRAME_ID, 120, "帧ID"},
    {TB_WDG_COL_DATA_LEN, 90 , "数据长度"},
    {TB_WDG_COL_DATA    , 300, "数据(HEX)"},
    {TB_WDG_COL_NULL    , 10 , ""}
};

/**
 * @brief candatacapture::tableWidgetSendWindowsInit
 * 发送帧的tablewidget串口初始化
 */
void candatacapture::tableWidgetSendWindowsInit()
{
    int i = 0;
    QStringList tableList;
    tableList << "帧类型"
              << "帧ID"
              << "数据(HEX)";
    ui->tableWidget_input->setColumnCount(3);                          //设置列数
    ui->tableWidget_input->verticalHeader()->setDefaultSectionSize(25);//设置每列默认高度
    ui->tableWidget_input->setHorizontalHeaderLabels(tableList);       //设置表格项

    ui->tableWidget_input->setColumnWidth(0, 60);//设置0列宽度
    ui->tableWidget_input->setColumnWidth(1, 100);//设置1列宽度
    ui->tableWidget_input->setColumnWidth(2, 200);//设置2列宽度

    m_columnBackGroundColor = QColor(0xF2, 0xF2, 0xF2);//表格背景填充颜色值

    m_rowCount = 99;//默认99行
    for(i=0; i<m_rowCount; i++)
    {
        ui->tableWidget_input->insertRow(i);
    }
}

void candatacapture::tableWidgetShowWindowsInit()
{
    QStringList tableList;
    int tableWidgetShowWindowsColNum = sizeof(tableWidgetShowWindowTitle) / sizeof(tableWidgetShowWindowTitle[0]);

    ui->tableWidget_show->setColumnCount(tableWidgetShowWindowsColNum); //设置列数
//    ui->tableWidget_show->setRowCount(900000);                        //设置行数
    ui->tableWidget_show->verticalHeader()->setDefaultSectionSize(25);  //设置每列默认高度

    for(int i=0; i<tableWidgetShowWindowsColNum; i++)
    {
        tableList << tableWidgetShowWindowTitle[i].columName;
        ui->tableWidget_show->setColumnWidth(tableWidgetShowWindowTitle[i].columNumber,tableWidgetShowWindowTitle[i].columWidth);
    }
    ui->tableWidget_show->setHorizontalHeaderLabels(tableList);         //设置表格项

    ui->tableWidget_show->horizontalHeader()->setStretchLastSection(true);//最后一列始终填充满窗口
//    ui->tableWidget_show->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white}");//设置表头背景颜色
    ui->tableWidget_show->setStyleSheet("background-color: rgb(80, 106, 187)");//设置背景颜色
    ui->tableWidget_show->verticalHeader()->hide();//取消行号
}

candatacapture::candatacapture(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::candatacapture)
{
    ui->setupUi(this);
    this->m_skt_client = new QTcpSocket(this);


    tableWidgetSendWindowsInit();
    tableWidgetShowWindowsInit();
}

candatacapture::~candatacapture()
{
    delete ui;
}

void candatacapture::on_pushButton_sendFrame_clicked()
{

}

void candatacapture::on_recvsoketMsg()
{
    int canframeCount = 0;
    canFrameStruct *canframe;

    QByteArray array = this->m_skt_client->readAll();
    qDebug() << "recv array size = " << array.size();
    canframe = (canFrameStruct *)array.data();
    canframeCount = array.size() / sizeof(canFrameStruct);

    if(array.size() % sizeof(canFrameStruct))
    {
        qDebug() << "erro data len received:" << array.size() ;
        return;
    }
    for(int i=0; i<canframeCount; i++)
    {
        print_frame(canframe);
        canframe++;
    }
}

void candatacapture::print_frame(canFrameStruct *canframe)
{
    qDebug() <<"id:" << QString().sprintf("0x%08X",canframe->can_id);
    qDebug() <<"len:" << QString().sprintf("0x%04X",canframe->can_dlc);
    for(int j=0; j<8; j++)
        qDebug() << QString().sprintf("0x%02X",canframe->data[j]);

    ui->tableWidget_show->insertRow(m_tableWidgCurRow);//加入一行
    ui->tableWidget_show->setItem(m_tableWidgCurRow, TB_WDG_COL_SEQ     , new QTableWidgetItem(QString::number(m_tableWidgseq++)));
    ui->tableWidget_show->setItem(m_tableWidgCurRow, TB_WDG_COL_DIRECT  , new QTableWidgetItem("接收"));//此行第0列输入值
    ui->tableWidget_show->setItem(m_tableWidgCurRow, TB_WDG_COL_DATE    , new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")));
    ui->tableWidget_show->setItem(m_tableWidgCurRow, TB_WDG_COL_FRAME_ID, new QTableWidgetItem(QString().sprintf("0x%08X",canframe->can_id)));
    ui->tableWidget_show->setItem(m_tableWidgCurRow, TB_WDG_COL_DATA_LEN, new QTableWidgetItem(QString::number(canframe->can_dlc)));
    ui->tableWidget_show->setItem(m_tableWidgCurRow, TB_WDG_COL_DATA    , new QTableWidgetItem(QString().sprintf("%02X %02X %02X %02X %02X %02X %02X %02X",
            canframe->data[0],
            canframe->data[1],
            canframe->data[2],
            canframe->data[3],
            canframe->data[4],
            canframe->data[5],
            canframe->data[6],
            canframe->data[7])));

    ui->tableWidget_show->scrollToBottom();//让滚动条拉到最下
    m_tableWidgCurRow++;
}

void candatacapture::enable_connect_pushButton()
{
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_connect->setDisabled(false);
}

void candatacapture::enable_disconnect_pushButton()
{
    ui->pushButton_disconnect->setEnabled(true);
    ui->pushButton_disconnect->setDisabled(false);

}

void candatacapture::on_pushButton_connect_clicked()
{
    QString serverIP   = ui->lineEdit_serverIP->text();
    QString serverPort = ui->lineEdit_serverPort->text();

    if(serverIP.isEmpty())
        serverIP = ui->lineEdit_serverIP->placeholderText();

    if(serverPort.isEmpty())
        serverPort = "16002";

    qDebug() <<"开始连接socket:" <<serverIP << ":" << serverPort;
    ui->pushButton_connect->setEnabled(false);
    ui->pushButton_connect->setDisabled(true);

    ui->pushButton_disconnect->setEnabled(false);
    ui->pushButton_disconnect->setDisabled(true);

    this->m_skt_client->connectToHost(serverIP,serverPort.toInt());
}

void candatacapture::on_pushButton_disconnect_clicked()
{
    qDebug() <<"断开socket连接";
    this->m_skt_client->disconnectFromHost();
}

//void candatacapture::run()
//{
////    QVector<canFrameStruct>::iterator iter =
//    quint32 can_id = 0x19000001;  //帧id
//    canFrameStruct canframe;
//    while(1)
//    {
//        canframe.can_id = can_id++;
//        canframe.can_dlc= 8;

//        signalSendMsg(canframe);
//        qDebug() << "running ";
//        sleep(1);
//    }

//}
