#ifndef FEI4_DATA_HANDLER_H
#define FEI4_DATA_HANDLER_H

#include <iostream>

typedef enum
{
    CMD_TYPE_DEVICE_LOGIN          = 1,
    CMD_TYPE_DEVICE_REALTIME_DATA  = 2,
    CMD_TYPE_DEVICE_RESEND         = 3,
    CMD_TYPE_DEVICE_LOGOUT         = 4,
    CMD_TYPE_DEVICE_TAKE_DOWN_WARN = 5,

    CMD_TYPE_DEVICE_UNKNOW
}cmdType_e;

class fei4_data_handler
{
public:
    fei4_data_handler();
    void trimAllApace(std::string &str);
    unsigned char* strToBytes(const std::string& str,unsigned int *outLen);
    char getBcc(unsigned char *pData, int dataLen);
    int dataCheck(unsigned char *data, int dataLen);
};



#endif /**>FEI4_DATA_HANDLER_H */
