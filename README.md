# SmartCar
Raspberry Pi Zero W + Arduino Pro Mini 智能小车
## 硬件清单
- [x] Raspberry Pi Zero W (1)
- [x] Arduino Pro Mini (1)
- [x] L293D Motor Control Shield (1)
- [x] SG90/TOWERPRO (2)
- [x] LM2596S DC-DC降压模块 (1)
- [x] 三角开关 (1)
- [x] 12V 3000mAh 18650 锂电池 (1)
- [x] Raspberry Pi Camera OV5674 500万像素 摄像头 (1)
- [x] 32G Micro储存SD卡 (1)
- [x] 四轮驱动智能小车底盘 (1)
## 设备接线
### L293D连接Arduino
```
数字端口11：直流电机#1/步进#1（PWM）
数字端口 3：直流电机#2/步进#1（PWM）
数字端口 5：直流电机#3/步进#2（PWM）
数字端口 6：直流电机#4/步进#2（PWM）
以及端口4，7，8和12。
数字端口 9：舵机控制#2 
数字端口 10：舵机控制#1
```
![L293D连接Arduino](http://i2.buimg.com/567571/363856fc4a22e5c9.png)
### Arduino链接Raspberry Pi
```
RX  -> TX
TX  -> RX
VCC -> VCC
GND -> GND
```
![RaspberryPi连接Arduino](https://oscarliang.com/ctt/uploads/2013/05/arduino-raspberry-pi-serial-connect-schematics.jpg)
## 服务器配置
### 域名配置
```
server{
    listen  80;
    server_name  car.ganktools.com;
    root "/root/SmartCar/Client/Html";
    index index.html index.htm;
    location ~ (/\w*/[^\.]*)$ {
        proxy_pass         http://127.0.0.1:5123;
        proxy_set_header   Host             $host;
        proxy_set_header   X-Real-IP        $remote_addr;
        proxy_set_header   X-Forwarded-For  $proxy_add_x_forwarded_for;
    }
    //视频流HLS配置
    location /live {
        types {
            application/vnd.apple.mpegurl m3u8;
            video/mp2t ts;
        }
        alias /root/video/hls;
        expires -1;
        add_header Cache-Control no-cache;
    }
}
```
### 视频流服务配置
>视频服务器需要nginx-rtmp-module模块支持
```
rtmp {  
    server {  
        listen 1935;
        chunk_size 4096;
        //HLS支持
        application live {  
            live on;
            hls on;
            #hls
            hls_path /root/video/hls;  
            hls_fragment 10s;
            #hls_playlist_length 3s;
            hls_continuous on;
            hls_cleanup on;
            hls_nested on;
            //支持视频录制
            recorder rec {
                record all manual;
                record_path /root/video;
                record_max_size 1280M;
                record_interval 86400s;
                record_suffix .flv; 
                record_unique on; 
            }
        }
        application video {
            play /root/video;
        }
    }  
}
```
## 程序编译
程序编译条件需要libevent,jsoncpp动态库支持
## 效果预览
![登录页面](https://github.com/chuanshuo843/SmartCar/blob/master/login.png)
![设备列表页面](https://github.com/chuanshuo843/SmartCar/blob/master/index.png)
![设备控制页面](https://github.com/chuanshuo843/SmartCar/blob/master/control.png)
