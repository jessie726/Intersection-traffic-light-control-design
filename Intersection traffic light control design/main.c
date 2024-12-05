
#include	<reg52.h>

typedef unsigned char uchar;
typedef unsigned int uint;

/*****�������λ**********************/
sbit	  EW_R=P2^2;
sbit	  EW_Y=P2^3;
sbit	  EW_G=P2^4;

sbit	  SN_R=P2^5;
sbit	  SN_Y=P2^6;
sbit	  SN_G=P2^7;

sbit U1 = P2^0;     //����ܶ�ѡ
sbit U2 = P2^1;     //�����λѡ


sbit key_set = P3^2;
sbit key_dec = P3^1;
sbit key_inc = P3^0;

static bit blink_flg;

static uchar	  Time_EW;//�������򵹼�ʱ��Ԫ
static uchar	  Time_SN;//�ϱ����򵹼�ʱ��Ԫ

const char SEG_CODE[14] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xBF, 0xFF, 0x88, 0x83};

static uchar ns_red_time = 45;
static uchar ew_green_time = 42;

static uchar ew_red_time = 45;
static uchar ns_green_time = 42;

static uchar change_flag = 0;

static uchar time_cnt = 0;
static uchar display_buf[4] = {0};

// ���ʱ�� = �̵�ʱ�� + �Ƶ�ʱ�䣻
// һ��ѭ��ʱ�� = �������ʱ�� + �ϱ����ʱ�䣻


/**********************��ʱ�ӳ���************************/
void	Delay(uint	 a)
{
	uint	i;
	i=a;
	while(i--){;}
}
/*****************��ʾ�Ӻ���**************************/
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
/*****************led�Ӻ���**************************/
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

/**********************T0�жϷ������*******************/
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
      if(time_cnt < ew_green_time)//�����̵������ϱ������ ,
      {
        Time_EW = ew_green_time - time_cnt; //  Time_EW = 25
        Time_SN = ns_red_time - time_cnt;   //  Time_SN = 45
        led1_handle();
      }
      else if(time_cnt <= ns_red_time)//�����Ƶ������ϱ������
      {
        Time_EW = ns_red_time - time_cnt;
        Time_SN = ns_red_time - time_cnt;
        led2_handle();
      }
      else if(time_cnt <= (ns_red_time + ns_green_time))//������������ϱ��̵���
      {
        Time_EW = ns_red_time + ew_red_time - time_cnt + 1;
        Time_SN = ns_red_time + ns_green_time - time_cnt  + 1;
        led3_handle();
      }
      else if(time_cnt <= ns_red_time + ew_red_time + 1 )//������������ϱ��Ƶ���
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

/*********************��������**********************/
void keypros()
{	
  // ���ʱ�� = �̵�ʱ�� + �Ƶ�ʱ�䣻
  // һ��ѭ��ʱ�� = �̵�ʱ�� + �Ƶ�ʱ�� + ���ʱ�䣻
  if(key_dec==0)		  //��ⰴ��-
	{	
		Delay(1000);   //�������� һ���Լ10ms
		if(key_dec==0)	 //�ٴ��жϰ����Ƿ���
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
		while(!key_dec);	 //��ⰴ���Ƿ��ɿ�
	}

  if(key_inc==0)		  //��ⰴ��+
	{	
		Delay(1000);   //�������� һ���Լ10ms
		if(key_inc==0)	 //�ٴ��жϰ����Ƿ���
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
		while(!key_inc);	 //��ⰴ���Ƿ��ɿ�
	}
}

/*********************�ⲿ�жϳ�ʼ��**********************/
void Int0Init()
{
  //���� INT0
  IT0=1;//�����س�����ʽ���½��أ�
  EX0=1;//�� INT0 ���ж�����
  EA=1;//�����ж�
}

/*********************������ʼ**********************/
void	main(void)
{ 
  TMOD=0x01;//��ʱ�������ڷ�ʽ1
  TH0=(65536-50000)/256;//��ʱ������ֵ
  TL0=(65536-50000)%256;
  EA=1; //CPU���ж�������
  ET0=1;//����ʱ�ж�
  TR0=1;//������ʱ
  
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

/*********************�ⲿ�ж�**********************/
void Int0() interrupt 0 //�ⲿ�ж� 0 ���жϺ���
{
  Delay(1000); //��ʱ����
  if(key_set==0)
  {
    change_flag++;
    if(change_flag > 4)
      change_flag = 0;
    time_cnt = 0;
  }
}


