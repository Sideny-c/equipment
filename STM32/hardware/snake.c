#include "stm32f10x.h"   // Device header
#include "oled.h"
#include "Delay.h"
#include "key.h"
/*绘制贪吃蛇单个节点*/
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

//保存节点坐标结果体
struct SNAKE
{
  u8 x;//节点x坐标
  u8 y;//节点y坐标 
};
struct SNAKE snake_info[50];//蛇节点最多50个
static u8 node_cnt=0;//蛇身长度
/*绘制蛇身节点个数
struct SNAKE *buff --保存蛇身节点结构体指针
cnt  --蛇身节点个数
c -- 1为绘制，0为取消绘制
*/
static void OLED_DrawSnakebody(struct SNAKE *buff,u8 cnt,u8 c)
{
    u8 i=0;
    for(i=0;i<cnt;i++)
    {
      OLED_DrawSnake(buff[i].x,buff[i].y,c);
    }
}

/**************判断实物位置***************
**
**判断新生成的实物x坐标是否落在蛇身上
**形参:u8 x --实物光标位置
**      cnt  --蛇身节点个数
**返回值：1 --落在蛇身上，0 -- 没有
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
/**************判断实物位置***************
**
**判断新生成的实物y坐标是否落在蛇身上
**形参:u8 y --实物光标位置
**      cnt  --蛇身节点个数
**返回值：1 --落在蛇身上，0 -- 没有
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

/*******************动态移动蛇身********************************/
static void Snake_Move(u8 x,u8 y)
{
  u8 i=0;
  OLED_DrawSnake(snake_info[0].x,snake_info[0].y,0);//清除蛇尾
  for(i=0;i<node_cnt-1;i++)//节点往前偏移
  {
    snake_info[i].x= snake_info[i+1].x;
    snake_info[i].y=snake_info[i+1].y;
  }
  /*蛇头节点坐标*/
  snake_info[node_cnt-1].x=x;  
  snake_info[node_cnt-1].y=y;
  OLED_DrawSnake(snake_info[node_cnt-1].x,snake_info[node_cnt-1].y,1);//显示蛇头节点
}
/*******************控制方向,判断游戏是否结束********************/
u8 SnakeGameStart(void)
{
  u8 x=10;
  u8 y=30;
  u8 food_x,food_y;
  u8 stat=0;
  char buff[20];
 // OLED_DrawRectangle(0, 0, 127, 63);//绘制矩形
//  OLED_DrawRectangle(1, 1, 126, 62);//绘制矩形
  //创建蛇身1
  snake_info[node_cnt].x=x;
  snake_info[node_cnt].y=y;
  node_cnt++;
  //创建蛇身2
  snake_info[node_cnt].x=x+5;
  snake_info[node_cnt].y=y;  
  //保存蛇头坐标
  x=snake_info[node_cnt].x;
  y=snake_info[node_cnt].y;
  node_cnt++;
  OLED_DrawSnakebody(snake_info,node_cnt,1);//绘制蛇身
  //生成实物坐标
  food_x=((rand()%115)/5)*5+5;
  food_y=((rand()%50)/5)*5+5;
  OLED_DrawSnake(food_x,food_y,1);//绘制实物
 // OLED_Refresh();//更新数据到屏幕
  while(1)
  {
    if(snake_info[node_cnt-1].x>=125 || snake_info[node_cnt-1].y>=60)//判断是否撞墙
    {
     // snprintf(buff,sizeof(buff),"Mark:%d",node_cnt-2);
    //  OLED_DisplayStr(36,8,8,16,buff);//字符串显示
    //  OLED_DisplayStr(10,28,12,24,"Game over");//字符串显示
   //   OLED_Refresh();//更新数据到屏幕
      Key_Num=0;
      node_cnt=0;//清空节点
      break;//游戏结束
    }
    if(snake_info[node_cnt-1].x==food_x && snake_info[node_cnt-1].y==food_y)//判断是否吃到食物
    {
      if(stat&1<<0 || stat&1<<1)//向上或向下时y+5
      {
        snake_info[node_cnt].x=food_x;
        snake_info[node_cnt].y=food_y+5; 
      }
      else //向左或向右时x+5
      {
        snake_info[node_cnt].x=food_x+5;
        snake_info[node_cnt].y=food_y; 
      }
      node_cnt++;
      while(1)//保证重新生成的实物不再蛇身上
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
  //    OLED_Refresh();//更新数据到屏幕 
    }
    switch(Key_Num=Key_getnum())
    {
      case 1://上 
      stat&=~(0x3<<2);//清除左右标志 
      if(stat&1<<1){}//向下时不能直接向上
      else
      {
        stat|=1<<0;//向上
        y-=5;
        Snake_Move(x,y);
      }
      break;
      case 2://下
      stat&=~(0x3<<2);//清除左右标志 
      if(stat&1<<0){}//在向上时不能直接向下
      else
      {
        stat|=1<<1;//向下标志位
        y+=5;
        Snake_Move(x,y); 
      }
      break;
      case 3://左
      stat&=~(0x3<<0);//清除上下标志  
      if(stat&1<<3){}//向右时不能直接向左
      else
      {
        stat|=1<<2;//向左
        x-=5;
        Snake_Move(x,y);       
      }
      break; 
      case 4://右 
      stat&=~(0x3<<0);//清除上下标志  
      if(stat&1<<2){}//向左时不能直接向右
      else
      {
        stat|=1<<3;
        x+=5;
        Snake_Move(x,y);      
      }
      break;      
    }
   // OLED_Refresh();//更新数据到屏幕
    Delay_ms(300);    
  }
  return 0;
}
