实验器材:
	战舰STM32F103开发板
	
实验目的:
	学习使用FreeRTOS事件标志组的相关API函数。
	
硬件资源:
	1,DS0(连接在PB5)，DS1(连接在PE5上)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面) 
	3,ALIENTEK 2.8/3.5/4.3/7寸LCD模块(仅支持MCU屏)
	4,按键KEY0(PE4)/KEY1(PE3)/KEY2(PE2)
	5,外部中断线4

实验现象:
	按下KEY0，KEY1和KEY2这个三个键的话事件标志组EventGroupHandler的三个
	事件位都置1。任务eventgroup_task会等待这三个事件位，当三个事件位都
	为1的时候就会执行相应的操作，本实验是刷新LCD指定区域背景颜色。
	
注意事项:
	无. 

参考资料：STM32F103 FreeRTOS开发手册.pdf 第十六章


-------------------------------------------------------------------------------------------

◆其他重要连接：
  开发板光盘资料下载地址（视频+文档+源码等）：http://www.openedv.com/posts/list/13912.htm


◆友情提示：如果您想以后及时免费的收到正点原子所有开发板资料更新增加通知，请关注微信公众平台：
 2种添加方法：（动动手提升您的学习效率，惊喜不断哦）
（1）打开微信->添加朋友->公众号->输入“正点原子”->点击关注
（2）打开微信->添加朋友->输入“alientek_stm32"->点击关注
 具体微信添加方法，请参考帖子：http://www.openedv.com/posts/list/45157.htm
 


						

						淘宝店铺： http://openedv.taobao.com
						           http://eboard.taobao.com
						公司网站：www.alientek.com
						技术论坛：www.openedv.com
                                                微信公众平台：正点原子
						电话：020-38271790
						传真：020-36773971
						广州市星翼电子科技有限公司
						正点原子@ALIENTEK
						     2016-11月