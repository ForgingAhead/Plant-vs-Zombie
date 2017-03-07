#ifndef _sit_h_2009_10_21_
#define _sit_h_2009_10_21_

#include <time.h>  //用于计算时间间隔
#include "陈奉.h"


bool is_should_exit=false;
//状态的基类
class BaseSit
{
protected:
	BITMAP *its_buf;
	BITMAP *its_check_buf;

	BITMAP *its_check;
public:
	virtual void draw()=0;//画图
	virtual void act(int timelen)=0;//行动
	virtual void mouse_callback(int flags)=0;//鼠标的回调函数
};

//游戏状态
class GamingSit:public BaseSit
{
public:
	void init();
	virtual void draw();
	virtual void act(int timelen);
	virtual void mouse_callback(int flags);
	void add_zombie(int timelen);
	void add_sun(int timelen);
private:
	int its_time_count;
	int its_zombie_produce_need_time;
	int its_sun_time_count;
	int its_start_time;
};

//游戏结束状态
class EndSit:public BaseSit
{
	bool its_is_win;
public:
	void init(bool is_win);
	virtual void draw();
	virtual void act(int timelen);
	virtual void mouse_callback(int flags);
};

//全局变量
BaseSit *g_p_cur_sit;//指向当前状态
GamingSit *g_p_gaming_sit;//指向游戏状态
EndSit *g_p_end_sit;//指向游戏结束状态


//做所有的准备工作
void do_all_preparation_work();


//时间间隔的计算
class TimeCounter
{
private:
	int its_last_time;
public:
	TimeCounter();

	int get_timelen();
};


/*************************************************

class GamingSit:public BaseSit
{
public:
	virtual void init();
	virtual void draw(BITMAP *dest,BITMAP *check_dest);
	virtual void act(int timelen);
	virtual void mouse_callback(int flags);
};

**************************************************/

void GamingSit::init()
{
	SunNumber::set0();
	RoadGroup::remove_all();
	Mouse::init();

	its_time_count=-9000;
	its_zombie_produce_need_time=20000;
	its_sun_time_count=0;
	its_start_time=clock();
}
void GamingSit::draw()
{
	Draw::clear();

	RoadGroup::draw();
	SunNumber::draw();
	Mouse::draw();

	Draw::show();
}
void GamingSit::act(int timelen)
{
	if(RoadGroup::act(timelen))
	{
		g_p_end_sit->init(false);
		g_p_cur_sit=g_p_end_sit;
	}
	if(clock()-its_start_time>6*60000)
	{
		g_p_end_sit->init(true);
		g_p_cur_sit=g_p_end_sit;
	}
	add_zombie(timelen);
	add_sun(timelen);
}
void GamingSit::mouse_callback(int flags)
{
	int col=Draw::getpixel(mouse_x,mouse_y);

	if(flags&MOUSE_FLAG_LEFT_UP)//如果鼠标左键抬起
	{
		if(Mouse::is_drag())//如果在拖拽东西
		{
			Mouse::put_down();
		}
	}

	if(flags&MOUSE_FLAG_LEFT_DOWN)//如果鼠标左键按下
	{
		if(getr(col)>200)//如果在按退出键 或者 上边的4个东西
		{
			if(getr(col)==255)//退出按钮
			{
				is_should_exit=true;
			}
			else//上边的4个东西
			{
				Object *p=0;
				switch(getr(col))
				{
				case 254://向日葵
					p=new SunFlower();
					break;
				case 253://低攻豌豆
					p=new LowAttackPea();
					break;
				case 252://高攻豌豆
					p=new HighAttackPea();
					break;
				case 251://铲子
					p=new Spade();
					break;
				default:
					assert(0);
				}
				Mouse::drag(p);
			}
		}
		else//看看是不是在阳光上
		{
			Sun *p=(Sun *)RoadGroup::mouse_search_sun(mouse_x,mouse_y);

			if(p)
			{
				RoadGroup::remove(p);
				SunNumber::increase(25);
			}
		}
	}
}
void GamingSit::add_zombie(int timelen)
{
	its_time_count+=timelen;
	srand(clock());

	if(its_time_count>its_zombie_produce_need_time)
	{
		its_time_count-=its_zombie_produce_need_time;
		its_zombie_produce_need_time*=0.95;
		if(its_zombie_produce_need_time<2000)
			its_zombie_produce_need_time=2000;
		int road_index=rand()%5;
		int y=RoadGroup::get_y_by_road_index(road_index);
		Zombie *p;
		if(its_zombie_produce_need_time>10000)
		{
			p=new CommonZombie(750,y);
		}
		else
		{
			if(rand()%(1+its_zombie_produce_need_time/2000))
				p=new CommonZombie(750,y);
			else
				p=new BucketZombie(750,y);
		}
		RoadGroup::add(p);
	}
}
void GamingSit::add_sun(int timelen)
{
	its_sun_time_count+=timelen;

	srand(clock());

	if(its_sun_time_count>=5000)
	{
		its_sun_time_count-=5000;

		int x0=Road::get_x_by_cell_index(0);
		int x1=Road::get_x_by_cell_index(Road::get_cell_n()-1);
		int new_x=x0+(x1-x0)*(rand()*1.0/RAND_MAX);

		int y0=RoadGroup::get_y_by_road_index(0);
		int y1=RoadGroup::get_y_by_road_index(4);
		int new_y=y0+(y1-y0)*(rand()*1.0/RAND_MAX);

		RoadGroup::add(new Sun(new_x,new_y));
	}
}

/*************************************************

//游戏结束状态
class EndSit:public BaseSit
{
public:
	virtual void init();
	virtual void draw();
	virtual void act(int timelen);
	virtual void mouse_callback(int flags);
};
**************************************************/
void EndSit::init(bool is_win)
{
	Mouse::init();
	its_is_win=is_win;
}
void EndSit::draw()
{
	Draw::clear();

	Background::draw(its_is_win);
	Mouse::draw();

	Draw::show();
}
void EndSit::act(int timelen)
{
}
void EndSit::mouse_callback(int flags)
{
	int col=Draw::getpixel(mouse_x,mouse_y);

	if(flags&MOUSE_FLAG_LEFT_UP)//如果鼠标左键抬起
	{
		if(getr(col)==100)//如果按再来一局
		{
			g_p_gaming_sit->init();
			g_p_cur_sit=g_p_gaming_sit;
		}
		else if(getr(col)==101)//如果按退出按钮
		{
			is_should_exit=true;
		}
	}
}
/*************************************************

//时间间隔的计算
class TimeCounter
{
private:
	int its_last_time;
public:
	TimeCounter();

	int get_timelen();
};
**************************************************/
TimeCounter::TimeCounter()
{
	its_last_time=clock();
}
int TimeCounter::get_timelen()
{
	int cur=clock();
	int re=cur-its_last_time;
	its_last_time=cur;
	return re;
}



//做所有的准备工作
void do_all_preparation_work()
{
	Draw::create_bitmap();
	UpsideColumn::load_picture();
	Sun::load_picture();
	SunNumber::load_picture();
	SunFlower::load_picture();
	LowAttackPea::load_picture();
	HighAttackPea::load_picture();
	CommonZombie::load_picture();
	BucketZombie::load_picture();
	Bullet::load_picture();
	Mouse::load_picture();
	Background::load_picture();
	Spade::load_picture();
	RoadGroup::load_picture();

	g_p_gaming_sit=new GamingSit();
	g_p_end_sit=new EndSit();
}


#endif