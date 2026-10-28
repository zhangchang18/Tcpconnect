#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define TTY_PATH            "/dev/tty"
#define STTY_US             "stty raw -echo -F "
#define STTY_DEF            "stty -raw echo -F "
int block[21][12] = { 0 };
int current_point;
int score=0;
int level = 1;
int speed=10;
static int get_char()		//非阻塞I/O, 如果没按下任何按键，程序会继续往下跑
{
    fd_set rfds;
    struct timeval tv;
    int ch = 0;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 10; //设置等待键盘的超时时间(以微秒为单位)
    
    //检测键盘是否有输入
    if (select(1, &rfds, NULL, NULL, &tv) > 0)	//以不等待(非阻塞IO)的方式扫描键盘
    {
        ch = getchar();	 
    }  
    return ch;
}
void gotoxy(int x, int y)
{
	printf("\033[%d;%dH", y + 1, x + 1);
}
struct point_pos	//每一个点都有x, y
{
	int x;
	int y;
};
struct point_pos point[4];
struct point_pos shapes[19][4]={
    // I
    {{0,0},{-1,0},{1,0},{2,0}},
    {{0,0},{0,-1},{0,1},{0,2}},
    // O
    {{0,0},{1,0},{0,1},{1,1}},

    // L
    {{0,0},{0,-1},{0,1},{0,2}},
    {{0,0},{1,0},{0,-1},{-1,-1}},
    {{0,0},{0,1},{0,-1},{1,1}},
    {{0,0},{-1,0},{1,0},{1,-1}},

    // J
    {{0,0},{0,-1},{0,1},{-1,1}},
    {{0,0},{1,0},{0,-1},{1,-1}},
    {{0,0},{1,0},{0,1},{0,2}},
    {{0,0},{-1,0},{-1,1},{1,0}},

    // S
    {{0,0},{1,0},{0,-1},{-1,-1}},
    {{0,0},{0,1},{1,0},{1,-1}},

    // Z
    {{0,0},{-1,0},{0,-1},{1,-1}},
    {{0,0},{0,-1},{-1,0},{-1,1}},

    // T
    {{0,0},{-1,0},{1,0},{0,-1}},
    {{0,0},{0,1},{0,-1},{1,0}},
    {{0,0},{-1,0},{1,0},{0,1}},
    {{0,0},{-1,0},{0,1},{0,-1}}
};		
	


void new_point()
{
    int i;
    current_point = rand() % 19;
    int base_x = 5; // 初始x坐标
    int base_y = 0; // 初始y坐标
    
    // 1. 临时存储坐标，先不写入全局 point，防止污染
    int temp_x[4], temp_y[4];
    
    for(i = 0; i < 4; i++)
    {
        temp_x[i] = base_x + shapes[current_point][i].x;
        temp_y[i] = base_y + shapes[current_point][i].y;
        
        // 2. 严格边界检查 & 碰撞检查
        // x范围：1~10, y范围：0~20 (20是底边框)
        // 如果越界或与已有方块(2)重叠，直接判负
        if( block[temp_y[i]][temp_x[i]] == 2)
        {
            system(STTY_DEF TTY_PATH);
            system("clear");
            gotoxy(10, 5);
            printf("\033[41;37m GAME OVER! Score:%d \033[0m\n", score);
            gotoxy(27, 0);
            exit(0);
        }
    }
    
    // 3. 全部合法，正式赋值给全局 point
    for(i = 0; i < 4; i++)
    {
        point[i].x = temp_x[i];
        point[i].y = temp_y[i];
    }
}
void draw_point()
{
	int i;
	for(i = 0; i < 4; i++)
    {
    	gotoxy(point[i].x * 2 , point[i].y);	//跳转到每一个点
    	printf("\033[1;33m☻\033[0m");	//打印方块
    }
}

void init_block()		//init初始化(初始化边框)
{
	int i;
	for(i = 0; i < 21; i++)
	{
		block[i][0] = 1;		//左边框
        	block[i][11] = 1;		//左边框
	}
	for(i = 0; i < 12; i++)
	{
		block[20][i] = 1;
		
	}
}
void display_all()		//显示所有静态数据
{
	int i, j;
	gotoxy(0, 0);
	//从头到尾扫描整个二维数组，如果是1，打印边框  2 打印方块  0 打印空格
	for(i = 0; i < 21; i++)
	{
		for(j = 0; j < 12; j++)
		{
			if(block[i][j] == 1)
				printf("\033[1;31m★ \033[0m");
			else if(block[i][j] == 2)		
            			printf("\033[1;34m☻ \033[0m");
			else
            			printf("\033[1;33m  \033[0m");
		}
		printf("\n\r");		//n 换行  r 回车
	}
}
void add_point_block()
{
	int i;
	for(i=0;i<4;i++)
	{
		block[point[i].y][point[i].x]=2;
	}
}
void clear_one_line(int line)
{
	int i,j;
	for(i=line;i>0;i--)
	{
		for(j=0;j<12;j++)
		block[i][j]=block[i-1][j];
	}
}
void clear_lines()
{
	int i,j;
	int lines_cleared = 0; 
	for(i=19;i>0;i--)
	{
		int k =0;
		for(j=0;j<12;j++)
		{
			if(block[i][j]==2)
			{
				k++;
			}
		}
		if(k==10)
		{
			clear_one_line(i);
			score += 100;
			if(score % 500 == 0)
			{
				level += 1;
				speed--;
				if(speed<1)
					speed=1;
			} 
			i++;
		}
	}
}	
void show_score()
{
	gotoxy(26, 2);
	printf("\033[1;36mScore: %d\033[0m", score);
}
void show_level()
{
	gotoxy(26, 4);
	printf("\033[1;36mLevel: %d\033[0m", level);
}
void show_speed()
{
	gotoxy(26,6);
	printf("\033[1;36mSpeed: %d\033[0m", speed);
}
void move_down()		//往下落，y++
{
	int i;
	for(i = 0; i < 4; i++)
	{	//判断下面是否有东西挡住，如果挡住了，就不能下落，挡住的东西有(边框，方块)
		if(block[point[i].y + 1][point[i].x] != 0)
		{
			
			add_point_block();
			clear_lines();	
			new_point();	
			return;		//结束整个函数
		}
	}
	for(i = 0; i < 4; i++)
	{
		point[i].y++;
	}
}
void right_move()
{
	int i,j;
	for(i = 0; i < 4; i++)
	{	
		if(block[point[i].y][point[i].x+1] != 0)
			return;
			
	}
	for(j=0;j<4;j++)
		point[j].x++;	
	
}
void left_move()
{
	int i,j;
	for(i = 0; i < 4; i++)
	{	
		if(block[point[i].y ][point[i].x -1] != 0)
		return;
				
	}
	for(j=0;j<4;j++)
		point[j].x--;	
	
}
void rotate_point()
{
	int i;
	int cx = point[0].x;
	int cy = point[0].y;

	// 1. 合法性检查（先不修改原坐标）
	
	for (i = 0; i < 4; i++)
	{
		int x = point[i].x;
		int y = point[i].y;

		int nx = cx - (y - cy);
		int ny = cy + (x - cx);

		// 边界检测
		if (nx < 1 || nx > 10 || ny < 0 || ny >= 21)
			return;

		// 碰撞检测
		if (block[ny][nx] != 0)
			return;
	}
	// 2. 全部合法，再真正旋转
	for (i = 0; i < 4; i++)
	{
		int x = point[i].x;
		int y = point[i].y;

		point[i].x = cx - (y - cy);
		point[i].y = cy + (x - cx);
	}
}
int isGameOver()
{
	int j=0;
	for(j=1;j<11;j++)
	{
		if(block[0][j]!=0)
		{
			//游戏结束
			return 1;


		}

	}

	//游戏未结束
	return 0;


}
int main()
{
	char ch;
	int i;
	system(STTY_US TTY_PATH);	//配置非阻塞IO
	srand(time(0));
	system("clear");
	init_block(); 		//因为边框只初始化一次，所以在while循环外面
	new_point();
	
	while(1)
	{
		display_all();
		draw_point();
		show_score();
		show_level();
		show_speed();
		fflush(stdout);		//刷新屏幕
			
		for(i=0;i<speed;i++)
		{
		ch = get_char();	//非阻塞IO
		if(ch == 'd')
		{right_move();
		break;}
		else if(ch == 'a')
		{left_move();
		break;}
		else if(ch == 's')
		break;
		else if(ch == 'w')
		{rotate_point();
		break;}
		else if(ch == 3)	//ctrl + c
		{
			system(STTY_DEF TTY_PATH);	//恢复为阻塞IO设置
			return 0;			//程序结束
		}
		
		
		usleep(50000);	//以微妙为单位延时， 50ms
		}
		
		move_down();//下落
	}
}
