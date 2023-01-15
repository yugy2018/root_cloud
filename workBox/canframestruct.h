#ifndef CANFRAMESTRUCT_H
#define CANFRAMESTRUCT_H

#include <QtGlobal>
#include <QMetaType>

//class canFrameStruct
typedef struct
{
//public:
    quint32 can_id;  //帧id
    quint16 can_dlc; //帧数据长度
    quint16 rsv_ms;  //数据时间戳
    quint8  data[8]; //帧数据内容
    quint8  rsv[4];  //保留位
}canFrameStruct;

Q_DECLARE_METATYPE(canFrameStruct);//声明之后才能作为信号参数被传递

#endif // CANFRAMESTRUCT_H
