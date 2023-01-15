#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->m_gbDataParseUI = new GBdataParse;
    this->m_canDataCapUI  = new candatacapture;

    /**>跳转国标数据解析页面 */
    connect(ui->action_gbParse, &QAction::triggered, this, [=]()
    {
        qDebug() << "跳转国标数据解析页面。";
        this->hide();/**>隐藏当前窗口 */
        this->m_gbDataParseUI->show();/**>显示跳转到的窗口 */
    });

    this->hide();/**>隐藏当前窗口 */
    this->m_canDataCapUI->show();/**>显示跳转到的窗口 */

    /**>转回主页面 */
    connect(this->m_gbDataParseUI, &GBdataParse::back, [=]{
        qDebug() << "回到主页面。";
        this->m_gbDataParseUI->hide();
        this->show();
    });

    qRegisterMetaType<canFrameStruct>("canFrameStruct");

    connect(this->m_canDataCapUI->m_skt_client, &QTcpSocket::disconnected, this, [=](){ /**>socket断开之后使能连接按钮 */
            qDebug() << "socket 断开连接";
            this->m_canDataCapUI->enable_connect_pushButton();
        });
    connect(this->m_canDataCapUI->m_skt_client, &QTcpSocket::connected, this,[=](){ /**>socket端口之后禁止连接按钮 */
            qDebug() << "socket 已连接";
            this->m_canDataCapUI->enable_disconnect_pushButton();
        });
    connect(this->m_canDataCapUI->m_skt_client, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this,[=](){ /**>socket连不上之后使能连接按钮 */
            qDebug() << "socket 连接失败";
            qDebug() << this->m_canDataCapUI->m_skt_client->errorString();
            this->m_canDataCapUI->enable_connect_pushButton();
        });
    connect(this->m_canDataCapUI->m_skt_client, &QTcpSocket::readyRead,
            this->m_canDataCapUI,&candatacapture::on_recvsoketMsg); /**>socket有数据接收 */

}

MainWindow::~MainWindow()
{
    delete ui;
}
