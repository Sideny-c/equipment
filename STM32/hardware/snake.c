#include "stm32f10x.h"   // Device header
#include "oled.h"
#include "Delay.h"
#include "key.h"
/*����̰���ߵ����ڵ�*/
static void OLED_DrawSnake(u8 x,u8 y,u8 c)
{
  u16 i;
  u8 x0=x;
  for(i=0;i<4*4;i++)
  {
   // OLED_DrawPoint(x0,y,c);
    x0++;
    if(x0-x==4)
    {
      x0=x;
      y++;
    }
  }    
}

//����ڵ���������
struct SNAKE
{
  u8 x;//�ڵ�x����
  u8 y;//�ڵ�y���� 
};
struct SNAKE snake_info[50];//�߽ڵ����50��
static u8 node_cnt=0;//������
/*��������ڵ����
struct SNAKE *buff --��������ڵ�ṹ��ָ��
cnt  --����ڵ����
c -- 1Ϊ���ƣ�0Ϊȡ������
*/
static void OLED_DrawSnakebody(struct SNAKE *buff,u8 cnt,u8 c)
{
    u8 i=0;
    for(i=0;i<cnt;i++)
    {
      OLED_DrawSnake(buff[i].x,buff[i].y,c);
    }
}

/**************�ж�ʵ��λ��***************
**
**�ж������ɵ�ʵ��x�����Ƿ�����������
**�β�:u8 x --ʵ����λ��
**      cnt  --����ڵ����
**����ֵ��1 --���������ϣ�0 -- û��
*******************************************/
static u8 OLED_GetSnakeFoodX(u8 x,u8 cnt)
{
  u8 i=0;
  for(i=0;i<cnt;i++)
  {
    if(x==snake_info[i].x)break;
  }
  if(i==cnt)return 0;
  else return 1;
}
/**************�ж�ʵ��λ��***************
**
**�ж������ɵ�ʵ��y�����Ƿ�����������
**�β�:u8 y --ʵ����λ��
**      cnt  --����ڵ����
**����ֵ��1 --���������ϣ�0 -- û��
*******************************************/
static u8 OLED_GetSnakeFoodY(u8 y,u8 cnt)
{
  u8 i=0;
  for(i=0;i<cnt;i++)
  {
    if(y==snake_info[i].y)break;
  }
  if(i==cnt)return 0;
  else return 1;
}

/*******************��̬�ƶ�����********************************/
static void Snake_Move(u8 x,u8 y)
{
  u8 i=0;
  OLED_DrawSnake(snake_info[0].x,snake_info[0].y,0);//�����β
  for(i=0;i<node_cnt-1;i++)//�ڵ���ǰƫ��
  {
    snake_info[i].x= snake_info[i+1].x;
    snake_info[i].y=snake_info[i+1].y;
  }
  /*��ͷ�ڵ�����*/
  snake_info[node_cnt-1].x=x;  
  snake_info[node_cnt-1].y=y;
  OLED_DrawSnake(snake_info[node_cnt-1].x,snake_info[node_cnt-1].y,1);//��ʾ��ͷ�ڵ�
}
/*******************���Ʒ���,�ж���Ϸ�Ƿ����********************/
u8 SnakeGameStart(void)
{
  u8 x=10;
  u8 y=30;
  u8 food_x,food_y;
  u8 stat=0;
  char buff[20];
 // OLED_DrawRectangle(0, 0, 127, 63);//���ƾ���
//  OLED_DrawRectangle(1, 1, 126, 62);//���ƾ���
  //��������1
  snake_info[node_cnt].x=x;
  snake_info[node_cnt].y=y;
  node_cnt++;
  //��������2
  snake_info[node_cnt].x=x+5;
  snake_info[node_cnt].y=y;  
  //������ͷ����
  x=snake_info[node_cnt].x;
  y=snake_info[node_cnt].y;
  node_cnt++;
  OLED_DrawSnakebody(snake_info,node_cnt,1);//��������
  //����ʵ������
  food_x=((rand()%115)/5)*5+5;
  food_y=((rand()%50)/5)*5+5;
  OLED_DrawSnake(food_x,food_y,1);//����ʵ��
 // OLED_Refresh();//�������ݵ���Ļ
  while(1)
  {
    if(snake_info[node_cnt-1].x>=125 || snake_info[node_cnt-1].y>=60)//�ж��Ƿ�ײǽ
    {
     // snprintf(buff,sizeof(buff),"Mark:%d",node_cnt-2);
    //  OLED_DisplayStr(36,8,8,16,buff);//�ַ�����ʾ
    //  OLED_DisplayStr(10,28,12,24,"Game over");//�ַ�����ʾ
   //   OLED_Refresh();//�������ݵ���Ļ
      Key_Num=0;
      node_cnt=0;//��սڵ�
      break;//��Ϸ����
    }
    if(snake_info[node_cnt-1].x==food_x && snake_info[node_cnt-1].y==food_y)//�ж��Ƿ�Ե�ʳ��
    {
      if(stat&1<<0 || stat&1<<1)//���ϻ�����ʱy+5
      {
        snake_info[node_cnt].x=food_x;
        snake_info[node_cnt].y=food_y+5; 
      }
      else //���������ʱx+5
      {
        snake_info[node_cnt].x=food_x+5;
        snake_info[node_cnt].y=food_y; 
      }
      node_cnt++;
      while(1)//��֤�������ɵ�ʵ�ﲻ��������
      {
        food_x=((rand()%115)/5)*5+5;
        if(OLED_GetSnakeFoodX(food_x,node_cnt)==0)break;
      }
      while(1)
      {
        food_y=((rand()%50)/5)*5+5;
        if(OLED_GetSnakeFoodY(food_y,node_cnt)==0)break;
      }
      OLED_DrawSnake(food_x,food_y,1);
  //    OLED_Refresh();//�������ݵ���Ļ 
    }
    switch(Key_Num=Key_getnum())
    {
      case 1://�� 
      stat&=~(0x3<<2);//������ұ�־ 
      if(stat&1<<1){}//����ʱ����ֱ������
      else
      {
        stat|=1<<0;//����
        y-=5;
        Snake_Move(x,y);
      }
      break;
      case 2://��
      stat&=~(0x3<<2);//������ұ�־ 
      if(stat&1<<0){}//������ʱ����ֱ������
      else
      {
        stat|=1<<1;//���±�־λ
        y+=5;
        Snake_Move(x,y); 
      }
      break;
      case 3://��
      stat&=~(0x3<<0);//������±�־  
      if(stat&1<<3){}//����ʱ����ֱ������
      else
      {
        stat|=1<<2;//����
        x-=5;
        Snake_Move(x,y);       
      }
      break; 
      case 4://�� 
      stat&=~(0x3<<0);//������±�־  
      if(stat&1<<2){}//����ʱ����ֱ������
      else
      {
        stat|=1<<3;
        x+=5;
        Snake_Move(x,y);      
      }
      break;      
    }
   // OLED_Refresh();//�������ݵ���Ļ
    Delay_ms(300);    
  }
  return 0;
}
