#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<sys/time.h>
#include<stdlib.h>
#include<string.h>
#include "../keyboard/keyboard.h"
//背景色
#define BC 7
//前景色
int FC=1;
//屏幕宽度
#define W 10
//屏幕高度
#define H 20
//图案
int cur_shape=6;
//数组用来表示方块形状
struct shape{
    int model[5][5];
};
//装方块图案
struct shape shape_arr[7]={
    {
        0,0,0,0,0,
        0,0,1,0,0,
        0,1,1,1,0,
        0,0,0,0,0,
        0,0,0,0,0
    },
    {
        0,0,0,0,0,
        0,1,0,0,0,
        0,1,1,1,0,
        0,0,0,0,0,
        0,0,0,0,0
    },
    {
        0,0,0,0,0,
        0,0,0,1,0,
        0,1,1,1,0,
        0,0,0,0,0,
        0,0,0,0,0
    },
    {
        0,0,0,0,0,
        0,1,1,0,0,
        0,0,1,1,0,
        0,0,0,0,0,
        0,0,0,0,0
    },
    {
        0,0,0,0,0,
        0,0,1,1,0,
        0,1,1,0,0,
        0,0,0,0,0,
        0,0,0,0,0
    },
    {
        0,0,0,0,0,
        0,1,1,0,0,
        0,1,1,0,0,
        0,0,0,0,0,
        0,0,0,0,0
    },
    {
        0,0,0,0,0,
        0,0,1,0,0,
        0,0,1,0,0,
        0,0,1,0,0,
        0,0,0,0,0
    }
};
struct data{
    //控制屏幕的列
    int x; 
    //控制屏幕的行
    int y;
};
//图形下落的起始位置
struct data t;
void init_t(){
    t.x=5;
    t.y=0;
}
//保存背景的数组
int backgroup[H][W]={0};
void draw_element(int x,int y,int c){
    //x坐标需要以2倍的速度进行 因为[]占两 个字符
    x*=2;
    x++;
    y++;
    //隐藏光标
    printf("\033[?25l");
    //控制光标位置
    printf("\033[%d;%dH",y,x);
    //设置颜色
    printf("\033[3%dm\033[4%dm",c,c);
    //输出方块
    printf("[]");
    //刷新缓存
    fflush(stdout);
    //关闭设置的属性
    printf("\033[0m");
}
void draw_shape(int x,int y,struct shape p,int c){
    int i=0;
    int j=0;
    for(i=0;i<5;i++){
        for(j=0;j<5;j++){
            if(p.model[i][j]!=0){
                draw_element(x+j,y+i,c);
            }
        }
    }
}
void draw_back(){
    int i=0;
    int j=0;
    for(i=0;i<H;i++){
        for(j=0;j<W;j++){
            if(backgroup[i][j]==0){
                draw_element(j,i,BC);
            }else{
                draw_element(j,i,backgroup[i][j]);
            }
        }
    }
}
void set_back(struct data* t,struct shape p){
    int i=0;
    int j=0;
    for(i=0;i<5;i++){
        for(j=0;j<5;j++){
            if(p.model[i][j]!=0){
                backgroup[t->y+i][t->x+j]=FC;
            }
        }
    }
}
int can_move(int x,int y,struct shape p){
    int i=0;
    int j=0;
    for(i=0;i<5;i++){
        for(j=0;j<5;j++){
            if(p.model[i][j]==0){
                continue;
            }
            if(x+j>=W){
                return 0;
            }
            if(y+i>=H){
                return 0;
            }
            if(x+j<0){
                return 0;
            }
            if(backgroup[y+i][x+j]!=0){
                return 0;
            }
        }
    }
    return 1;
}
void clean_line(){
    int i=0;
    int j=0;
    int k=0;
    for(i=0;i<H;i++){
        int total=0;
        for(j=0;j<W;j++){
            if(backgroup[i][j]!=0){
                total++;
            }
        }
        if(total==W){
            for(k=i;k>0;k--){
                memcpy(backgroup[k],backgroup[k-1],sizeof(backgroup[k]));
            }
            memset(backgroup[0],0x00,sizeof(backgroup[0]));
        }
    }
}
void tetris_timer(struct data* t){
    draw_shape(t->x,t->y,shape_arr[cur_shape],BC);
    if(can_move(t->x,t->y+1,shape_arr[cur_shape])){
        t->y++;
    }else{
       set_back(t,shape_arr[cur_shape]);
       clean_line();
       draw_back();
       do{
             FC=rand()%7;
       }while(FC==BC);
            t->x=0;
            t->y=0;
            cur_shape=rand()%7;
        }
    draw_shape(t->x,t->y,shape_arr[cur_shape],FC);
}

struct shape turn_90(struct shape p){
    struct shape t;
    int i=0;
    int j=0;
    for(i=0;i<5;i++){
        for(j=0;j<5;j++){
            t.model[i][j]=p.model[4-j][i];
        }
    }
    return t;
}
int tetris(struct data* t){
    int c=get_key();
    int ret=0; 
    if(is_up(c)){
        draw_shape(t->x,t->y,shape_arr[cur_shape],BC);
        shape_arr[cur_shape]=turn_90(shape_arr[cur_shape]);
        if(can_move(t->x,t->y,shape_arr[cur_shape])==0){
            shape_arr[cur_shape]=turn_90(shape_arr[cur_shape]);
            shape_arr[cur_shape]=turn_90(shape_arr[cur_shape]);
            shape_arr[cur_shape]=turn_90(shape_arr[cur_shape]);
        }
        draw_shape(t->x,t->y,shape_arr[cur_shape],FC);
    }else if(is_left(c)){
        draw_shape(t->x,t->y,shape_arr[cur_shape],BC);
        if(can_move(t->x-1,t->y,shape_arr[cur_shape]))
        t->x--;
        draw_shape(t->x,t->y,shape_arr[cur_shape],FC);
    }else if(is_right(c)){
        draw_shape(t->x,t->y,shape_arr[cur_shape],BC);
        if(can_move(t->x+1,t->y,shape_arr[cur_shape]))
        t->x++;
        draw_shape(t->x,t->y,shape_arr[cur_shape],FC);
    }else if(is_down(c)){
        draw_shape(t->x,t->y,shape_arr[cur_shape],BC);
        if(can_move(t->x,t->y+1,shape_arr[cur_shape]))
        t->y++;
        draw_shape(t->x,t->y,shape_arr[cur_shape],FC);
    }else if(is_esc(c)){
        ret=1;
    }
    return ret;
}
void handler(int s){
    if(s==SIGALRM){
        tetris_timer(&t);
    }
    else if(s==SIGINT){
        printf("\033[?0m");
        printf("\033[?25h");
        recover_keyboard();
        exit(0);
    }
}
int main(){
    init_t(t);
    
    init_keyboard();
    draw_back();
    struct sigaction act;
    act.sa_handler=handler;
    act.sa_flags=0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGALRM,&act,NULL);
    struct itimerval it;
    it.it_value.tv_sec=0;
    it.it_value.tv_usec=1;
    it.it_interval.tv_sec=1;
    it.it_interval.tv_usec=0;
    setitimer(ITIMER_REAL,&it,NULL);
    while(1){
        if(tetris(&t)==1){
            break;
        }
    }
    printf("\033[?25h");
    recover_keyboard();
    return 0;
}
