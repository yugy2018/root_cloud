#include "fei4_data_handler.h"


#include <QDebug>

fei4_data_handler::fei4_data_handler()
{

}

/**
 * @brief fei4_data_handler::trimAllApace  清除字符串内所有空格
 * @param str 被处理的字符串
 */
void fei4_data_handler::trimAllApace(std::string &str)
{
    unsigned int index = 0;
    if(!str.empty())
    {
        while( (index = str.find(' ',index)) != std::string::npos)
        {
            str.erase(index, 1);
        }
    }
}

unsigned char* fei4_data_handler::strToBytes(const std::string& str,unsigned int *outLen)
{
    if(str.empty())
    {
        *outLen = 0;
        return NULL;
    }
    int bytelen = str.length() / 2;
    unsigned char *bytes = (unsigned char *)malloc(bytelen);
    std::string strByte;
    unsigned char n;
    for (int i = 0; i < bytelen; i++)
    {
        strByte = str.substr(i * 2, 2);
        sscanf(strByte.c_str(),"%02x",(unsigned int *)&n);
        bytes[i] = n;
    }
    *outLen = bytelen;

    return bytes;
}


char fei4_data_handler::getBcc(unsigned char *pData, int dataLen)
{
    char _bbc = pData[0];
    int i = 0;

    for(i = 1; i < dataLen; i++)
    {
        _bbc ^= pData[i];
    }
    return _bbc;
}


int fei4_data_handler::dataCheck(unsigned char *data, int dataLen)
{
    if((data[0] != 0x23) || (data[1] != 0x23))
    {
        qDebug() << "数据头部两字节错误，正确应该是0x23 0x23";
        return -1;
    }

    int cmdType = data[2];
    if((cmdType <CMD_TYPE_DEVICE_LOGIN) || (cmdType > CMD_TYPE_DEVICE_TAKE_DOWN_WARN))
    {
        qDebug() << "不认识的命令单元：" << data[2];
        return -1;
    }

    unsigned char bccCode = getBcc(&data[2] , dataLen - 3);
    if(data[dataLen - 1] != bccCode)
    {
        qDebug() << "Bcc校验错误，源数据校验码：" << QString::number(data[dataLen - 1],16)<< " ,程序计算校验码：" << QString::number(bccCode,16);
        return -1;
    }

    qDebug() << "数据检查通过！！";

    return 0;
}



