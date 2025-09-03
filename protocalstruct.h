#ifndef PROTOCALSTRUCT_H
#define PROTOCALSTRUCT_H

#include <QtGlobal>

// #pragma pack(push, 1)
struct ProtocolHeader {
    quint16 magic;      // 魔数 0x55AA    
    quint8 version;     // 协议版本
    quint8 type;        // 消息类型
    quint8 flags;       // 标志位
    quint8 reserved;    // 保留字段
    quint32 body_len;   // 消息体长度
    
    // ProtocolHeader() {
    //     magic = 0x55AA;
    //     version = 1;
    //     type = 0;
    //     flags = 0;
    //     reserved = 0;
    //     body_len = 0;
    // }
    
    // bool isValid() const {
    //     return magic == 0x55AA && version == 1;
    // }
};
// #pragma pack(pop)

#endif // PROTOCALSTRUCT_H
