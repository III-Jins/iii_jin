#include "stdint.h"
#include "stdbool.h"

#include "HalUart.h"
#include "HalInterrupt.h"
#include "HalTimer.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "task.h"
#include "Kernel.h"

#define X_MAX 10
#define Y_MAX 10 //board max size
#define NOT_ENOUGH_TASK_NUM 0xFFFFFFFF

int ABS(int x) {
  return  (((x) < 0) ? -(x) : (x) );
}

typedef struct xy{
   uint32_t x;
   uint32_t y;
}xy;
xy xy1;
uint32_t x1 = 5;
uint32_t y1 = 5;
uint32_t x2 = 5;
uint32_t y2 = 5;

uint32_t q = 0;
uint32_t w = 0;

uint32_t c[X_MAX][Y_MAX];
uint32_t black[X_MAX][Y_MAX];
uint32_t white[X_MAX][Y_MAX];

signed int place_x1[X_MAX];
signed int place_y1[Y_MAX];
signed int place_x2[X_MAX];
signed int place_y2[Y_MAX];

static void Hw_init(void);
static void Kernel_init(void);

void User_task0(void);
//void User_task1(void);
//void User_task2(void);

void clear_board();
void show_board();
uint32_t input_dir(uint8_t, uint32_t, uint32_t);
void game_end();

void main(void)
{
   Hw_init();
   Kernel_init();

   while(true);
}

static void Hw_init(void)
{
   Hal_interrupt_init();
   Hal_uart_init();
   Hal_timer_init();
}


static void Kernel_init(void)
{
   uint32_t taskId;

   Kernel_task_init();
   Kernel_event_flag_init();
   Kernel_msgQ_init();


   taskId = Kernel_task_create(User_task0);
   if (NOT_ENOUGH_TASK_NUM == taskId)
   {
      putstr("Task0 creation fail\n");
   }
/*
   taskId = Kernel_task_create(User_task1);
   if (NOT_ENOUGH_TASK_NUM == taskId)
   {
      putstr("Task1 creation fail\n");
   }
   taskId = Kernel_task_create(User_task2);   
   if (NOT_ENOUGH_TASK_NUM == taskId)
   {
      putstr("Task2 creation fail\n");
   }
*/
   Kernel_start();
}



void User_task0(void) //초기화면
{
   uint8_t cmdlen=0;
   uint8_t cmd=0;

   uint8_t check = 2;
   uint32_t enter = 0;

   debug_printf("\n\nStart with this position !\n");
   show_board();

   debug_printf("Enter the 'w, a, s, d' to move \n");
   while(true) //메시징 기능
   {
      KernelEventFlag_t handle_event = Kernel_wait_events(KernelEventFlag_UartIn|KernelEventFlag_CmdOut);
      switch(handle_event)
      {
         case KernelEventFlag_UartIn:
            memclr(cmd, 16);
            Kernel_recv_msg(KernelMsgQ_Task0, cmd, 1);
            //debug_printf("\nRecv Cmd: %s\n", cmd); - null 뜸

            if(check==1){ //흑
               enter = input_dir(cmd, x1, y1);
               x1=xy1.x;
               y1=xy1.y;
               debug_printf("balck_turn x: %u, y: %u\n", x1, y1);
	       if(enter == 1){
                 black[y1][x1]=1;
               	 c[y1][x1] = 'o';
                 q++;
               	 check=2;
	       }
	       else{
		 if (!strcmp(c[y1][x1], 'o'))
                        c[y1][x1] = 'o';
                 else if(!strcmp(c[y1][x1], 'x'))
                        c[y1][x1] = 'x';
                 else
                        c[y1][x1] = 'm';
	       }
               debug_printf("%u\n",q);
               show_board();
               break;
            }
            else if(check==2){ //백
               enter = input_dir(cmd, x2, y2);
               x2=xy1.x;
               y2=xy1.y;
               if(enter == 1){
                 white[y2][x2]=2;
		 c[y2][x2] = 'x';
                 w++;
                 check = 1;
	       }
	       else{
		 if (!strcmp(c[y2][x2], 'o'))
             	 	c[y2][x2] = 'o';
         	 else if(!strcmp(c[y2][x2], 'x'))
            		c[y2][x2] = 'x';
		 else
                	c[y2][x2] = 'm';

	       }
	       debug_printf("white turn x: %u, y: %u\n", x2, y2);
               debug_printf("%u\n",w);

               show_board();
               break;
            }

      }
      game_end();
      Kernel_yield();
   }  
}

void game_end(){ //가로 세로 기준으로 3개만!
   //debug_printf("q: %u, w: %u\n", q, w);
   uint32_t check_win = 0;
   uint32_t winn[4] = {0};
   if(q>=3){ //흑
      for(uint32_t i=0; i<=9; i++){     
	 for(uint32_t j = 0; j < 10; j++){
		if (black[i][j] == 1){
			for (int t = 0; t < 3; t++) {
				if (i + t < 10 && 1 == black[i][j + t])
					winn[0]++;
				if (j + t < 10 && 1 == black[i + t][j])
					winn[1]++;
				if (i + t < 10 && j + t < 10 && 1 == black[i + t][j + t])
					winn[2]++;
				if (i + t < 10 && j - t > 0  && 1 ==  black[i + t][j - t])
                                        winn[3]++;
			}
			for (int k = 0; k < 4; k++){
				if (winn[k] == 3){
               				debug_printf("black win!");
               				clear_board();
					break;
				}
				else{
                                        winn[k] = 0;
                                }
			}
		}
	 }
      }
   }
   if(w>=3){ //백
      for(uint32_t i=0; i<=9; i++){
         for(uint32_t j = 0; j < 10; j++){
		if (white[i][j] == 2){
                        for (int t = 0; t < 3; t++) {
                                if (i + t < 10 && 2 == white[i][j + t])
                                        winn[0]++;
                                if (j + t < 10 && 2 == white[i + t][j])
                                        winn[1]++;
                                if (i + t < 10 && j + t < 10 && 2 ==  white[i + t][j + t])
                                        winn[2]++;
				if (i + t < 10 && j - t > 0  && 2 ==  white[i + t][j - t])
                                        winn[3]++;
                        }
                        for (int k = 0; k < 4; k++){
                                if (winn[k] == 3){
                                        debug_printf("white win!");
                                        clear_board();
                                        break;
                                }
				else{
					winn[k] = 0;
				}
                        }
                }
         }
      }
   }

}


uint32_t input_dir(uint8_t cmd,uint32_t x, uint32_t y){
   //debug_printf("%s\n",cmd);

   if(!strcmp(cmd,"s\0")){
      if( y < Y_MAX ){
         y = y + 1;
      }
      else{
         y = Y_MAX;
      }
      debug_printf("\nDOWN\n");
   }
   else if(!strcmp(cmd,"a\0")){
      if( x > 0 ){
         x = x - 1;
      }
      else{ 
         x = 0;
      }
      debug_printf("\nLEFT\n");
   }         
   else if(!strcmp(cmd,"w\0")){
      if( y > 0){
         y = y - 1;
      }   
      else{
         y = 0;
      }
      debug_printf("\nUP\n");
   }
   else if(!strcmp(cmd,"d\0")){
      if( x < X_MAX){
         x = x + 1;
      }
      else{
         x = X_MAX;
      }
      debug_printf("\nRIGHT\n");
   }
   else if(!strcmp(cmd,"o\0")){
     	debug_printf("\nENTER\n");
   	return 1;
   }
   else{
   	debug_printf("\n!!! not allowed input !!!\n");
   }
   xy1.x=x;
   xy1.y=y;
   return 0;
   //return xy1;
}

void clear_board()
{
   debug_printf("clear!\n");
   for (int i = 0 ; i <= Y_MAX ; i ++)
   {
      for (int j = 0; j <= X_MAX ; j ++)
      {
         c[i][j] = 0;
      }
   }
   while(true);
}

void show_board()
{
   for (int i = 0 ; i < Y_MAX ; i ++)
   {
      for (int j = 0 ; j < X_MAX ; j ++)
      {
         if (!strcmp(c[i][j], 'o'))
            debug_printf("○");
         else if(!strcmp(c[i][j], 'x'))
            debug_printf("●");
	 else if(!strcmp(c[i][j], 'm')){
            debug_printf("+");
            c[i][j] = '0';
         }
         else 
            debug_printf("□");
      }
      debug_printf("\n");
   }
}   
