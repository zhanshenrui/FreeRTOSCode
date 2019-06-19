#include "malloc.h"	    
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//内存管理 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/1/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////


//内存池(32字节对齐)
__align(32) u8 mem1base[MEM1_MAX_SIZE];													//内部SRAM内存池
__align(32) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));					//外部SRAM内存池
//内存管理 表               1280
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];		
//外部SRAM内存管理表     30720
u16 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000+MEM2_MAX_SIZE)));	//外部SRAM内存池MAP
//内存管理参数	   
//                                   1280               30720
const u32 memtblsize[SRAMBANK]={MEM1_ALLOC_TABLE_SIZE,MEM2_ALLOC_TABLE_SIZE};			//内存表大小
//                                      32      32
const u32 memblksize[SRAMBANK]={MEM1_BLOCK_SIZE,MEM2_BLOCK_SIZE};						//内存分块大小
//                              40k         960k
const u32 memsize[SRAMBANK]={MEM1_MAX_SIZE,MEM2_MAX_SIZE};								//内存总大小


//内存管理控制器
struct _m_mallco_dev mallco_dev=
{
	my_mem_init,				//内存初始化
	my_mem_perused,				//内存使用率
	mem1base,mem2base,			//内存池
	mem1mapbase,mem2mapbase,	//内存管理状态表
	0,0,  		 				//内存管理未就绪
};

//复制内存
//*des:目的地址
//*src:源地址
//n:需要复制的内存长度(字节为单位)
void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  
//设置内存
//*s:内存首地址
//c :要设置的值
//count:需要设置的内存大小(字节为单位)
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	   
//内存管理初始化  
//memx:所属内存块
void my_mem_init(u8 memx)  
{  
    mymemset(mallco_dev.memmap[memx], 0,memtblsize[memx]*2);//内存状态表数据清零  
	mymemset(mallco_dev.membase[memx], 0,memsize[memx]);	//内存池所有数据清零  
	mallco_dev.memrdy[memx]=1;								//内存管理初始化OK  
}  
//获取内存使用率
//memx:所属内存块
//返回值:使用率(0~100)
u8 my_mem_perused(u8 memx)  
{  
    u32 used=0;  
    u32 i;  
    for(i=0;i<memtblsize[memx];i++)  
    {  
        if(mallco_dev.memmap[memx][i])used++; 
    } 
    return (used*100)/(memtblsize[memx]);  
}  
//内存分配(内部调用)
//memx:所属内存块
//size:要分配的内存大小(字节)
//返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址 
/*
根据size/每块大小 得到要分配n个内存块
从内存管理表里查接着n个元素为0的位置，如从1280开始查接连10个为0的元素，假设memmap[0][1270]-memmap[0][1261]连续为0，则确定的
位置是1261，返回的地址则是1261*32=40352=0x9da0, 则分配的内存块为0x9da0-0xaa1f,同时将memmap[0][1270]-memmap[0][1261]
这10个元素的值填为10
分配内存是从顶到底的，如刚开始分配10块内存，则分配的是0x68100000-0x680FFEA3(0x68100000-32*10+1=0x680FFEA3),第二次分配则从
0x680FFEA2开始向下分配，直到分配完。释放时则相反，从下往上释放，则当指针处于0x680FFEA2时，则往下释放，即0x680FFEA2-0x68000000释放完
*/
u32 my_mem_malloc(u8 memx,u32 size)  
{  
	signed long offset=0;  
	u32 nmemb;	//需要的内存块数  
	u32 cmemb=0;//连续空内存块数
	u32 i;  
	if(!mallco_dev.memrdy[memx])
		mallco_dev.init(memx);//未初始化,先执行初始化 
	if(size==0)
		return 0XFFFFFFFF;//不需要分配
	nmemb=size/memblksize[memx];  	//获取需要分配的连续内存块数           确定要分配多少块内存
	if(size%memblksize[memx])
		nmemb++;  
	//相当于for(offset=1280-1;offset>=0;offset--)
	for(offset=memtblsize[memx]-1;offset>=0;offset--)//搜索整个内存控制区      是memtblsize大小，即内存表的大小是1280或30720，不是内存块大小，不是32
	{     
	    //其实就是 mem1mapbase 或mem2mapbase数组，即看第offset块内存的管理表是否为0，如果为0，说明第offset块内存的32字
	    //节是空的，未用，可以分配给别人
	    //相当于if(!mallco_dev.memmap[0][offset])
		if(!mallco_dev.memmap[memx][offset])
			cmemb++;//连续空内存块数增加
		else
			cmemb=0;								//连续内存块清零
		if(cmemb==nmemb)							//找到了连续nmemb个空内存块
		{
			for(i=0;i<nmemb;i++)  					//标注内存块非空 
			{  
				mallco_dev.memmap[memx][offset+i]=nmemb;  
			}  
			return (offset*memblksize[memx]);//返回偏移地址  
		}
	}  
	return 0XFFFFFFFF;//未找到符合分配条件的内存块  
}  
//释放内存(内部调用) 
//memx:所属内存块
//offset:内存地址偏移
//返回值:0,释放成功;1,释放失败;  
u8 my_mem_free(u8 memx,u32 offset)  
{  
    int i;  
    if(!mallco_dev.memrdy[memx])//未初始化,先执行初始化
	{
		mallco_dev.init(memx);    
        return 1;//未初始化  
    }  
    if(offset<memsize[memx])//偏移在内存池内. 
    {  
        int index=offset/memblksize[memx];			//偏移所在内存块号码  
        int nmemb=mallco_dev.memmap[memx][index];	//内存块数量
        for(i=0;i<nmemb;i++)  						//内存块清零
        {  
            mallco_dev.memmap[memx][index+i]=0;  
        }  
        return 0;  
    }else return 2;//偏移超区了.  
}  
//释放内存(外部调用) 
//memx:所属内存块
//ptr:内存首地址 
/*
 分配内存是从顶到底的，如刚开始分配10块内存，则分配的是0x68100000-0x680FFEA3(0x68100000-32*10+1=0x680FFEA3),第二次分配则从
0x680FFEA2开始向下分配，直到分配完。释放时则相反，从下往上释放，则当指针处于0x680FFEA2时，则往下释放，即0x680FFEA2-0x68000000释放完
*/
void myfree(u8 memx,void *ptr)  
{  
	u32 offset;   
	if(ptr==NULL)return;//地址为0.  
 	offset=(u32)ptr-(u32)mallco_dev.membase[memx];     
    my_mem_free(memx,offset);	//释放内存      
}  
//分配内存(外部调用)
//memx:所属内存块
//size:内存大小(字节)
//返回值:分配到的内存首地址.
/*
 分配内存是从顶到底的，如刚开始分配10块内存，则分配的是0x68100000-0x680FFEA3(0x68100000-32*10+1=0x680FFEA3),第二次分配则从
0x680FFEA2开始向下分配，直到分配完。释放时则相反，从下往上释放，则当指针处于0x680FFEA2时，则往下释放，即0x680FFEA2-0x68000000释放完
*/

void *mymalloc(u8 memx,u32 size)  
{  
    u32 offset;   
	offset=my_mem_malloc(memx,size);  	   	 	   
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((u32)mallco_dev.membase[memx]+offset);  
}  
//重新分配内存(外部调用)
//memx:所属内存块
//*ptr:旧内存首地址
//size:要分配的内存大小(字节)
//返回值:新分配到的内存首地址.
void *myrealloc(u8 memx,void *ptr,u32 size)  
{  
    u32 offset;    
    offset=my_mem_malloc(memx,size);   	
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  									   
	    mymemcpy((void*)((u32)mallco_dev.membase[memx]+offset),ptr,size);	//拷贝旧内存内容到新内存   
        myfree(memx,ptr);  											  		//释放旧内存
        return (void*)((u32)mallco_dev.membase[memx]+offset);  				//返回新内存首地址
    }  
}












