#include "lcd.h"
#include "lcdfont.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gbk.h"
#include "All_struct.h"

uint8_t lcd_finish_flag=0;
extern DMA_HandleTypeDef hdma_spi1_tx;
#define lcd_enable 0



uint8_t video_memory[134400]={0};   // 定义一块区域作为显存     131.25KBytes
Pixel_t video_buffer[280][240]={0};// 图像缓冲区
Pixel_t empty_pixel={0};            // 空像素
uint16_t column_index[240]={0};     //列索引
uint16_t row_index[280]={0};        //行索引


void LCD_Writ_Bus(uint8_t dat)
{
    LCD_CS_Clr();
    HAL_SPI_Transmit(&hspi1, &dat, 1, 1000);
    LCD_CS_Set();
}
void LCD_Writ_Bus_DMA(uint8_t *dat,uint16_t num)
{
    LCD_CS_Clr();
    HAL_SPI_Transmit_DMA(&hspi1, dat, num);
    lcd_finish_flag=1;
    while(__HAL_DMA_GET_COUNTER(&hdma_spi1_tx)!=0);  //detect the dma finish
    for(volatile uint8_t i=0;i<90;i++){}//delay,To wait lcd
    LCD_CS_Set();
}
void LCD_WR_DATA8(uint8_t dat)
{
    LCD_Writ_Bus(dat);
}
void LCD_WR_DATA(uint16_t dat)
{
    LCD_Writ_Bus(dat>>8);
    LCD_Writ_Bus(dat);
}
void LCD_WR_REG(uint8_t dat)
{
    LCD_DC_Clr();//写命令
    LCD_Writ_Bus(dat);
    LCD_DC_Set();//写数据
}
/**
************************************************************************
* @brief:      	LCD_Address_Set: 设置LCD显示区域的地址范围
* @param:      	x1, y1, x2, y2 - 显示区域的左上角和右下角坐标
* @retval:     	void
* @details:    	根据屏幕方向通过调用LCD_WR_REG和LCD_WR_DATA函数，设置LCD的列地址和行地址，然后写入储存器进行显示。
************************************************************************
**/
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    LCD_WR_REG(0x2a);//列地址设置
    LCD_WR_DATA(x1);
    LCD_WR_DATA(x2);
    LCD_WR_REG(0x2b);//行地址设置
    LCD_WR_DATA(y1+20);
    LCD_WR_DATA(y2+20);
    LCD_WR_REG(0x2c);//储存器写

}
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{

    uint8_t Big_endian=color>>8;
    uint8_t Little_endian=color;
#if Video_Memory_Enable==0
    uint32_t num=(xend-xsta)*(yend-ysta)*2;

    LCD_Address_Set(xsta,ysta,xend-1,yend-1);//设置显示范围
    for(uint32_t i=0;i<num-1;i+=2)
    {
        color_temp[i]=Big_endian;
        color_temp[i+1]=Little_endian;
    }
    if(num<65535)
    {
        LCD_Writ_Bus_DMA(color_temp,num);
    }else if(num>65535 && num<131070)
    {
        LCD_Writ_Bus_DMA(color_temp,65535);
        LCD_Writ_Bus_DMA(&color_temp[65535],num-65535);
    }else if(num>131070)
    {
        LCD_Writ_Bus_DMA(color_temp,65535);
        LCD_Writ_Bus_DMA(&color_temp[65535],65535);
        LCD_Writ_Bus_DMA(&color_temp[131070],num-131070);
    }
#else
    uint32_t start_address=((ysta - 1) * 240 + xsta)*2;
    for(uint32_t i=0;i<yend-ysta;i++)
    {
        for(uint32_t b=0;b<2*(xend-xsta);b+=2)
        {
            video_memory[i*240*2+start_address+b] = Big_endian;
            video_memory[i*240*2+start_address+b+1] = Little_endian;
        }
    }
#endif

}
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
#if Video_Memory_Enable==0
    LCD_Address_Set(x,y,x,y);//设置光标位置
    LCD_WR_DATA(color);
#else
    uint32_t start_address=((y-1)*240+x)*2;
    video_memory[start_address]=color>>8;
    video_memory[start_address+1]=color;
#endif
}
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{

    uint16_t t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //计算坐标增量
    delta_y=y2-y1;
    uRow=x1;//画线起点坐标
    uCol=y1;
    if(delta_x>0)incx=1; //设置单步方向
    else if (delta_x==0)incx=0;//垂直线
    else {incx=-1;delta_x=-delta_x;}
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;//水平线
    else {incy=-1;delta_y=-delta_y;}
    if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
    else distance=delta_y;
    for(t=0;t<distance+1;t++)
    {
        LCD_DrawPoint(uRow,uCol,color);//画点
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }

}
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color)
{
    LCD_DrawLine(x1,y1,x2,y1,color);
    LCD_DrawLine(x1,y1,x1,y2,color);
    LCD_DrawLine(x1,y2,x2,y2,color);
    LCD_DrawLine(x2,y1,x2,y2,color);
}
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color)
{
    int a,b;
    a=0;b=r;
    while(a<=b)
    {
        LCD_DrawPoint(x0-b,y0-a,color);             //3
        LCD_DrawPoint(x0+b,y0-a,color);             //0
        LCD_DrawPoint(x0-a,y0+b,color);             //1
        LCD_DrawPoint(x0-a,y0-b,color);             //2
        LCD_DrawPoint(x0+b,y0+a,color);             //4
        LCD_DrawPoint(x0+a,y0-b,color);             //5
        LCD_DrawPoint(x0+a,y0+b,color);             //6
        LCD_DrawPoint(x0-b,y0+a,color);             //7
        a++;
        if((a*a+b*b)>(r*r))//判断要画的点是否过远
        {
            b--;
        }
    }
}
/**
************************************************************************
* @brief:      	LCD_ShowChar: 在LCD上显示单个字符
* @param:      	x, y - 起始坐标，显示字符的左上角坐标
*              	num - 要显示的字符的ASCII码值
*              	fc - 字体颜色
*              	bc - 背景颜色
*              	sizey - 字体大小，支持12x6, 16x8, 24x12, 32x16
* @retval:     	void
* @details:    	在LCD上显示单个字符，可以选择显示模式和不同的字体大小。
************************************************************************
**/
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t fc,uint16_t bc,uint8_t sizey)
{
    uint8_t temp,sizex,t,m=0;
    uint16_t i,TypefaceNum;//一个字符所占字节大小
    uint16_t x0=x;
    sizex=sizey/2;
    TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
    num=num-' ';    //得到偏移后的值

    for(i=0;i<TypefaceNum;i++)
    {
        if(sizey==12)temp=ascii_1206[num][i];		       //调用6x12字体
        else if(sizey==16)temp=ascii_1608[num][i];		 //调用8x16字体
        else if(sizey==24)temp=ascii_2412[num][i];		 //调用12x24字体
        else if(sizey==32)temp=ascii_3216[num][i];		 //调用16x32字体
        else return;
        for(t=0;t<8;t++)
        {
            if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//画一个点
            x++;
            if((x-x0)==sizex)
            {
                x=x0;
                y++;
                break;
            }

        }
    }
}
/**
************************************************************************
* @brief:      	LCD_ShowString: 在LCD上显示字符串
* @param:      	x, y - 起始坐标，显示字符串的左上角坐标
*              	p - 要显示的字符串，以'\0'结尾
*              	fc - 字体颜色
*              	bc - 背景颜色
*              	sizey - 字体大小，支持12x6, 16x8, 24x12, 32x16
*              	mode - 显示模式，1表示反色，0表示正常显示
* @retval:     	void
* @details:    	在LCD上显示字符串，可以选择显示模式和不同的字体大小。
************************************************************************
**/
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t fc,uint16_t bc,uint8_t sizey,uint8_t mode)
{
    while(*p!='\0')
    {
        LCD_ShowChar(x,y,*p,fc,bc,sizey);
        x+=sizey/2;
        p++;
    }
}
/**
************************************************************************
* @brief:      	mypow: 计算m的n次方
* @param:      	m - 底数
*              	n - 指数
* @retval:     	uint32_t - m的n次方
* @details:    	计算m的n次方，返回结果。
************************************************************************
**/
uint32_t mypow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
    while(n--)result*=m;
    return result;
}
/**
************************************************************************
* @brief:      	LCD_ShowIntNum: 在LCD上显示整数数字
* @param:      	x - x坐标
*              	y - y坐标
*              	num - 要显示的整数
*              	len - 数字显示的位数
*              	fc - 字体颜色
*              	bc - 背景颜色
*              	sizey - 字体大小
* @retval:     	void
* @details:    	在LCD上显示整数数字，支持设置显示的位数、字体颜色、背景颜色和字体大小。
************************************************************************
**/
void LCD_ShowIntNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t fc,uint16_t bc,uint8_t sizey)
{
    uint8_t t,temp;
    uint8_t enshow=0;
    uint8_t sizex=sizey/2;

    for(t=0;t<len;t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey);
                continue;
            }else enshow=1;

        }
        LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey);
    }
}
/**
************************************************************************
* @brief:      	LCD_ShowFloatNum: 在LCD上显示格式化的浮点数，支持负数
* @param:      	x - x坐标
*              	y - y坐标
*              	num - 要显示的浮点数
*              	len - 整数位数
*              	decimal - 小数位数
*              	fc - 字的颜色
*              	bc - 背景颜色
*              	sizey - 字体大小
* @retval:     	void
* @details:    	在LCD上显示格式化的浮点数，支持设置整数位数、小数位数、字体颜色、背景颜色和字体大小。
************************************************************************
**/
void LCD_ShowFloatNum(uint16_t x, uint16_t y, float num, uint8_t len, uint8_t decimal, uint16_t fc, uint16_t bc, uint8_t sizey)
{
    int16_t num_int;
    uint8_t t, temp, sizex;
    sizex = sizey / 2;
    num_int = num * mypow(10, decimal);

    if (num < 0)
    {
        LCD_ShowChar(x, y, '-', fc, bc, sizey);
        num_int = -num_int;
        x += sizex;
        len++;
    }
    else
    {
        LCD_ShowChar(x, y, ' ', fc, bc, sizey);
        num_int = num_int;
        x += sizex;
        len++;
    }

    // 在更新数字时刷新显示的位置
    //LCD_Fill(x, y, x + len * sizex + decimal + 1, y + sizey + 1, bc);

    for (t = 0; t < len; t++)
    {
        if (t == (len - decimal))
        {
            LCD_ShowChar(x + (len - decimal) * sizex, y, '.', fc, bc, sizey);
            t++;
            len += 1;
        }
        temp = ((num_int / mypow(10, len - t - 1)) % 10) + '0';
        LCD_ShowChar(x + t * sizex, y, temp, fc, bc, sizey);
    }
}
/**

/**
************************************************************************
* @brief:      	LCD_ShowPicture: 在LCD上显示图片
* @param:      	x - x坐标
*               y - y坐标
 *               length - 长
 *               width - 宽
 *               pic[] - 图像所在数组或缓冲区首地址
 *               size - 图像所占存储空间(单位：字节)
* @retval:     	void
* @details:    	图像的长和宽必须与实际相符（不然图像会抽畜），大小限制为240 X 280像素
************************************************************************
**/
void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[],uint32_t size)
{
#if Video_Memory_Enable==0
    LCD_Address_Set(x,y,x+length-1,y+width-1);
    if(size<65535)
    {
        LCD_Writ_Bus_DMA((uint8_t *)pic,size);
    }else if(size>65535 && size<131070)
    {
        LCD_Writ_Bus_DMA((uint8_t *)pic,65535);
        LCD_Writ_Bus_DMA((uint8_t *)&pic[65535],size-65535);
    }else if(size>131070)
    {
        LCD_Writ_Bus_DMA((uint8_t *)pic,65535);
        LCD_Writ_Bus_DMA((uint8_t *)&pic[65535],65535);
        LCD_Writ_Bus_DMA((uint8_t *)&pic[131070],size-131070);
    }
#else
    uint32_t start_address=((y - 1) * 240 + x)*2;
    uint32_t a=0;
    for(uint32_t i=0;i<width;i++)
    {
        for(uint32_t b=0;b<2*(length);b+=2)//,a<size
        {
            video_memory[i*240*2+start_address+b] = pic[a];
            a++;
            video_memory[i*240*2+start_address+b+1] = pic[a];
            a++;
        }
    }
#endif

}
void video_memory_update(void)
{
    LCD_Writ_Bus_DMA(video_memory,65535);
    LCD_Writ_Bus_DMA(&video_memory[65535],65535);
    LCD_Writ_Bus_DMA(&video_memory[131070],3330);
}
/**
************************************************************************
* @brief:      	LCD_Init: LCD初始化函数
* @param:      	void
* @details:    	执行LCD的初始化过程，包括复位、背光控制、寄存器配置等
* @retval:     	void
************************************************************************
**/
void LCD_Init(void)
{
    LCD_RES_Clr();//复位
    HAL_Delay(100);
    LCD_RES_Set();
    HAL_Delay(100);

    LCD_BLK_Set();//打开背光
    HAL_Delay(100);

    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x11); //Sleep out
    HAL_Delay(120);              //Delay 120ms
    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x36);
    if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
    else LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x32); //Vcom=1.35V

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x15); //GVDD=4.8V  颜色深度

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20); //VDV, 0x20:0v

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F); //0x0F:60Hz

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);
    LCD_WR_REG(0x21);

    LCD_WR_REG(0x29);
}
/***************************************************************************************************/
void LCD_ShowChinese(uint16_t x,uint16_t y,const uint8_t *s,uint16_t fc,uint16_t bc,uint8_t sizey)
{

    while(*s!=0)
    {
        uint8_t *ss = NULL;
        uint8_t i,j;
        uint16_t k;
        uint16_t HZnum;
        uint16_t TypefaceNum;
        uint16_t x0=x,y0=y;
        TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
        HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);
        strcpy(ss, u2g(s));
        for(k=0;k<HZnum;k++)
        {
            if (tfont32[k].Index[0]==*(ss) && tfont32[k].Index[1]==*(ss+1))
            {
                for(i=0;i<TypefaceNum;i++)
                {
                    for(j=0;j<8;j++)
                    {
                        if(tfont32[k].Msk[i]&(0x01<<j))
                            LCD_DrawPoint(x,y,fc);
                        x++;
                        if((x-x0)==sizey)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
            continue;
        }
        s+=3;
        x+=sizey;
        y=y0;
    }
}
void hex_data_to_log_data(unsigned char *src, int len, char *dst)
{
    int i = 0;
    int index = 0;
    char buff[1024] = {0};

    for (i = 0; i < len; i++) {
        index += sprintf(buff + index, "%02x", src[i]);
        index += sprintf(buff + index, " ");
    }
    if (strlen(buff) > 0) strcpy(dst, buff);
}
int get_gbk_len(char *str) {
    char *p = str;
    int len = 0;
    if (p == NULL) return 0;

    while (*p != 0x00) {
        len++;
        p = p + 1;
    }

    return len;
}
Point_t Centralize_images(uint16_t length,uint16_t width)
{
    Point_t point;
    point.x=(LCD_W-length)/2;
    point.y=(LCD_H-width)/2;
    return point;
}
void LCD_Show_Reduced_Picture(const uint8_t *raw_pic,uint16_t length,uint16_t width,float x_k,float y_k)
{
    _constrain(x_k,0,1);
    _constrain(y_k,0,1);
    empty_video_buffer();
    uint8_t Big,Little;
    uint32_t p=0;
    for(uint16_t i=0;i<width;i++)
    {
        for (uint16_t a=0;a<length;a++)
        {
            video_buffer[i][a].Big_endian=raw_pic[p];
            p++;
            video_buffer[i][a].Little_endian=raw_pic[p];
            p++;
        }
    }
    p=0;
    uint16_t reduced_length=(uint16_t)roundf((float)length*x_k);
    uint16_t reduced_width=(uint16_t)roundf((float)width*y_k);
    float x_k_b=1.f/x_k;
    float y_k_b=1.f/y_k;
    for(uint16_t i=0;i<reduced_length;i++)
    {
        column_index[i]=(uint16_t)roundf((float)i*x_k_b);
    }
    for(uint16_t i=0;i<reduced_width;i++)
    {
        row_index[i]=(uint16_t)roundf((float)i*y_k_b);
    }
    Point_t point=Centralize_images(reduced_length,reduced_width);
    uint32_t start_address=((point.y - 1) * 240 + point.x)*2;
    for(uint32_t i=0;i<reduced_width;i++)
    {
        for (uint32_t a=0,b=0;a<reduced_length;a++,b+=2)
        {
            video_memory[i*240*2+start_address+b]=video_buffer[column_index[i]][row_index[a]].Big_endian;
            video_memory[i*240*2+start_address+b+1]=video_buffer[column_index[i]][row_index[a]].Little_endian;
        }
    }
}
void empty_video_buffer(void)//清空图像缓冲区
{
    for(uint16_t i=0;i<LCD_H;i++)
    {
        for (uint16_t a=0;a<LCD_W;a++)
        {
            video_buffer[i][a]=empty_pixel;
        }
    }
}



