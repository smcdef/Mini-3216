# MINI 3216电子时钟

还记得5年前刚入门51单片机的时候的学习场景。当年单片机的学习之路是跟着杜洋的《爱上单片机》一书学习。那时候就很喜欢这本书里面的电子制作。杜老师有很多的电子DIY作品，唯一让我感兴趣的就是Mini3216。这是一个电子时钟。非常的有意思。刚学习单片机的时候就励志以后一定也做个一模一样的时钟。

但是当我真的开始琢磨时，发现事情并不是那么的简单。如此简单的电路是怎么驱动 32 x 16 点阵的呢？很是好奇，经过自己的琢磨终于想明白了原理。于是就开始了自己的DIY之旅。虽然很多年过去了，Mini 3216已经陪伴了我4个春秋。

而现在我依稀发现，网上还是没有人公开原理。虽然大学毕业后并没有从事单片机行业，但是依然保持对单片机的热爱。也由于自己工作接触的软件多为开源软件。因此，自己也有一颗开源的心。希望分享给更多的人。

关注这款时钟实现原理的朋友，好奇的问题一定是如何驱动 32 x 16 点阵。这部分代码实现位于hardware/fb_core.c文件。

## 效果展示

我个人是集齐了红、蓝、白、橙、绿等几种颜色。总得效果来说各个颜色都挺好看。各有特色。附上两张蓝色显示效果图。

<img src="Documentation/image/time.JPG" style="zoom:25%" />

<img src="Documentation/image/temperature.JPG" style="zoom:27%" />

硬件经过几次改版，现在的硬件具有上下左右4个micro USB口，方便供电。上下左右4个光敏电阻全方位检测环境光强。背面3个触摸按键。PCB top layout 和 bottom layout 展示如下图所示。

<img src="Documentation/image/top_layout.png" style="zoom:43%" />

<img src="Documentation/image/bottom_layout.png" style="zoom:45%" />

## 元器件清单

| 元器件                                                       | 数量 |
| :----------------------------------------------------------- | ---: |
| 47uF 0805电容                                                |    1 |
| 10nF 0805电容                                                |    1 |
| 100nF 0805电容                                               |    4 |
| CR1220                                                       |    1 |
| CR1220电池座                                                 |    1 |
| [平口micro USB](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.56db2e8d9M883Y&id=563928996432&_u=o1ltpbj55c9c) |    4 |
| [圆孔单排排母](https://item.taobao.com/item.htm?spm=a1z10.3-c.w4002-21261304145.10.1f211f35uFoswJ&id=564032495761) |    4 |
| [SW-520D倾角开关](https://detail.tmall.com/item.htm?id=580815564872&spm=a1z09.2.0.0.56db2e8d9M883Y&_u=o1ltpbj5179b) |    1 |
| 10k 欧姆0805电阻                                             |    2 |
| 100 欧姆0805电阻                                             |    1 |
| 22pF 0805电容                                                |    3 |
| LM75温度传感器                                               |    1 |
| [BS813A触摸芯片](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.56db2e8d9M883Y&id=36285536377&_u=o1ltpbj5139d) |    1 |
| PNP S8550三极管                                              |    1 |
| 光敏电阻5506                                                 |    4 |
| DS3231SN时钟芯片                                             |    1 |
| [KLJ-9032蜂鸣器](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.56db2e8d9M883Y&id=564040278829&_u=o1ltpbj5673d) |    1 |
| [32mm 共阳点阵（上下各8个引脚，引脚间距2.54mm）](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.56db2e8d9M883Y&id=573348473815&_u=o1ltpbj52711) |    8 |
| STC15F2K60S2单片机                                           |    1 |
| [1x4单排排母（下载接口使用）](https://item.taobao.com/item.htm?spm=a1z10.3-c.w4002-21261304145.9.51231f35z1KhNQ&id=563776620907) |    1 |

## 更新说明
1. PCB由BS814A更改为BS813A（代码配置CONFIG_BS813A）。
2. 增加DS3231中断引脚连接（代码配置CONFIG_DS3231_INT）。
3. 光敏电阻一端增加10k上拉电阻（代码配置CONFIG_PRES_PULL_UP）。

## 注意事项

1. 单片机使用的是STC15F2K60S2，使用内部晶振
2. 下载时，晶振频率请选择27MHz
3. 初次构建Keil工程后，请如下设置。
- 快捷键**Alt+F7**进入Options for Target ‘Target 1’设置界面。
- 点击**Device**选项框，勾选**Use Extended Linker(LX51) instead of BL51**。
- 点击**LX51 Misc**选项框，在Misc controls输入栏填写**REMOVEUNUSED**。
- 点击**OK**，设置完成。
