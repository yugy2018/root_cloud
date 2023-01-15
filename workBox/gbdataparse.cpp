#include "gbdataparse.h"
#include "ui_gbdataparse.h"
#include "fei4/fei4_data_handler.h"

#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QTableWidgetItem>

/**
 * @brief getGBdate 组装时间字符串
 * @param data      必须要传符合yy mm dd hh mm ss留个字节的数据首地址
 * @return
 */
static QString getGBdate(unsigned char *data)
{
    QString date_yyyymmddhhmmss;
    date_yyyymmddhhmmss.sprintf("20%02d-%02d-%02d %02d:%02d:%02d",data[0], data[1], data[2], data[3], data[4], data[5] );
    return date_yyyymmddhhmmss;
}

/**
 * @brief GBdataParse::getEncryptType 获取加密类型
 * @param index 报文中解析到的加密类型字段
 * @return
 */
QString GBdataParse::getEncryptType(int index)
{
    QString encryptType;

    switch (index) {
    case 1:
        encryptType = "数据不加密";
        break;
    case 2:
        encryptType = "数据经过RSA算法加密";
        break;
    case 3:
        encryptType = "数据经过国密SM2算法加密";
        break;
    case 0xFE:
        encryptType = "异常";
        break;
    case 0xFF:
        encryptType = "无效";
        break;

    default:
        encryptType = "不知道嘛玩意！";
        break;
    }
    return encryptType;
}

/**
 * @brief GBdataParse::getCmdUnitType获取命令单元标识
 * @param index 命令单元字段值
 * @return
 */
QString GBdataParse::getCmdUnitType(int index)
{
    QString cmdUnitType;

    switch (index) {
    case CMD_TYPE_DEVICE_LOGIN:
        cmdUnitType = "机械登入";
        break;
    case CMD_TYPE_DEVICE_REALTIME_DATA:
        cmdUnitType = "实时信息上报";
        break;
    case CMD_TYPE_DEVICE_RESEND:
        cmdUnitType = "补发信息上报";
        break;
    case CMD_TYPE_DEVICE_LOGOUT:
        cmdUnitType = "机械登出";
        break;
    case CMD_TYPE_DEVICE_TAKE_DOWN_WARN:
        cmdUnitType = "拆除报警";
        break;

    default:
        cmdUnitType = "未知的命令单元";
        break;
    }
    return cmdUnitType;
}

/**
 * @brief GBdataParse::comboBoxDataTypeInit
 * 初始化combobox控件下拉菜单项
 */
void GBdataParse::comboBoxDataTypeInit()
{
    QStringList items;
    items << FEI4_ITEM_DEVICE
          << FEI4_ITEM_PLATFORM
          << OTHER_ITEM;
    ui->comboBox_dataType->addItems(items);

}

/**
 * @brief GBdataParse::tableWidgetDelAllRow
 * 删除tableWidget行
 */
void GBdataParse::tableWidgetDelAllRow()
{
    /// tableWidget行索引是随表格操作变动的，所以全删时从后面往前删避免异常
    for(int i = ui->tableWidget_showResult->rowCount() - 1; i >= 0; i--)
    {
        ui->tableWidget_showResult->removeRow(i);
    }
}

/**
 * @brief GBdataParse::tableWidgetShowResultInit
 * 初始化tableWidget表格（表头、列宽...）
 */
void GBdataParse::tableWidgetShowResultInit()
{
    QStringList tableList;
    tableList << "字段"
              << "值(十六进制)"
              << "值(十进制)"
              << "值(国家平台显示)"
              << "备注";
    ui->tableWidget_showResult->setColumnCount(5);//设置列数
    ui->tableWidget_showResult->verticalHeader()->setDefaultSectionSize(35);//设置每列默认高度
    ui->tableWidget_showResult->setHorizontalHeaderLabels(tableList);//设置表格项

    ui->tableWidget_showResult->setColumnWidth(0, 150);//设置0列宽度
    ui->tableWidget_showResult->setColumnWidth(1, 100);//设置1列宽度
    ui->tableWidget_showResult->setColumnWidth(2, 200);//设置2列宽度
    ui->tableWidget_showResult->setColumnWidth(3, 250);//设置3列宽度
    ui->tableWidget_showResult->setColumnWidth(4, 450);//设置4列宽度

    m_backGroundColor = QColor(0xF2, 0xF2, 0xF2);
}

GBdataParse::GBdataParse(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GBdataParse)
{
    ui->setupUi(this);

    //设置窗口顶置: 一直在最前面.
//    Qt::WindowFlags m_flags = windowFlags();
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint); /**>一直置顶窗口 */
    setWindowFlags(NULL); /**>取消置顶窗口，点别的窗口就可以压下这个窗口 */
//    setAttribute(Qt::WA_TranslucentBackground);

    comboBoxDataTypeInit();
    tableWidgetShowResultInit();
}

/**
 * @brief GBdataParse::closeEvent 重写父类虚函数，串口点击‘x’关闭前执行此函数
 * @param event
 */
void GBdataParse::closeEvent(QCloseEvent *event)
{
    /**>弹窗是否转回主页面 */
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("关闭页面"), QString(tr("返回主页面")), QMessageBox::Yes | QMessageBox::No );
    if(QMessageBox::No == button)
    {
        qDebug() << "直接关闭国标数据解析页面，进程退出。";
    }
    else if(QMessageBox::Yes == button)
    {
        emit this->back();
        qDebug() << "发信号给主页面。";
    }

    event->accept();//接收退出信号，程序退出
//    event->ignore();//忽略退出信号，程序继续
}

GBdataParse::~GBdataParse()
{
    delete ui;
}

void GBdataParse::tableWidgetInsertData(int insertRow, QString fieldName, int dataValue, int nationalPlatFormValue, QString comment)
{
    int j =0;

    ui->tableWidget_showResult->insertRow(insertRow);//加入一行
    ui->tableWidget_showResult->setItem(insertRow, 0, new QTableWidgetItem(fieldName));//此行第0列输入值
    ui->tableWidget_showResult->setItem(insertRow, 1, new QTableWidgetItem(QString::number(dataValue, 16)));
    ui->tableWidget_showResult->setItem(insertRow, 2, new QTableWidgetItem(QString::number(dataValue)));
    if(dataValue == 0xFF || dataValue == 0xFFFF)
    {
        ui->tableWidget_showResult->setItem(insertRow, 3, new QTableWidgetItem("无效"));
    }
    else
    {
        ui->tableWidget_showResult->setItem(insertRow, 3, new QTableWidgetItem(QString::number(nationalPlatFormValue)));
    }

    ui->tableWidget_showResult->setItem(insertRow, 4, new QTableWidgetItem(comment));

    if(insertRow % 2)/**>格行显示颜色 */
    {
        for(j=0; j < 5; j++)
        {
            ui->tableWidget_showResult->item(insertRow, j)->setBackgroundColor(m_backGroundColor);
        }
    }
}

void GBdataParse::tableWidgetInsertData(int insertRow, QString fieldName, int dataValue, QString nationalPlatFormValue, QString comment)
{
    int j =0;

    ui->tableWidget_showResult->insertRow(insertRow);
    ui->tableWidget_showResult->setItem(insertRow, 0, new QTableWidgetItem(fieldName));
    ui->tableWidget_showResult->setItem(insertRow, 1, new QTableWidgetItem(QString::number(dataValue, 16)));
    ui->tableWidget_showResult->setItem(insertRow, 2, new QTableWidgetItem(QString::number(dataValue)));
    ui->tableWidget_showResult->setItem(insertRow, 3, new QTableWidgetItem(nationalPlatFormValue));
    ui->tableWidget_showResult->setItem(insertRow, 4, new QTableWidgetItem(comment));

    if(insertRow % 2)/**>格行显示颜色 */
    {
        for(j=0; j < 5; j++)
        {
            ui->tableWidget_showResult->item(insertRow, j)->setBackgroundColor(m_backGroundColor);
        }
    }
}

void GBdataParse::tableWidgetInsertData(int insertRow, QString fieldName, QString value, QString comment)
{
    int j =0;

    ui->tableWidget_showResult->insertRow(insertRow);
    ui->tableWidget_showResult->setItem(insertRow, 0, new QTableWidgetItem(fieldName));
    ui->tableWidget_showResult->setItem(insertRow, 1, new QTableWidgetItem("-"));
    ui->tableWidget_showResult->setItem(insertRow, 2, new QTableWidgetItem("-"));
    ui->tableWidget_showResult->setItem(insertRow, 3, new QTableWidgetItem(value));
    ui->tableWidget_showResult->setItem(insertRow, 4, new QTableWidgetItem(comment));

    if(insertRow % 2)/**>格行显示颜色 */
    {
        for(j=0; j < 5; j++)
        {
            ui->tableWidget_showResult->item(insertRow, j)->setBackgroundColor(m_backGroundColor);
        }
    }
}

void GBdataParse::showTakeDownWarnDataUnit(unsigned char *data)
{
    int tmpValue  = 0;
    double tmpDoubleValue = 0.0;

//    QString date_yyyymmddhhmmss;
//    date_yyyymmddhhmmss.sprintf("20%02d-%02d-%02d %02d:%02d:%02d",data[0], data[1], data[2], data[3], data[4], data[5] );
    tableWidgetInsertData(m_tableWidgetRows++, "采集时间"   , getGBdate(data), "-");

    tmpValue = data[6] << 8 | data[7];
    tableWidgetInsertData(m_tableWidgetRows++, "信息流水号", tmpValue, tmpValue,"-");

    tmpValue = data[8] & 1;
    tableWidgetInsertData(m_tableWidgetRows++, "定位信息-状态位", data[8], tmpValue==0 ? "有效定位" : "无效定位", "-");
    tmpValue = data[9] << 8 | data[10];
    tmpValue = tmpValue << 8 | data[11];
    tmpValue = tmpValue << 8 | data[12];
    tmpDoubleValue = tmpValue * 0.000001;
    tableWidgetInsertData(m_tableWidgetRows++, "经度", tmpValue, QString::number(tmpDoubleValue, 'f', 6), "精度：0.000001");
    tmpValue = data[13] << 8 | data[14];
    tmpValue = tmpValue << 8 | data[15];
    tmpValue = tmpValue << 8 | data[16];
    tmpDoubleValue = tmpValue * 0.000001;
    tableWidgetInsertData(m_tableWidgetRows++, "纬度", tmpValue, QString::number(tmpDoubleValue, 'f', 6), "精度：0.000001");

    tableWidgetInsertData(m_tableWidgetRows++, "拆除状态", data[17], data[17]==0 ? "控制诊断信息可以获取" : "控制诊断信息无法获取", "-");
}

void GBdataParse::showLoginDataUnit(unsigned char *data)
{
    int tmpValue  = 0;
    tableWidgetInsertData(m_tableWidgetRows++, "采集时间"   , getGBdate(data), "-");

    tmpValue = data[6] << 8 | data[7];
    tableWidgetInsertData(m_tableWidgetRows++, "登入流水号", tmpValue, tmpValue,"-");

    char   iccid[20+1];
    memcpy(iccid, &data[8], 20);
    tableWidgetInsertData(m_tableWidgetRows++, "SIM卡iccid号", iccid,"-");
}

void GBdataParse::showLogoutDataUnit(unsigned char *data)
{
    int tmpValue  = 0;
    tableWidgetInsertData(m_tableWidgetRows++, "采集时间"   , getGBdate(data), "-");

    tmpValue = data[6] << 8 | data[7];
    tableWidgetInsertData(m_tableWidgetRows++, "登出流水号", tmpValue, tmpValue,"-");
}

/**
 * @brief getECDprotocol排放控制诊断信息-诊断协议
 * @param index
 * @return
 */
static QString getECDprotocol(int index)
{
    QString ECDprotocol;
    switch (index) {
    case 0:
        ECDprotocol = "ISO 15765";
        break;
    case 1:
        ECDprotocol = "ISO 27145";
        break;
    case 2:
        ECDprotocol = "SAEJ1939";
        break;
    case 3:
        ECDprotocol = "ISO 15031";
        break;
    case 0xFE:
        ECDprotocol = "无效";
        break;
    default:
        ECDprotocol = "不认识的东西";
        break;
    }
    return ECDprotocol;
}

/**
 * @brief getECDledStat排放控制诊断信息-报警灯状态
 * @param index
 * @return
 */
static QString getECDledStat(int index)
{
    QString ECDprotocol;
    switch (index) {
    case 0:
        ECDprotocol = "未点亮";
        break;
    case 1:
        ECDprotocol = "点亮";
        break;
    case 2:
        ECDprotocol = "闪烁";
        break;

    default:
        ECDprotocol = "不认识的东西";
        break;
    }
    return ECDprotocol;
}
/**
 * @brief GBdataParse::showRealtimeECDDataUnit排放控制诊断信息
 * @param data
 */
void GBdataParse::showRealtimeECDDataUnit(unsigned char *data)
{
   tableWidgetInsertData(m_tableWidgetRows++, "排放控制诊断协议", data[0], getECDprotocol(data[0]),"-");
   tableWidgetInsertData(m_tableWidgetRows++, "排放控制报警灯状态", data[1], getECDledStat(data[1]),"-");
   if(data[2] == 0xFE)
   {
       tableWidgetInsertData(m_tableWidgetRows++, "排放控制故障码总数", "无效，不继续解析","-");
       return;
   }
   else
   {
       tableWidgetInsertData(m_tableWidgetRows++, "排放控制故障码总数", data[2], data[2],"-");
   }

   unsigned int ECDcode;
   QString ECDcodeList = "";
   if(data[2])
   {
       for(int i = 0; i < data[2]; i++)
       {
           ECDcode = data[3+(i*4)] << 8 |data[4+(i*4)];
           ECDcode = ECDcode << 8 |data[5+(i*4)];
           ECDcode = ECDcode << 8 |data[6+(i*4)];
           ECDcodeList += QString::number(ECDcode).sprintf("%08X ",ECDcode);
       }
       tableWidgetInsertData(m_tableWidgetRows++, "排放控制故障码列表", ECDcode, ECDcodeList,"-");
   }
   else
   {
       tableWidgetInsertData(m_tableWidgetRows++, "排放控制故障码列表", data[2], "","-");
   }

}

/**
 * @brief GBdataParse::showRealtimeECDDataUnit数据流信息
 * @param data
 */
void GBdataParse::showRealtimeFlowDataUnit(unsigned char *data)
{
    unsigned int tmpValue =0;
    tmpValue = data[0] << 8 | data[1];
    tableWidgetInsertData(m_tableWidgetRows++, "车速"          , tmpValue, tmpValue/256  ,"精度：1/256 km/h per bit");
    tableWidgetInsertData(m_tableWidgetRows++, "大气压"        , data[2]  , data[2]/2     ,"精度：0.5 kPa/bit");
    tableWidgetInsertData(m_tableWidgetRows++, "柴油机净输出扭矩", data[3]  , data[3]-125  ,"精度：1%/bit，偏移：-125");
    tableWidgetInsertData(m_tableWidgetRows++, "摩擦扭矩"       , data[4]  , data[4]-125  ,"精度：1%/bit，偏移：-125");

    tmpValue = data[5] << 8 | data[6];
    tableWidgetInsertData(m_tableWidgetRows++, "柴油机转速"     , tmpValue  , tmpValue/8   ,"精度：0.125/bit");
    tmpValue = data[7] << 8 | data[8];
    tableWidgetInsertData(m_tableWidgetRows++, "柴油机燃料流量"  , tmpValue  , tmpValue/20   ,"精度：0.05/bit");
    tmpValue = data[9] << 8 | data[10];
    tableWidgetInsertData(m_tableWidgetRows++, "SCR上游NOx传感器输出值"  , tmpValue  , tmpValue/20 -200  ,"精度：0.05/bit 偏移：-200");
    tmpValue = data[11] << 8 | data[12];
    tableWidgetInsertData(m_tableWidgetRows++, "SCR下游NOx传感器输出值"  , tmpValue  , tmpValue/20 -200  ,"精度：0.05/bit 偏移：-200");

    tableWidgetInsertData(m_tableWidgetRows++, "反应剂余量"     , data[13]  , data[13]*4/10   ,"精度：0.4/bit");
    tmpValue = data[14] << 8 | data[15];
    tableWidgetInsertData(m_tableWidgetRows++, "进气量"  , tmpValue  , tmpValue/20  ,"精度：0.05/bit");
    tmpValue = data[16] << 8 | data[17];
    tableWidgetInsertData(m_tableWidgetRows++, "SCR入口温度"  , tmpValue  , tmpValue*3125/100000 -273  ,"精度：0.03125/bit 偏移：-273");
    tmpValue = data[18] << 8 | data[19];
    tableWidgetInsertData(m_tableWidgetRows++, "SCR出口温度"  , tmpValue  , tmpValue*3125/100000 -273  ,"精度：0.03125/bit 偏移：-273");
    tmpValue = data[20] << 8 | data[21];
    tableWidgetInsertData(m_tableWidgetRows++, "DFP压差"  , tmpValue  , tmpValue/10  ,"精度：0.1/bit");

    tableWidgetInsertData(m_tableWidgetRows++, "柴油机冷却液温度"  , data[22]  , data[22]-40  ,"偏移：-40");
    tableWidgetInsertData(m_tableWidgetRows++, "油箱液位"  , data[23]  , data[23]*4/10  ,"精度：0.4/bit");
    tmpValue = data[24] << 8 | data[25];
    tableWidgetInsertData(m_tableWidgetRows++, "实际的EGR阀开度"  , tmpValue  , tmpValue*25/10000  ,"精度：0.0025/bit");
    tmpValue = data[26] << 8 | data[27];
    tableWidgetInsertData(m_tableWidgetRows++, "设定的EGR阀开度"  , tmpValue  , tmpValue*25/10000  ,"精度：0.0025/bit");
}

/**
 * @brief GBdataParse::showRealtimeHeaderDataUnit 解析实时数据（有两个子类型：排放控制诊断信息、数据流信息）
 * @param data
 */
void GBdataParse::showRealtimeHeaderDataUnit(unsigned char *data)
{
    int tmpValue  = 0;
    double tmpDoubleValue = 0.0;
    tableWidgetInsertData(m_tableWidgetRows++, "采集时间"   , getGBdate(data), "-");

    tmpValue = data[6] << 8 | data[7];
    tableWidgetInsertData(m_tableWidgetRows++, "登出流水号", tmpValue, tmpValue,"-");

    tmpValue = data[8] & 1;
    tableWidgetInsertData(m_tableWidgetRows++, "定位信息-状态位", data[8], tmpValue==0 ? "有效定位" : "无效定位", "-");
    tmpValue = data[9] << 8 | data[10];
    tmpValue = tmpValue << 8 | data[11];
    tmpValue = tmpValue << 8 | data[12];
    tmpDoubleValue = tmpValue * 0.000001;
    tableWidgetInsertData(m_tableWidgetRows++, "经度", tmpValue, QString::number(tmpDoubleValue, 'f', 6), "精度：0.000001");
    tmpValue = data[13] << 8 | data[14];
    tmpValue = tmpValue << 8 | data[15];
    tmpValue = tmpValue << 8 | data[16];
    tmpDoubleValue = tmpValue * 0.000001;
    tableWidgetInsertData(m_tableWidgetRows++, "纬度", tmpValue, QString::number(tmpDoubleValue, 'f', 6), "精度：0.000001");

    if(data[17] == 1)
    {
        tableWidgetInsertData(m_tableWidgetRows++, "信息类型标志", data[17], "排放控制诊断信息", "-");
        showRealtimeECDDataUnit(&data[18]);
    }
    else if(data[17] == 2)
    {
        tableWidgetInsertData(m_tableWidgetRows++, "信息类型标志", data[17], "数据流信息", "-");
        showRealtimeFlowDataUnit(&data[18]);
    }
    else
    {
        tableWidgetInsertData(m_tableWidgetRows++, "信息类型标志", data[17], "不支持的信息类型，不继续解析", "-");
        return;
    }
}

/**
 * @brief GBdataParse::showHeaderDeviceFormat 处理终端标准协议格式头部
 * @param data
 */
void GBdataParse::showHeaderDeviceFormat(unsigned char *data)
{
    int tmpValue = 0;
    memcpy(m_vin, &data[3], sizeof(m_vin)-1);
    m_vin[sizeof(m_vin)-1] = 0;

    tableWidgetInsertData(m_tableWidgetRows++, "命令单元"   , data[2] , getCmdUnitType(data[2]), "-");
    tableWidgetInsertData(m_tableWidgetRows++, "机械环保代码", m_vin   , "-");
    tableWidgetInsertData(m_tableWidgetRows++, "终端软件版本", data[20], data[20],"-");
    tableWidgetInsertData(m_tableWidgetRows++, "数据加密方式", data[21], getEncryptType(data[21]),"-");
    tmpValue = data[22] << 8 | data[23];
    tableWidgetInsertData(m_tableWidgetRows++, "数据单元长度", tmpValue, tmpValue, "单位：Bytes");
}

void GBdataParse::on_pushBtn_dataParse_clicked()
{
    m_tableWidget_row = 0;
    fei4_data_handler fei4DataHandler;
    QString dataStr;
    unsigned char* data = NULL;
    unsigned int dataOutLen;

    dataStr = ui->textEdit->toPlainText();/**>can报文数据 */
    if(dataStr.isEmpty())
    {
        qDebug() << "dataStr is empty" ;
        return;
    }
    std::string str = dataStr.toStdString();
    qDebug() << "comboBox current text:" << ui->comboBox_dataType->currentText();
    qDebug() << "parse data:" << dataStr;

    fei4DataHandler.trimAllApace(str);
    qDebug() << "parse data trimAllApace:" << QString::fromStdString(str);

    data = fei4DataHandler.strToBytes(str,&dataOutLen);

    if(fei4DataHandler.dataCheck(data, dataOutLen))
    {
        return;
    }

//    ui->tableWidget_showResult->clearContents();/**>情况所有内容，但不删除列表 */
    tableWidgetDelAllRow();
    m_tableWidgetRows = 0;
    if(ui->comboBox_dataType->currentText()==FEI4_ITEM_DEVICE)
    {
        showHeaderDeviceFormat(data);
    }
    else if(ui->comboBox_dataType->currentText()==FEI4_ITEM_PLATFORM)
    {
        qDebug() << "暂时还没实现";
    }

    int cmdType = data[2];
    unsigned char *pDataUnit = &data[24];
    switch (cmdType) {
    case CMD_TYPE_DEVICE_LOGIN:
        showLoginDataUnit(pDataUnit);
        break;
    case CMD_TYPE_DEVICE_REALTIME_DATA:
        showRealtimeHeaderDataUnit(pDataUnit);
        break;
    case CMD_TYPE_DEVICE_RESEND:

        break;
    case CMD_TYPE_DEVICE_LOGOUT:
        showLogoutDataUnit(pDataUnit);
        break;
    case CMD_TYPE_DEVICE_TAKE_DOWN_WARN:
        showTakeDownWarnDataUnit(pDataUnit);
        break;
    default:
        break;
    }

}
