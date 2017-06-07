#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
//设备上线处理
#define API_DEVICE_INFO "handlerDeverInfo"
//获取设备基本信息
#define API_DEVICE_BASE_INFO "handlerGetDeviceBaseInfo"
//键盘按下
#define API_DEVICE_KEY_DOWN "handlerKeyDown"
//没有找到方法
#define API_NOT_FIND "notFindFunction"
//没有找到设备
#define API_NOT_FIND_DEVICE "notFindDevice"
//打开或关闭相机
#define API_SET_CAMERA_POWER "setCameraPower"
//消息级别
enum MESSAGE_LEVEL{
    MSG_LEVEL_SYSTEM,
    MSG_LEVEL_DEVICE,
    MSG_LEVEL_ADMIN
};
#endif