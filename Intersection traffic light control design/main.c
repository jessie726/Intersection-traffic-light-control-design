
#include	<reg52.h>

typedef unsigned char uchar;
typedef unsigned int uint;

/*****定义控制位**********************/
sbit	  EW_R=P2^2;
sbit	  EW_Y=P2^3;
sbit	  EW_G=P2^4;

sbit	  SN_R=P2^5;
sbit	  SN_Y=P2^6;
sbit	  SN_G=P2^7;

sbit U1 = P2^0;     //数码管段选
sbit U2 = P2^1;     //数码管位选


sbit key_set = P3^2;
sbit key_dec = P3^1;
sbit key_inc = P3^0;

static bit blink_flg;

static uchar	  Time_EW;//东西方向倒计时单元
static uchar	  Time_SN;//南北方向倒计时单元

const char SEG_CODE[14] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xBF, 0xFF, 0x88, 0x83};

static uchar ns_red_time = 45;
static uchar ew_green_time = 42;

static uchar ew_red_time = 45;
static uchar ns_green_time = 42;

static uchar change_flag = 0;

static uchar time_cnt = 0;
static uchar display_buf[4] = {0};

// 红灯时间 = 绿灯时间 + 黄灯时间；
// 一次循环时间 = 东西红灯时间 + 南北红灯时间；


/**********************延时子程序************************/
void	Delay(uint	 a)
{
	uint	i;
	i=a;
	while(i--){;}
}
/*****************显示子函数**************************/
static void smg_display(void)
{
  static uchar pos = 0;
  U2=1;
  P0=0x00;
  U2=0;
    
  if(pos == 0)
  {
    P0 = SEG_CODE[Time_EW/10];
    U1=1;
    U1=0;
    
    if(change_flag == 1)
    {
      if(blink_flg)
      {
        P0 = 0xff;
        U1=1;
        U1=0;
      }
    }
    
    U2=1;
    P0=0x02;
    U2=0;
  }
  else if(pos == 1)
  {
    P0 = SEG_CODE[Time_EW%10];
    U1=1;
    U1=0;

    if(change_flag == 1)
    {
      if(blink_flg)
      {
        P0 = 0xff;
        U1=1;
        U1=0;
      }
    }
    
    U2=1;
    P0=0x01;
    U2=0;
  }
  else if(pos == 2)
  {
    P0 = SEG_CODE[Time_SN/10];
    U1=1;
    U1=0;

    if(change_flag == 2)
    {
      if(blink_flg)
      {
        P0 = 0xff;
        U1=1;
        U1=0;
      }
    }
    
    U2=1;
    P0=0x08;
    U2=0;
  }
  else if(pos == 3)
  {
    P0 = SEG_CODE[Time_SN%10];
    U1=1;
    U1=0;

    if(change_flag == 2)
    {
      if(blink_flg)
      {
        P0 = 0xff;
        U1=1;
        U1=0;
      }
    }
    
    U2=1;
    P0=0x04;
    U2=0;
  }

  pos++;
  if(pos == 4)
  {
    pos = 0;
  }
}
/*****************led子函数**************************/
static void led1_handle(void)
{
  EW_R = 0;
  EW_Y = 0;
  EW_G = 1;

  SN_R = 1;
  SN_Y = 0;
  SN_G = 0;
}
static void led2_handle(void)
{
  EW_R = 0;
  EW_Y = 1;
  EW_G = 0;

  SN_R = 1;
  SN_Y = 0;
  SN_G = 0;
}
static void led3_handle(void)
{
  EW_R = 1;
  EW_Y = 0;
  EW_G = 0;

  SN_R = 0;
  SN_Y = 0;
  SN_G = 1;
}
static void led4_handle(void)
{
  EW_R = 1;
  EW_Y = 0;
  EW_G = 0;

  SN_R = 0;
  SN_Y = 1;
  SN_G = 0;
}
static void led5_handle(void)
{
  EW_R = 0;
  EW_Y = 1;
  EW_G = 0;

  SN_R = 0;
  SN_Y = 1;
  SN_G = 0;
}
static void led6_handle(void)
{
  EW_R = 0;
  EW_Y = 0;
  EW_G = 0;

  SN_R = 0;
  SN_Y = 0;
  SN_G = 0;
}
static void led7_handle(void)
{
  EW_R = 1;
  EW_Y = 0;
  EW_G = 0;

  SN_R = 1;
  SN_Y = 0;
  SN_G = 0;
}

/**********************T0中断服务程序*******************/
void timer0(void)interrupt 1 using 1
{
	static uchar count;
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;

	count++;

  if(count == 10)
  {
    blink_flg = 1;
    if(change_flag == 3)
    {
      led6_handle();
    }
  }

	if(count == 20)
	{
    blink_flg = 0;
    if(change_flag == 0)
    {
      if(time_cnt < ew_green_time)//东西绿灯亮，南北红灯亮 ,
      {
        Time_EW = ew_green_time - time_cnt; //  Time_EW = 25
        Time_SN = ns_red_time - time_cnt;   //  Time_SN = 45
        led1_handle();
      }
      else if(time_cnt <= ns_red_time)//东西黄灯亮，南北红灯亮
      {
        Time_EW = ns_red_time - time_cnt;
        Time_SN = ns_red_time - time_cnt;
        led2_handle();
      }
      else if(time_cnt <= (ns_red_time + ns_green_time))//东西红灯亮，南北绿灯亮
      {
        Time_EW = ns_red_time + ew_red_time - time_cnt + 1;
        Time_SN = ns_red_time + ns_green_time - time_cnt  + 1;
        led3_handle();
      }
      else if(time_cnt <= ns_red_time + ew_red_time + 1 )//东西红灯亮，南北黄灯亮
      {
        Time_EW = ns_red_time + ew_red_time - time_cnt + 1;
        Time_SN = ns_red_time + ew_red_time - time_cnt  + 1;
        led4_handle();
      }
      
      time_cnt++;
      if(time_cnt > (ns_red_time + ew_red_time  + 1))
      {
        time_cnt = 0;
      }
    }
    else if(change_flag == 3)
    {
      led5_handle();
    }
    else if(change_flag == 4)
    {
      led7_handle();
    }
  	count=0;
	}
}

/*********************按键程序**********************/
void keypros()
{	
  // 红灯时间 = 绿灯时间 + 黄灯时间；
  // 一次循环时间 = 绿灯时间 + 黄灯时间 + 红灯时间；
  if(key_dec==0)		  //检测按键-
	{	
		Delay(1000);   //消除抖动 一般大约10ms
		if(key_dec==0)	 //再次判断按键是否按下
		{
      if(change_flag == 1)
      {
        if(Time_EW > 5)
        {
          Time_EW--;
          ew_green_time = Time_EW;
          ns_red_time = ew_green_time + 3;
          ew_red_time = ns_green_time + 3;
        }
      }
      else if(change_flag == 2)
      {
        if(Time_SN > 5)
        {
          Time_SN--;
          ns_green_time = Time_SN;
          ns_red_time = ew_green_time + 3;
          ew_red_time = ns_green_time + 3;
        }
      }
		}
		while(!key_dec);	 //检测按键是否松开
	}

  if(key_inc==0)		  //检测按键+
	{	
		Delay(1000);   //消除抖动 一般大约10ms
		if(key_inc==0)	 //再次判断按键是否按下
		{
      if(change_flag == 1)
      {
        if(Time_EW < 55)
        {
          Time_EW++;
          ew_green_time = Time_EW;
          ns_red_time = ew_green_time + 3;
          ew_red_time = ns_green_time + 3;
        }
      }
      else if(change_flag == 2)
      {
        if(Time_SN < 55)
        {
          Time_SN++;
          ns_green_time = Time_SN;
          ns_red_time = ew_green_time + 3;
          ew_red_time = ns_green_time + 3;
        }
      }
		}
		while(!key_inc);	 //检测按键是否松开
	}
}

/*********************外部中断初始化**********************/
void Int0Init()
{
  //设置 INT0
  IT0=1;//跳变沿出发方式（下降沿）
  EX0=1;//打开 INT0 的中断允许。
  EA=1;//打开总中断
}

/*********************主程序开始**********************/
void	main(void)
{ 
  TMOD=0x01;//定时器工作于方式1
  TH0=(65536-50000)/256;//定时器赋初值
  TL0=(65536-50000)%256;
  EA=1; //CPU开中断总允许
  ET0=1;//开定时中断
  TR0=1;//启动定时
  
  Int0Init();

  ns_red_time = ew_green_time + 3;
  ew_red_time = ns_green_time + 3;

  while(1)
  {
    smg_display();
    keypros();
    Delay(20);
  }
}

/*********************外部中断**********************/
void Int0() interrupt 0 //外部中断 0 的中断函数
{
  Delay(1000); //延时消抖
  if(key_set==0)
  {
    change_flag++;
    if(change_flag > 4)
      change_flag = 0;
    time_cnt = 0;
  }
}


