#include "ST7789.h"
#include "ST7789_init.h"
#include "lcdfont.h"
#include "delay.h"
#include "spi.h"
#include "dma.h"

//定义是否用dma传输画点指令（填充始终用dma）
//#define DrawPointWithDMA 
/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：	xsta,ysta   起始坐标
				xend,yend   终止坐标
				color       要填充的颜色
      返回值：  无
******************************************************************************/
void ST7789_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{    		
	u16 color1[1], t = 1;
	u32 num, num1;
	color1[0] = color;
	num = (xend - xsta)*(yend - ysta);
	if (num == 0)
	{
		return;
	}
	ST7789_Address_Set(xsta, ysta, xend - 1, yend - 1);//设置显示范围
	SPI_Cmd(SPI1, DISABLE);//关闭SPI
	SPI1->CR1 |= 1 << 11;//设置SPI16位传输模式
	SPI_Cmd(SPI1, ENABLE);//使能SPI
	//就算是切换SPI寄存器消耗的时间，比起16位字长传输带来的速度提升来说也不值一提
	ST7789_SELECT();//片选lcd
	while (t)
	{
		if (num > 65534)
		{
			num -= 65534;
			num1 = 65534;
		}
		else
		{
			t = 0;
			num1 = num;
		}
		DMA_16L_NoMemoryInc(DMA1_Channel3, (u32)&SPI1->DR, (u32)color1, num1);
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
		MYDMA_Enable(DMA1_Channel3);
		while (1)
		{
			if (DMA_GetFlagStatus(DMA1_FLAG_TC3) != RESET)//等待通道3传输完成
			{
				DMA_ClearFlag(DMA1_FLAG_TC3);//清除通道3传输完成标志
				break;
			}
		}
	}
	ST7789_UNSELECT();//片选lcd
	SPI_Cmd(SPI1, DISABLE);//关闭SPI
	//SPI1->CR1=~SPI1->CR1;
	//SPI1->CR1|=1<<11;
	//SPI1->CR1=~SPI1->CR1;//设置SPI8位传输模式
	SPI1->CR1 &= ~(1 << 11);//第11位置0设置SPI8位传输模式
	SPI_Cmd(SPI1, ENABLE);//使能SPI
}


/******************************************************************************
      函数说明：在指定区域填充有透明度颜色
      入口数据：	xsta,ysta   起始坐标
				xend,yend   终止坐标
				color       要填充的颜色
				trans		透明度
      返回值：  无
******************************************************************************/
/*
void ST7789_FillTrans(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color,u8 trans)
{   
	u16 color1[1], t = 1;
	u32 num, num1;
	color1[0] = color;
	num = (xend - xsta)*(yend - ysta);
	if (num == 0)
	{
		return;
	}
	
	u16 Y = 0;
	
	u32 curaddr = inputimage.StartADDR;
	
	u16 YSize = yend - ysta;
	u16 XSize = xend - xsta;
	
	u8 blocklines;//刷图每次处理的行数
	u8 mem_lines_hold = 3000 / XSize;	
	//内存分配9000个字节，每个像素3字节则是3000个像素
	//注意！读出时手册要求rgb666
	//注意！rgb666读出时为byte填充，有效数据仅为7-2位
	if(mem_lines_hold >= YSize)//内存可以放下所有区域
	{
		blocklines = YSize;
	}
	else
	{
		blocklines = mem_lines_hold;
	}
		

	u8 blocks = YSize / blocklines;//记录要刷的次数
	u8 endblocklines = YSize % blocklines;//记录最后一块行数（不满足一块的尺寸）
	u8 i;
	u8 LineData[XSize * 3 * blocklines + 1];//每次处理的数据都丢在里面
	
	for (i = 0;i < blocks;i++)//刷整块
	{
		W25QXX_FastRead(LineData, curaddr, inputimage.XSize * 2 * blocklines);
		curaddr += inputimage.XSize * 2 * blocklines;
		ST7789_ShowPicture(x, y + Y, inputimage.XSize, blocklines, LineData);
		
		Y += blocklines;
	}
	
	W25QXX_FastRead(LineData, curaddr, inputimage.XSize * 2 * endblocklines);
	curaddr += inputimage.XSize * 2 * endblocklines;
	if (Y < 240)
	{
		ST7789_ShowPicture(x, y + Y, inputimage.XSize, endblocklines, LineData);
	}
		
	
	

	ST7789_Address_Set(xsta, ysta, xend - 1, yend - 1);//设置显示范围
	SPI_Cmd(SPI1, DISABLE);//关闭SPI
	SPI1->CR1 |= 1 << 11;//设置SPI16位传输模式
	SPI_Cmd(SPI1, ENABLE);//使能SPI
	//就算是切换SPI寄存器消耗的时间，比起16位字长传输带来的速度提升来说也不值一提
	ST7789_SELECT();//片选lcd
	while (t)
	{
		if (num > 65534)
		{
			num -= 65534;
			num1 = 65534;
		}
		else
		{
			t = 0;
			num1 = num;
		}
		DMA_16L_NoMemoryInc(DMA1_Channel3, (u32)&SPI1->DR, (u32)color1, num1);
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
		MYDMA_Enable(DMA1_Channel3);
		while (1)
		{
			if (DMA_GetFlagStatus(DMA1_FLAG_TC3) != RESET)//等待通道3传输完成
			{
				DMA_ClearFlag(DMA1_FLAG_TC3);//清除通道3传输完成标志
				break;
			}
		}
	}
	ST7789_UNSELECT();//片选lcd
	SPI_Cmd(SPI1, DISABLE);//关闭SPI
	//SPI1->CR1=~SPI1->CR1;
	//SPI1->CR1|=1<<11;
	//SPI1->CR1=~SPI1->CR1;//设置SPI8位传输模式
	SPI1->CR1 &= ~(1 << 11);//第11位置0设置SPI8位传输模式
	SPI_Cmd(SPI1, ENABLE);//使能SPI
}

*/

/******************************************************************************
      函数说明：在指定区域读出颜色
      入口数据：	xsta,ysta   起始坐标
				xend,yend   终止坐标
				buf			要读出的地址指针
      返回值：  无
******************************************************************************/
void ST7789_Read(u16 xsta, u16 ysta, u16 xend, u16 yend, u8* buf)//指定区域读出颜色
{
	u16 color1[1], t = 1;
	u32 nums;
	nums = (xend - xsta)*(yend - ysta) * 3;
	if (nums == 0)
	{
		return;
	}
	ST7789_Address_Set(xsta, ysta, xend - 1, yend - 1);//设置显示范围
	ST7789_WR_REG(0x3A);//传输格式定义
	ST7789_WR_DATA8(0x66);//01100110 110代表18bit单像素
	//ST7789_WR_REG(0x2E);//发送读出指令
	//ST7789_WR_DATA8(0xff);//传输一个dummy byte
	//ST7789_WR_DATA8(0xff);//传输一个dummy byte
	//ST7789_WR_DATA8(0xff);//传输一个dummy byte
	ST7789_Read_Busl(buf, nums);
	ST7789_WR_REG(0x3A);//传输格式定义
	ST7789_WR_DATA8(0x05);//00000101 101代表16bit单像素
}


/******************************************************************************
      函数说明：在指定位置画点
      入口数据：x,y 画点坐标
                color 点的颜色
      返回值：  无
******************************************************************************/
void ST7789_DrawPoint(u16 x, u16 y, u16 color)
{
#ifdef DrawPointWithDMA
	ST7789_Fill(x, y, x + 1, y + 1, color);
#else
	ST7789_Address_Set(x, y, x, y);//设置光标位置 
	ST7789_WR_DATA(color);
#endif
}


void _usecacheupdate(u8 xS[30], u16 lastY, u16 color)
{
	u8 i, j;
	u16 startx = YCLEAR;
	u16 stopx = YCLEAR;
	u8 isoneing = 0;
	u8 isbreak = 0;
	for (i = 0; i < 30; i++)
	{
		if (isbreak)
		{
			break;
		}
		u8 temp = xS[i];
		for (j = 0; j < 8; j++)
		{
			//按bit找起始点
			if(isoneing == 0 && temp&(0x01 << j))
			{
				startx = i * 8 + j;
				isoneing = 1;
				continue;
			}
			//按bit找结束点
			if(isoneing == 1 && !(temp&(0x01 << j)))
			{
				stopx = i * 8 + j;
				isbreak = 1;
				break;
			}
		}
					
	}
	if (startx != YCLEAR && stopx != YCLEAR)
	{
		ST7789_Address_Set(startx, lastY, stopx - 1, lastY);//设置光标位置
		u16 times;
		for (times = 0; times < (stopx - startx); times++)
		{
			ST7789_WR_DATA(color);
		}
		u8 hhh;
	}
}

/******************************************************************************
      函数说明：在指定位置画点(带缓存)
      入口数据：x,y 画点坐标
               color 点的颜色
               flush 为1时强制刷新
      返回值：  无
******************************************************************************/
/*
 *flush发生时，判断是否有缓存过数据（xy缓存不为空），有则按照长缓存和缓存y（不使用缓存x）更新，
 *没有则进行处理
 *
 *y和缓存的y不一样（而且不为空）的时候要更新，更新后将更新发生时的点x记录下来，同时还需要更新缓存y值
 *x和缓存的最后一个x差距超过1的时候要更新，更新后将更新发生时的点x记录下来，此时不可以更新y值
 *y和缓存y一样而且x和最后一个x差距不超过1时要缓存点x，同时还需要更新缓存y值
 *
 **/
void ST7789_DrawPointWithCache(u16 x, u16 y, u16 color,u8 flush)
{
	
	// 缓存
	static u8 xS[30];
	// 上个点座标
	static u16 lastY = YCLEAR;
	static u16 lastX = YCLEAR;
	
	if (flush)
	{
		//flush发生
		if(lastY != YCLEAR && lastX != YCLEAR)
		{
			//xy缓存都不为空
			
			//使用xS和lastY更新屏幕
			_usecacheupdate(xS, lastY, color);
			
			memset(xS, 0, sizeof(xS));//抹掉缓冲区
			lastY = YCLEAR;
			lastX = YCLEAR;
			//flush更新完成了，数据恢复为空，之后传入任意数据都算开始传输状态
			return;
		}
		else
		{
			//记录数据为空，不可以更新
			return;
		}
	}
	else
	{
		//用户没有主动flush
		if(y != lastY && lastY != YCLEAR)
		{//y和缓存不一样，而且缓存不为空,			有数据的情况下遇到了换行
			
			//使用xS和lastY更新屏幕
			_usecacheupdate(xS, lastY, color);
			
			memset(xS, 0, sizeof(xS));//抹掉缓冲区
			
			//将当前点x记录在buffer
			u8 databytes = x / 8;
			u8 databits = x % 8;
			u8 temp = xS[databytes];
			temp |= (1 << (databits));
			xS[databytes] = temp;
			
			lastY = y;//更新缓存y
			lastX = x;//更新缓存y
			return;
		}
		if ((x - lastX) > 1 && lastX != YCLEAR)
		//y优先级高
		{//X和缓存差2（间断点），而且缓存不为空,	有数据的情况下遇到了跳点
			
			//使用xS和lastY更新屏幕
			_usecacheupdate(xS, lastY, color);
			
			memset(xS, 0, sizeof(xS));//抹掉缓冲区
			
			//将当前点x记录在buffer
			u8 databytes = x / 8;
			u8 databits = x % 8;
			u8 temp = xS[databytes];
			temp |= (1 << (databits));
			xS[databytes] = temp;
			
			lastX = x;//更新缓存x
			return;
		}
		if (y == lastY && x - lastX <= 1)
		{//										有数据的情况下xy变化符合记录规则
			//将当前点x记录在buffer
			u8 databytes = x / 8;
			u8 databits = x % 8;
			u8 temp = xS[databytes];
			temp |= (1 << (databits));
			xS[databytes] = temp;
			
			lastX = x;//更新缓存x
			lastY = y;//更新缓存y
			return;
		}
		
		if (lastY == YCLEAR && lastX == YCLEAR)
		{//										无数据的情况下只能是第一次记录
			memset(xS , 0 , sizeof(xS));//抹掉缓冲区
			
			//将当前点x记录在buffer
			u8 databytes = x / 8;
			u8 databits = x % 8;
			u8 temp = xS[databytes];
			temp |= (1 << (databits));
			xS[databytes] = temp;
			
			lastX = x;//更新缓存x
			lastY = y;//更新缓存y
			return;
		}
		
	}
}

 


/******************************************************************************
      函数说明：画线
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   线的颜色
      返回值：  无
******************************************************************************/
void ST7789_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	u16 t; 
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; //计算坐标增量 
	delta_y = y2 - y1;
	uRow = x1;//画线起点坐标
	uCol = y1;
	if (delta_x > 0)incx = 1; //设置单步方向 
	else if(delta_x == 0)incx = 0;//垂直线 
	else {incx = -1;delta_x = -delta_x;}
	if (delta_y > 0)incy = 1;
	else if (delta_y == 0)incy = 0;//水平线 
	else {incy = -1;delta_y = -delta_x;}
	if (delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴 
	else distance = delta_y;
	for (t = 0;t < distance + 1;t++)
	{
		ST7789_DrawPoint(uRow, uCol, color);//画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}


/******************************************************************************
      函数说明：画矩形
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   矩形的颜色
      返回值：  无
******************************************************************************/
void ST7789_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	ST7789_DrawLine(x1, y1, x2, y1, color);
	ST7789_DrawLine(x1, y1, x1, y2, color);
	ST7789_DrawLine(x1, y2, x2, y2, color);
	ST7789_DrawLine(x2, y1, x2, y2, color);
}


/******************************************************************************
      函数说明：画圆
      入口数据：x0,y0   圆心坐标
                r       半径
                color   圆的颜色
      返回值：  无
******************************************************************************/
void ST7789_Draw_Circle(u16 x0, u16 y0, u8 r, u16 color)
{
	int a, b;
	a = 0;b = r;	  
	while (a <= b)
	{
		ST7789_DrawPoint(x0 - b, y0 - a, color);             //3           
		ST7789_DrawPoint(x0 + b, y0 - a, color);             //0           
		ST7789_DrawPoint(x0 - a, y0 + b, color);             //1                
		ST7789_DrawPoint(x0 - a, y0 - b, color);             //2             
		ST7789_DrawPoint(x0 + b, y0 + a, color);             //4               
		ST7789_DrawPoint(x0 + a, y0 - b, color);             //5
		ST7789_DrawPoint(x0 + a, y0 + b, color);             //6 
		ST7789_DrawPoint(x0 - b, y0 + a, color);             //7
		a++;
		if ((a*a + b*b) > (r*r))//判断要画的点是否过远
		{
			b--;
		}
	}
}

/******************************************************************************
      函数说明：画实心圆
      入口数据：x0,y0   圆心坐标
                r       半径
                color   圆的颜色
      返回值：  无
******************************************************************************/

void ST7789_Draw_ALLCircle(u16 x0, u16 y0, u8 r, u16 color)
{
	int a, b;
	int di;
	a = 0;b = r;	  
	di = 3 - (r << 1);             //判断下个点位置的标志
	while(a <= b)
	{
		int i = a, p = b;
		while (i > 0) {		
			ST7789_DrawPoint(x0 + b, y0 - i, color);
			ST7789_DrawPoint(x0 - i, y0 + b, color);
			i--;
		}		
		while (p > 0) {		
			ST7789_DrawPoint(x0 - a, y0 - p, color);
			ST7789_DrawPoint(x0 - p, y0 - a, color);
			ST7789_DrawPoint(x0 + a, y0 - p, color);
			ST7789_DrawPoint(x0 - p, y0 + a, color);
			ST7789_DrawPoint(x0 + a, y0 + p, color);
			ST7789_DrawPoint(x0 + p, y0 + a, color);
			p--;
		}
		a++;
		//Bresenham算法画圆    
		if(di < 0)di += 4*a + 6;	  
		else
		{
			di += 10 + 4*(a - b);   
			b--;
		} 						    
	}
	ST7789_DrawPoint(x0, y0, color); //圆心坐标
}

///******************************************************************************
//      函数说明：显示汉字串
//      入口数据：x,y显示坐标
//                *s 要显示的汉字串
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号 可选 16 24 32
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void ST7789_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
//{
//	while (*s != 0)
//	{
//		if (sizey == 16) ST7789_ShowChinese16x16(x, y, s, fc, bc, sizey, mode);
//		else if (sizey == 24) ST7789_ShowChinese24x24(x, y, s, fc, bc, sizey, mode);
//		else if (sizey == 32) ST7789_ShowChinese32x32(x, y, s, fc, bc, sizey, mode);
//		else return;
//		s += 2;
//		x += sizey;
//	}
//}
//
///******************************************************************************
//      函数说明：显示单个16x16汉字
//      入口数据：x,y显示坐标
//                *s 要显示的汉字
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void ST7789_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
//{
//	u8 i, j;
//	u16 k;
//	u16 HZnum;//汉字数目
//	u16 TypefaceNum;//一个字符所占字节大小
//	u16 x0 = x;
//	TypefaceNum = sizey / 8*sizey;//此算法只适用于字宽等于字高，且字高是8的倍数的字，
//	//也建议用户使用这样大小的字,否则显示容易出问题！
//HZnum = sizeof(tfont16) / sizeof(typFNT_GB16);	//统计汉字数目
//for(k = 0 ; k < HZnum ; k++) 
//	{
//		if ((tfont16[k].Index[0] == *(s))&&(tfont16[k].Index[1] == *(s + 1)))
//		{ 	
//			ST7789_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
//			for (i = 0;i < TypefaceNum;i++)
//			{
//				for (j = 0;j < 8;j++)
//				{	
//					if (!mode)//非叠加方式
//					{
//						if (tfont16[k].Msk[i]&(0x01 << j))ST7789_WR_DATA(fc);
//						else ST7789_WR_DATA(bc);
//					}
//					else//叠加方式
//					{
//						if (tfont16[k].Msk[i]&(0x01 << j))ST7789_DrawPoint(x, y, fc);//画一个点
//						x++;
//						if ((x - x0) == sizey)
//						{
//							x = x0;
//							y++;
//							break;
//						}
//					}
//				}
//			}
//		}				  	
//		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
//	}
//} 
//
//
///******************************************************************************
//      函数说明：显示单个24x24汉字
//      入口数据：x,y显示坐标
//                *s 要显示的汉字
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void ST7789_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
//{
//	u8 i, j;
//	u16 k;
//	u16 HZnum;//汉字数目
//	u16 TypefaceNum;//一个字符所占字节大小
//	u16 x0 = x;
//	TypefaceNum = sizey / 8*sizey;//此算法只适用于字宽等于字高，且字高是8的倍数的字，
//	//也建议用户使用这样大小的字,否则显示容易出问题！
//HZnum = sizeof(tfont24) / sizeof(typFNT_GB24);	//统计汉字数目
//for(k = 0 ; k < HZnum ; k++) 
//	{
//		if ((tfont24[k].Index[0] == *(s))&&(tfont24[k].Index[1] == *(s + 1)))
//		{ 	
//			ST7789_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
//			for (i = 0;i < TypefaceNum;i++)
//			{
//				for (j = 0;j < 8;j++)
//				{	
//					if (!mode)//非叠加方式
//					{
//						if (tfont24[k].Msk[i]&(0x01 << j))ST7789_WR_DATA(fc);
//						else ST7789_WR_DATA(bc);
//					}
//					else//叠加方式
//					{
//						if (tfont24[k].Msk[i]&(0x01 << j))ST7789_DrawPoint(x, y, fc);//画一个点
//						x++;
//						if ((x - x0) == sizey)
//						{
//							x = x0;
//							y++;
//							break;
//						}
//					}
//				}
//			}
//		}				  	
//		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
//	}
//} 
//
///******************************************************************************
//      函数说明：显示单个32x32汉字
//      入口数据：x,y显示坐标
//                *s 要显示的汉字
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void ST7789_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
//{
//	u8 i, j;
//	u16 k;
//	u16 HZnum;//汉字数目
//	u16 TypefaceNum;//一个字符所占字节大小
//	u16 x0 = x;
//	TypefaceNum = sizey / 8*sizey;//此算法只适用于字宽等于字高，且字高是8的倍数的字，
//	//也建议用户使用这样大小的字,否则显示容易出问题！
//HZnum = sizeof(tfont32) / sizeof(typFNT_GB32);	//统计汉字数目
//for(k = 0 ; k < HZnum ; k++) 
//	{
//		if ((tfont32[k].Index[0] == *(s))&&(tfont32[k].Index[1] == *(s + 1)))
//		{ 	
//			ST7789_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
//			for (i = 0;i < TypefaceNum;i++)
//			{
//				for (j = 0;j < 8;j++)
//				{	
//					if (!mode)//非叠加方式
//					{
//						if (tfont32[k].Msk[i]&(0x01 << j))ST7789_WR_DATA(fc);
//						else ST7789_WR_DATA(bc);
//					}
//					else//叠加方式
//					{
//						if (tfont32[k].Msk[i]&(0x01 << j))ST7789_DrawPoint(x, y, fc);//画一个点
//						x++;
//						if ((x - x0) == sizey)
//						{
//							x = x0;
//							y++;
//							break;
//						}
//					}
//				}
//			}
//		}				  	
//		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
//	}
//}


/******************************************************************************
      函数说明：显示单个字符
      入口数据：x,y显示坐标
                num 要显示的字符
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void ST7789_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 temp, sizex, t;
	u16 i, TypefaceNum;//一个字符所占字节大小
	u16 x0 = x;
	sizex = sizey / 2;
	
	if (sizey == 16)		TypefaceNum = ICOPYX_FONTLIBS[0].DataSize;
	else if(sizey == 32)	TypefaceNum = ICOPYX_FONTLIBS[3].DataSize;
	else if(sizey == 24)	TypefaceNum = ICOPYX_FONTLIBS[2].DataSize;
	else if(sizey == 20)	TypefaceNum = ICOPYX_FONTLIBS[1].DataSize;

	
	num = num - ' ';    //得到偏移后的值
	ST7789_Address_Set(x, y, x + sizex - 1, y + sizey - 1);  //设置光标位置 
	u8 sendbytes = 0;
	u8 charbytes = sizex / 8;
	u8 charbits = sizex % 8;
	u8 datatemp[TypefaceNum];//存整个数据
	//读出整个字的数据
	if(sizey == 16)			ICPX_Select_Font_Data_all(datatemp, num, TypefaceNum, ICOPYX_FONTLIBS[0]);
	//调用8x16字体
	else if(sizey == 20)	ICPX_Select_Font_Data_all(datatemp, num, TypefaceNum, ICOPYX_FONTLIBS[1]);
	//调用16x32字体
	else if(sizey == 24)	ICPX_Select_Font_Data_all(datatemp, num, TypefaceNum, ICOPYX_FONTLIBS[2]);
	//调用12x24字体
	else if(sizey == 32)	ICPX_Select_Font_Data_all(datatemp, num, TypefaceNum, ICOPYX_FONTLIBS[3]);
	//调用10x20字体
	else return;
	
	for (i = 0; i < TypefaceNum; i++)
	{ 
		temp = datatemp[i];
		//开始传输字符信息，单byte
		if(!mode)//非叠加模式
		{
			if (charbits > 0)
			{
				//需要非整byte传输
				if(sendbytes == charbytes)
				{
					//所有byte传输完毕，单独传输bit
					for(t = 0 ; t < charbits ; t++)
					{
						if (temp&(0x01 << t))
						{
							ST7789_WR_DATA(fc);
						}
						else 
						{
							ST7789_WR_DATA(bc);
						}
					}
					sendbytes = 0;
				}
				else
				{
					//传byte
					for(t = 0 ; t < 8 ; t++)
					{
						if (temp&(0x01 << t))
						{
							ST7789_WR_DATA(fc);
						}
						else 
						{
							ST7789_WR_DATA(bc);
						}
					}
					sendbytes++;
				}
			
			
			}
			else
			{
				//不需要非整byte传输
				for(t = 0 ; t < 8 ; t++)
				{
					if (temp&(0x01 << t))
					{
						ST7789_WR_DATA(fc);
					}
					else 
					{
						ST7789_WR_DATA(bc);
					}
				}
			}
		}
		else
		{
			for (t = 0;t < 8;t++)
			{
				if (temp&(0x01 << t))
				{
					ST7789_DrawPoint(x, y, fc);//画一个点
					//ST7789_DrawPointWithCache(x, y, fc, 0);//画一个点
				}
				x++;
				if ((x - x0) == sizex)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}   	 	  
}

/******************************************************************************
      函数说明：显示单个符号
      入口数据：x,y显示坐标
                num 要显示的符号id
                fc 符号的颜色
                bc 符号的背景色
                sizey 符号y值
                sizex 符号x值
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void ST7789_ShowSimbol(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizex, u8 sizey, u8 mode)
{
	u8 temp, t;
	u16 i, TypefaceNum;//一个字符所占字节大小
	u16 x0 = x;
	u16 numbyte = sizex / 8;//字体宽占用byte数
	u16 numbit = sizex % 8;//字体宽除了整byte还占用的bit数
	u16 numbytew = 0;		//已经写入的byte量
	u16 numbitw = 0;		//已经写入的（除了整byte）的bit量
	u8	byteover = 0;		//byte写入完毕，可以使用bit判断结束
	//sizex = sizey / 2;
	//TypefaceNum = sizex / 8*sizey;
	//num = num - ' ';    //得到偏移后的值
	if(sizey == 20) {
		TypefaceNum = ICOPYX_FONTLIBS[4].DataSize;
	}
	ST7789_Address_Set(x, y, x + sizex - 1, y + sizey - 1);  //设置光标位置 
	for(i = 0 ; i < TypefaceNum ; i++)
	//遍历TypefaceNum个字节
	{ 
		if (numbytew == numbyte)	//已经完成整byte的传输
			{
				byteover = 1;
				numbytew = 0;
			}
		else						//整byte的传输未完成
			{
				byteover = 0;
			}
		if (sizey == 20) {
			temp = ICPX_Select_Font_Data(num, i, ICOPYX_FONTLIBS[4]);
			//simbol_1810[num][i];		 //调用20x20符号
		}
		//读出字库内对应字节的数据
		else return;
		for (t = 0;t < 8;t++)
			//遍历字节内8个bit
			{
				if (numbitw == numbit)//已经写入的bit数量达到目标空闲数量
				{
					break;
				}
				
				if (!mode)//非叠加模式
				{
					if (temp&(0x01 << t))
					{
						ST7789_WR_DATA(fc);
					}
					else 
					{
						ST7789_WR_DATA(bc);
					}
				}
				else//叠加模式
				{
					if (temp&(0x01 << t))
					{
						ST7789_DrawPoint(x, y, fc);//画一个点
					}
					x++;
					if ((x - x0) == sizex)
					{
						x = x0;
						y++;
						break;
					}
				}
				if (byteover)
				{
					numbitw += 1;//最后一字节才开始记录bit数量
				}	
			}
		if (!byteover)	//没结束byte传输时
			{
				numbytew++;//已完成的byte自增
			}
		else			//结束byte传输时（最后bit传输结束）
			{
				numbitw = 0;
			}
		
	}   	 	  
}

/******************************************************************************
      函数说明：显示字符串
      入口数据：x,y显示坐标
                *p 要显示的字符串
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void ST7789_ShowString(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode)
{         
	while (*p != '\0')
	{       
		ST7789_ShowChar(x, y, *p, fc, bc, sizey, mode);
		x += sizey / 2;
		p++;
	} 
	ST7789_DrawPointWithCache(0, 0, fc, 1);//强制刷新
}


/******************************************************************************
      函数说明：显示数字
      入口数据：m底数，n指数
      返回值：  无
******************************************************************************/
u32 mypow(u8 m, u8 n)
{
	u32 result = 1;	 
	while (n--)result *= m;
	return result;
}


/******************************************************************************
      函数说明：显示整数变量
      入口数据：x,y显示坐标
                num 要显示整数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void ST7789_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey)
{         	
	u8 t, temp;
	u8 enshow = 0;
	u8 sizex = sizey / 2;
	for (t = 0;t < len;t++)
	{
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0&&t < (len - 1))
		{
			if (temp == 0)
			{
				ST7789_ShowChar(x + t*sizex, y, ' ', fc, bc, sizey, 0);
				continue;
			}
			else enshow = 1; 
		 	 
		}
		ST7789_ShowChar(x + t*sizex, y, temp + 48, fc, bc, sizey, 0);
	}
} 


/******************************************************************************
      函数说明：显示两位小数变量
      入口数据：x,y显示坐标
                num 要显示小数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void ST7789_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey)
{         	
	u8 t, temp, sizex;
	u16 num1;
	sizex = sizey / 2;
	num1 = num * 100;
	for (t = 0;t < len;t++)
	{
		temp = (num1 / mypow(10, len - t - 1)) % 10;
		if (t == (len - 2))
		{
			ST7789_ShowChar(x + (len - 2)*sizex, y, '.', fc, bc, sizey, 0);
			t++;
			len += 1;
		}
		ST7789_ShowChar(x + t*sizex, y, temp + 48, fc, bc, sizey, 0);
	}
}


/******************************************************************************
      函数说明：显示图片
      入口数据：x,y起点坐标
                length 图片长度
                width  图片宽度
                pic[]  图片数组    
      返回值：  无
******************************************************************************/
void ST7789_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[])
{
	u8 t = 1;
	u32 num = length*width * 2, num1;
	ST7789_Address_Set(x, y, x + length - 1, y + width - 1);
	ST7789_SELECT();//片选lcd
	while (t)
	{
		if (num > 65534)
		{
			num -= 65534;
			num1 = 65534;
		}
		else
		{
			t = 0;
			num1 = num;
		}
		DMA_8L_MemoryInc(DMA1_Channel3, (u32)&SPI1->DR, (u32)pic, num1);
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
		MYDMA_Enable(DMA1_Channel3);
		while (1)
		{
			if (DMA_GetFlagStatus(DMA1_FLAG_TC3) != RESET)//等待通道3传输完成
			{
				DMA_ClearFlag(DMA1_FLAG_TC3);//清除通道3传输完成标志
				break; 
			}
		}
		pic += 65534;
	}
	ST7789_UNSELECT();//失能lcd
}

void ST7789_SetBaklight(u8 light)
{
	if (light > 100)light = 100;
	u16 temp = 900 - light * 9;
	TIM_SetCompare1(TIM1, temp);
}



