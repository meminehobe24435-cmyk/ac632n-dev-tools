# 项目目标

硬件名称：WT9011DCL-BT50
主控：AC6321A4
SDK：fw-AC63_BT_SDK
SDK路径：D:\23178\JL\sdk\fw-AC63_BT_SDK
目标应用：apps/spp_and_le
目标平台：bd19

## 蓝牙目标

经典蓝牙名称：WT9011DCL-BT50
BLE广播名称：WT9011DCL-BT50

需要打印：

1. 固件启动信息
2. SDK版本
3. 板级配置名称
4. 经典蓝牙名称
5. BLE广播名称
6. 蓝牙连接事件
7. 蓝牙断开事件
8. BLE连接参数
9. SPI测试结果
10. IIC测试结果
11. ADC测试结果

## SPI

板载器件：QMI8658A

CS：PB0
SCK：PA7
MOSI：PA8
MISO：PA6
INT：PA3

测试目标：

1. 初始化SPI
2. 读取芯片识别寄存器
3. 读取加速度和角速度原始数据
4. 转动板子时数据应发生变化

## IIC

板载器件：QMC5883

SCL：USB1DP
SDA：USB1DM
上拉：10K到3.3V

测试目标：

1. 扫描IIC地址
2. 读取器件识别或状态寄存器
3. 连续读取磁场原始数据
4. 转动板子时数据应发生变化

## ADC

ADC输入：PA1
信号名称：PWR_ADC
分压电阻：上方3M，下方1M

理论关系：

ADC输入电压约等于VBAT的1/4
VBAT约等于ADC计算电压的4倍

测试目标：

1. 读取ADC原始值
2. 多次采样并求平均
3. 输出ADC电压
4. 计算VBAT电压
5. 与万用表测量值比较

## Python串口工具

需要：

1. 自动列出COM口
2. 支持选择COM口
3. 默认波特率1000000
4. 支持修改波特率
5. 显示时间戳
6. 保存日志文件
7. 串口断开后自动重连
8. 支持关键字过滤
9. 提供start_logger.bat
10. 尽量生成Windows exe

## 安全要求

1. 保留现有SDK修改，不执行git reset --hard
2. 新建干净Git worktree开发
3. 编译和烧录必须分开
4. 编译时不能自动调用download.bat
5. 未经确认不得烧录
6. 不得假设PA0已经引出
7. 不得使用USB-TTL给板子供电
8. 不得把USB-TTL当成下载器