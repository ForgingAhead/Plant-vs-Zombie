#ifndef _chenfeng_h_2009_10_21_
#define _chenfeng_h_2009_10_21_

#include <iostream>
#include <vector>
#include <cmath>
#include <allegro.h>
#include <assert.h>

const int zombie_object_type=0;
const int plant_object_type=1;
const int bullet_object_type=2;
const int sun_object_type=3;


class Zombie;
class Plant;
class Sun;



class Draw
{
protected:
	static BITMAP *its_buf;
	static BITMAP *its_buf_check;
	static BITMAP *its_check;
public:
	virtual void draw()=0;
	static void create_bitmap();
	static void clear();
	static void show();
	static int getpixel(int x,int y);
};
BITMAP *Draw::its_buf=0;
BITMAP *Draw::its_buf_check=0;
BITMAP *Draw::its_check=0;

void Draw::create_bitmap()
{
	its_buf=::create_bitmap(800,600);
	its_buf_check=::create_bitmap(800,600);
	its_check=::create_bitmap(800,600);
}
void Draw::clear()
{
	clear_to_color(its_buf,makecol(0,0,0));
	clear_to_color(its_buf_check,makecol(0,0,0));
}
void Draw::show()
{
	blit(its_buf,screen,0,0,0,0,its_buf->w,its_buf->h);
	blit(its_buf_check,its_check,0,0,0,0,its_buf_check->w,its_buf_check->h);
}
int Draw::getpixel(int x,int y)
{
	return ::getpixel(its_check,x,y);
}

class UpsideColumn:public Draw
{
private:
	static BITMAP *its_bmp;
	static BITMAP *its_che;
public:
	static void load_picture();
	static void draw();
};
BITMAP *UpsideColumn::its_bmp=0;
BITMAP *UpsideColumn::its_che=0;

void UpsideColumn::load_picture()
{
	its_bmp=load_bitmap("upside_column.bmp",0);
	its_che=load_bitmap("upside_column_check.bmp",0);
}
void UpsideColumn::draw()
{
	masked_blit(its_bmp,its_buf,0,0,0,0,its_bmp->w,its_bmp->h);
	masked_blit(its_che,its_buf_check,0,0,0,0,its_che->w,its_che->h);
}



class Object :public Draw
{    //物体类
protected :
	int its_x;
	int its_y;
public :
	virtual bool on_put_down()=0;
	void set_position(int x,int y);
	int get_x(){return its_x;}
	int get_y(){return its_y;}
};
void Object::set_position(int x,int y)
{
	its_x=x;
	its_y=y;
}

class ObjectOnRoad: public Object
{    //跑道上的物体
protected :
	int its_road_index;
	int its_object_type;
	
public :
	ObjectOnRoad(int x, int y, int road_index, int object_type);
//	~ObjectOnRoad();

	int get_road_index(){return its_road_index;}
	int get_object_type(){return its_object_type;}

	void set_road_index(int road_index){its_road_index=road_index;}
	virtual bool act(int timelen)=0;
};


//阳光类
class Sun:public ObjectOnRoad
{
public:
	Sun(int x, int y):ObjectOnRoad(x,y,0,sun_object_type),its_exist_time(0),its_max_exist_time(8000){};
	bool act(int timelen);
	bool on_put_down();
	virtual void draw();
	static void load_picture();
	static int get_bmp_w();
	static int get_bmp_h();
private:
	int its_exist_time;
	int its_max_exist_time;
	static BITMAP *its_bmp;
};



class Road
{   //跑道

private :
	std::vector<ObjectOnRoad*> its_object_list; 

	static int its_cell_w;
	static int its_cell_n;
	static int its_start_x;
public :
	void add(ObjectOnRoad* p_object); 
	void remove(ObjectOnRoad*  p_object);
	bool act(int timelen);
	bool plant_search_zombie();
	ObjectOnRoad* zombie_search_plant(int x, int zone);
	ObjectOnRoad* spade_search_plant(int x);
	ObjectOnRoad* bullet_search_zombie(int x, int zone);
	ObjectOnRoad* mouse_search_sun(int x, int y);
	bool plant_search_empty_position(int* x, int mx);
	std::vector<ObjectOnRoad *> &get_object_list(){return its_object_list;}
	void remove_all();
	void draw();
	static int get_x_by_cell_index(int cell_index);
	static int get_cell_index_by_x(int x);
	static int get_cell_n(){return its_cell_n;}
};

class RoadGroup :public Draw
{  //跑道群   

private :
	static Road road_array[5];
//	static const int WIDTH;
//	static const int HEIGHT;
	static int its_cell_h;
	static int its_start_y;

	static BITMAP *its_bmp;
public :
	RoadGroup(){};
	~RoadGroup();

	static bool act(int timelen);
	static void add(ObjectOnRoad* object);
	static void remove(ObjectOnRoad* object);
	static bool plant_search_zombie(int index);
	static ObjectOnRoad* zombie_search_plant(int index, int x, int zone);
	static ObjectOnRoad* spade_search_plant(int x, int y);
	static ObjectOnRoad* bullet_search_zombie(int index, int x, int zone);
	static ObjectOnRoad* mouse_search_sun(int x, int y);
	static bool plant_search_empty_position(int* x, int* y, int mx, int my);
	static void remove_all();
	static void load_picture();
	static void draw();
	static int get_y_by_road_index(int road_index);
	static int get_road_index_by_y(int y);
};

Road RoadGroup::road_array[5];
//const int ROAD_Y[5]={130,230,330,430,530};		//跑道y轴位置
//const int RoadGroup::WIDTH = 81;
//const int RoadGroup::HEIGHT = 100;

int RoadGroup::its_cell_h	= 100;
int RoadGroup::its_start_y	= 130;
int Road::its_cell_w		= 81;
int Road::its_start_x		= 73;
int Road::its_cell_n		= 9;

BITMAP *RoadGroup::its_bmp=0;



//实现

ObjectOnRoad:: ObjectOnRoad(int x, int y, int index, int object_type) 
{
	its_x = x;
	its_y = y;
	its_road_index = index;
	its_object_type = object_type;
}

void Road:: add(ObjectOnRoad* p_object) 
{
	its_object_list.push_back(p_object);
}

void Road:: remove(ObjectOnRoad*  p_object) 
{
//	its_object_list.erase(&p_object);	//需要找到这个iterator的位置吗？？这个就是？

	std::vector<ObjectOnRoad*>::iterator it;
	for(it=its_object_list.begin();it!=its_object_list.end();it++)
	{
		if(*it==p_object)
		{
			its_object_list.erase(it);
			return;
		}
	}
	assert(0);
}

bool Road:: act(int timelen)		//统一按bool的
{
#if 0
	std::vector<ObjectOnRoad*>::iterator iter;
	iter = its_object_list.begin();
	for(;iter != its_object_list.end();)
	{
		if((*iter)->get_object_type() == zombie_object_type)
		{
			if((*iter)->act(timelen) == true)
			{
				return 1;
			}
		}
		else if((*iter)->get_object_type() == sun_object_type)
		{
			if((*iter)->act(timelen) == true)
			{	
				remove(*iter);
			}
		}
		else
		{
			(*iter)->act(timelen);
		}

		 ++iter;
		 ;

	}
	return 0;

#endif 

	int i;
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *p=its_object_list[i];

		if(p->get_object_type() == zombie_object_type)
		{
			if(p->act(timelen) == true)
			{
				return 1;
			}
		}
		else if(p->get_object_type() == sun_object_type   ||
			    p->get_object_type() == bullet_object_type     )
		{
			if(p->act(timelen) == true)
			{	
				remove(p);
			}
		}
		else
		{
			p->act(timelen);
		}
	}
	return 0;

}

bool Road:: plant_search_zombie()
{
	int i;
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *p=its_object_list[i];
		if(p->get_object_type() == zombie_object_type)
			return true;
	}
	return false;
}

ObjectOnRoad* Road:: zombie_search_plant(int x, int zone)
{
	ObjectOnRoad* p=0;
	int min_dis=-1;

	int i;
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *cur_p=its_object_list[i];
		if(cur_p->get_object_type() == plant_object_type)
		{
			int dis=x - cur_p->get_x();
			if(dis <= zone && dis>=0)	//zone 即是WIDTH？？
			{
				if(p==0 || dis<min_dis)
				{
					p=cur_p;
					min_dis=dis;
				}
			}
		}
	}

	return p;
}

//	for(std::vector<ObjectOnRoad*>::iterator iter = its_object_list.begin(); iter != its_object_list.end(); ++iter)
//	{
ObjectOnRoad* Road:: spade_search_plant(int x)	//my没有用的呢。。。
{
	int i;
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *p=its_object_list[i];
		if(p->get_object_type() == plant_object_type)
		{
			if(abs(x - p->get_x()) < its_cell_w/2)	//鼠标指在那个方格区域中
				return p;
		}
	}
	return 0;
}

ObjectOnRoad* Road:: bullet_search_zombie(int x, int zone)
{
	ObjectOnRoad* p=0;
	int min_dis=-1;

	int i;
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *cur_p=its_object_list[i];
		if(cur_p->get_object_type() == zombie_object_type)
		{
			int dis=cur_p->get_x() - x ;
			if(dis <= zone && dis>=0)
			{
				if(p==0 || dis<min_dis)
				{
					p=cur_p;
					min_dis=dis;
				}
			}
		}
	}

	return p;
}

//int const sun_width = 40;

ObjectOnRoad* Road:: mouse_search_sun(int x, int y)		//如果已经具体到跑道了，y应该没用了
{
	int i;
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *p=its_object_list[i];
		if(p->get_object_type() == sun_object_type)
		{
			if(abs(x - p->get_x()) <= Sun::get_bmp_w()/2)
				if(abs(y - p->get_y()) <= Sun::get_bmp_h()/2)
					return p;
		}
	}
	return 0;
}

bool Road:: plant_search_empty_position(int* x, int mx)
{
	int i;
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *p=its_object_list[i];
		if(p->get_object_type() == plant_object_type)
			if(abs(mx - p->get_x()) <= its_cell_w/2)
				return false;
	}
	*x=get_x_by_cell_index(get_cell_index_by_x(mx));
	return true;
}
void Road:: remove_all()
{
	its_object_list.clear();//这里会有一点点的内存没被释放
}
void Road:: draw()
{
	int i;
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *p=its_object_list[i];
		if(p->get_object_type()==plant_object_type)
			p->draw();
	}
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *p=its_object_list[i];
		if(p->get_object_type()==zombie_object_type)
			p->draw();
	}
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *p=its_object_list[i];
		if(p->get_object_type()==sun_object_type)
			p->draw();
	}
	for(i=0;i<its_object_list.size();i++)
	{
		ObjectOnRoad *p=its_object_list[i];
		if(p->get_object_type()==bullet_object_type)
			p->draw();
	}
}
int Road:: get_x_by_cell_index(int cell_index)
{
	assert(cell_index>=0 && cell_index<its_cell_n);

	return its_start_x+cell_index*its_cell_w;
}
int Road:: get_cell_index_by_x(int x)
{
	int index=(x-its_start_x+its_cell_w/2)/its_cell_w;

	if(index>=0&&index<its_cell_n)
		return index;
	else
		return -1;
}

void RoadGroup:: add(ObjectOnRoad* object) 
{
	int road_index=get_road_index_by_y(object->get_y());
	object->set_road_index(road_index);
	road_array[road_index].add(object);
}

void RoadGroup:: remove(ObjectOnRoad* object) 
{
	road_array[object->get_road_index()].remove(object);
}


bool RoadGroup:: act(int timelen)
{
	bool is_out = 0;
	for(int i = 0; i < 5; i++)
	{
		if(road_array[i].act(timelen))
		{
			is_out = 1;
		}
	}
	return is_out;
}

bool RoadGroup:: plant_search_zombie(int index)
{
	return road_array[index].plant_search_zombie();
}

ObjectOnRoad* RoadGroup:: zombie_search_plant(int index, int x, int zone)
{
	return road_array[index].zombie_search_plant(x, zone);
}

ObjectOnRoad* RoadGroup:: spade_search_plant(int x, int y)
{
	int index=get_road_index_by_y(y);
	if(index>=0)
		return road_array[index].spade_search_plant(x);
	else
		return 0;
#if 0
	int index;
	for(int i = 0; i < 5; i++)
	{
		if(abs(y - ROAD_Y[i]) < HEIGHT / 2)
		{
			index = i;
			return road_array[index].spade_search_plant(x, y,WIDTH);
		}
	}
	return 0;
#endif
}

ObjectOnRoad* RoadGroup:: bullet_search_zombie(int index, int x, int zone)
{
	return road_array[index].bullet_search_zombie(x, zone);
}

ObjectOnRoad* RoadGroup:: mouse_search_sun(int x, int y)
{
	int index=get_road_index_by_y(y);
	if(index>=0)
	{
		ObjectOnRoad* p;

		p=road_array[index].mouse_search_sun(x,y);
		if(p)
			return p;

		if(index!=0)
		{
			p=road_array[index-1].mouse_search_sun(x,y);
			if(p)
				return p;
		}

		if(index!=4)
		{
			p=road_array[index+1].mouse_search_sun(x,y);
			if(p)
				return p;
		}
		return 0;
	}
	else
		return 0;
#if 0
	int index;
	for(int i = 0; i < 5; i++)
	{
		if(abs(y - ROAD_Y[i]) < HEIGHT / 2)
		{
			index = i;
			return road_array[index].mouse_search_sun(x, y);
		}
	}
	return 0;
#endif
}

bool RoadGroup:: plant_search_empty_position(int* x, int* y, int mx, int my)
{
	int index=get_road_index_by_y(my);

	if(index>=0)
	{
		*y=get_y_by_road_index(index);
		return road_array[index].plant_search_empty_position(x,mx);
	}
	else
	{
		return 0;
	}
}
void RoadGroup:: remove_all()
{
	for(int i = 0; i < 5; i++)
	{
		road_array[i].remove_all();
	}
}
void RoadGroup:: load_picture()
{
	its_bmp=load_bitmap("road_group.bmp",0);
}
void RoadGroup:: draw()
{
	blit(its_bmp,its_buf,0,0,0,0,its_bmp->w,its_bmp->h);
	UpsideColumn::draw();
	for(int i = 0; i < 5; i++)
	{
		road_array[i].draw();
	}
}
int RoadGroup:: get_y_by_road_index(int road_index)
{
	assert(road_index>=0 && road_index<5);

	return its_start_y+road_index*its_cell_h;
}
int RoadGroup:: get_road_index_by_y(int y)
{
	int index=(y-its_start_y+its_cell_h/2)/its_cell_h;

	if(index>=0&&index<5)
		return index;
	else
		return -1;
}

//植物类
class Plant:public ObjectOnRoad
{
public:
	Plant(int hp,int x, int y, int road_index,int sun_cost):ObjectOnRoad(x,y,road_index,plant_object_type),its_hp(hp),its_sun_cost(sun_cost){};
	virtual bool act(int timelen)=0;
	bool under_attack(int attack);
	bool on_put_down();
private:
	int its_hp;
	int its_sun_cost;
};



//阳光数量类
class SunNumber:public Draw
{
public:
	static void increase(int);
	static bool decrease(int);
	static void set0();
	static void draw();
	static void load_picture();
private:
	static const int its_x;
	static const int its_y;
	static int its_amount;
	static BITMAP *its_bmp;
};

//向日葵类
class SunFlower:public Plant
{
public:
	SunFlower():Plant(400,0,0,0,50),its_exist_time(0),its_produce_sun_need_time(20000){};
//	SunFlower(int x, int y, int road_index):Plant(100,1,x,y,road_index,50),its_exist_time(0){};
	bool act(int timelen);
	virtual void draw();
	static void load_picture();
private:
	int its_exist_time;
	int its_produce_sun_need_time;
	static BITMAP *its_bmp;
};


//低攻击豌豆
class LowAttackPea:public Plant
{
public:
	LowAttackPea():Plant(400,0,0,0,100),its_attack_rest_time(2000){its_have_rest=its_attack_rest_time;}
//	LowAttackPea(int x, int y, int road_index):Plant(60,300,x,y,road_index,70),its_have_rest(0){};
	bool act(int);
	static void load_picture();
	virtual void draw();
private:
	static BITMAP *its_bmp;
	int its_have_rest;
	int its_attack_rest_time;
};


//高攻击豌豆
class HighAttackPea:public Plant
{
public:
	HighAttackPea():Plant(400,0,0,0,200),its_attack_rest_time(2000),its_second_attack_rest_time(200),its_is_not_attack(true){its_have_rest=its_attack_rest_time;}
//	HighAttackPea(int x, int y, int road_index):Plant(60,300,x,y,road_index,100){};
	bool act(int);
	static void load_picture();
	virtual void draw();
private:
	static BITMAP *its_bmp;
	int its_have_rest;
	bool its_is_not_attack;
	int its_attack_rest_time;
	int its_second_attack_rest_time;
};


BITMAP *Sun::its_bmp=0;

bool Sun::act(int time_between)
{
	its_exist_time+=time_between;
	if(its_exist_time>=its_max_exist_time)
		return 1;//如果存在时间大于its_max_exist_time，通知删除
	else return 0;
}
bool Sun::on_put_down()
{
	return 1;//通知删除，没用,保留
}
void Sun::draw()
{
	masked_blit(its_bmp,its_buf,0,0,its_x-its_bmp->w/2,its_y-its_bmp->h/2,its_bmp->w,its_bmp->h);
}
void Sun::load_picture()
{
	its_bmp=load_bitmap("sun.bmp",0);
}
int Sun::get_bmp_w()
{
	return its_bmp->w;
}
int Sun::get_bmp_h()
{
	return its_bmp->h;
}

BITMAP *SunNumber::its_bmp=0;
void SunNumber::increase(int i)
{
	its_amount+=i;
}

int SunNumber::its_amount=0;

bool SunNumber::decrease(int i)
{
	if(its_amount<i)
		return false;

	its_amount-=i;
	return true;
}
void SunNumber::set0()
{
	its_amount=50;
}
void SunNumber::load_picture()
{
	its_bmp=load_bitmap("sun_number.bmp",0);
}
void SunNumber::draw()
{
	masked_blit(its_bmp,its_buf,0,0,0,0,its_bmp->w,its_bmp->h);
	textprintf_centre_ex(its_buf,font,its_bmp->w/2,its_bmp->h/2+19,makecol(255,0,0),-1,"%d",its_amount);
}


BITMAP *SunFlower::its_bmp=0;

bool SunFlower::act(int time_between)
{
	its_exist_time+=time_between;
	if(its_exist_time>=its_produce_sun_need_time)
	{
		//与上次新建太阳时间间隔大于its_produce_sun_need_time，新建太阳
		its_exist_time-=its_produce_sun_need_time;
		Sun* tmp=new Sun(its_x+rand()%40-20,its_y+rand()%10-5+20);
		RoadGroup::add(tmp);
	}
	return 0;
}
void SunFlower::draw()
{
	masked_blit(its_bmp,its_buf,0,0,its_x-its_bmp->w/2,its_y-its_bmp->h/2,its_bmp->w,its_bmp->h);
}
void SunFlower::load_picture()
{
	its_bmp=load_bitmap("sunflower.bmp",0);
}

bool Plant::under_attack(int hp_decrease)
{
	if(hp_decrease>0)
	{
		its_hp-=hp_decrease;//先被攻击
	}
	if(its_hp<=0)
	{
		return 1;//如果没血，通知被删
	}
	else
	{
		return 0;
	}
}

bool Plant::on_put_down()
{
	int x;
	int y;
	if(RoadGroup::plant_search_empty_position(&x,&y,its_x,its_y))
	{
		if(SunNumber::decrease(its_sun_cost)==false)
			return 1;
		its_x=x;
		its_y=y;
		RoadGroup::add(this);
		return 0;//如果位置恰当，放入跑道
	}
	else
	{
		return 1;//否则通知被删
	}
}

//炮弹
class Bullet:public ObjectOnRoad
{
private:
	int		its_attack;
	double	its_speed;
	int		its_attack_zone;
	double	its_double_x;
	static BITMAP *its_bmp;
public:
	//构造函数
	Bullet(int x,int y,int attack);

	//其他函数
	bool act(int timelen);

	virtual bool on_put_down();

	static void load_picture();
	virtual void draw();
};



BITMAP *LowAttackPea::its_bmp=0;

bool LowAttackPea::act(int time_between)
{
	its_have_rest+=time_between;

	if(its_have_rest>=its_attack_rest_time&&RoadGroup::plant_search_zombie(its_road_index))
	{
		its_have_rest=0;
		Bullet* tmp=new Bullet(its_x+27,its_y-18,100);
		RoadGroup::add(tmp);
	}
	return 0;
}
void LowAttackPea::load_picture()
{
	its_bmp=load_bitmap("low_attack_pea.bmp",0);
}
void LowAttackPea::draw()
{
	masked_blit(its_bmp,its_buf,0,0,its_x-its_bmp->w/2,its_y-its_bmp->h/2,its_bmp->w,its_bmp->h);
}

BITMAP *HighAttackPea::its_bmp=0;
bool HighAttackPea::act(int time_between)
{
	its_have_rest+=time_between;

	if(its_is_not_attack)
	{
		if(its_have_rest>=its_attack_rest_time&&RoadGroup::plant_search_zombie(its_road_index))
		{
			its_have_rest=0;
			Bullet* tmp=new Bullet(its_x+31,its_y-23,100);
			RoadGroup::add(tmp);
			its_is_not_attack=false;
		}
	}
	else
	{
		if(its_have_rest>=its_second_attack_rest_time)
		{
			its_have_rest=0;
			Bullet* tmp=new Bullet(its_x+31,its_y-23,100);
			RoadGroup::add(tmp);
			its_is_not_attack=true;
		}
	}
	return 0;
}
void HighAttackPea::load_picture()
{
	its_bmp=load_bitmap("high_attack_pea.bmp",0);
}
void HighAttackPea::draw()
{
	masked_blit(its_bmp,its_buf,0,0,its_x-its_bmp->w/2,its_y-its_bmp->h/2,its_bmp->w,its_bmp->h);
}

//僵尸类  
class Zombie:public ObjectOnRoad
{
protected:
	bool		its_is_attacking;
private:
	int			its_hp;
	double		its_speed;
	int			its_attack;
	int			its_attack_rest_time;
	int			its_have_rest_time;
	int	        its_attack_zone;
	double		its_double_x;
	double		its_double_y;
public:
	Zombie(int x,int y,int road_index,int hp,double speed,int attack,int attack_rest_time);

	bool act(int timelen);
	bool under_attack(int attack);
	virtual bool on_put_down();
};

//普通僵尸
class CommonZombie:public Zombie
{
public:
	CommonZombie(int x,int y);
	virtual void draw();
	static void load_picture();
private:
	static BITMAP *its_attack_bmp;
	static BITMAP *its_walk_bmp;
};

//铁桶僵尸
class BucketZombie:public Zombie
{
public:
	BucketZombie(int x,int y);
	virtual void draw();
	static void load_picture();
private:
	static BITMAP *its_attack_bmp;
	static BITMAP *its_walk_bmp;
};



class Mouse:public Draw
{
private:
	int		its_x;
	int		its_y;
	static Object*	its_current_object;
	static BITMAP *its_bmp;
public:
	void static drag(Object* current_object);
	void static put_down();
	bool static is_drag();
	void static init();
	void static load_picture();
	void static draw();
};

class Spade:public Object
{
private:
	static BITMAP *its_bmp;
public:
	bool on_put_down();
	static void load_picture();
	virtual void draw();
};

Zombie::Zombie(int x,int y,int road_index,int hp,double speed,int attack,int attack_rest_time)
	  : ObjectOnRoad(x,y,road_index,zombie_object_type),its_attack_zone(40),its_is_attacking(false)
{
	its_hp=hp;
	its_speed=speed;
	its_attack=attack;
	its_attack_rest_time=attack_rest_time;
	its_have_rest_time=0;
	its_double_x=x;
	its_double_y=y;
}


bool Zombie::act(int timelen)
{
	Plant* p;

	p=(Plant*)RoadGroup::zombie_search_plant(its_road_index,its_x,its_attack_zone);
	if(p!=0)
	{
		its_is_attacking=true;
		its_have_rest_time+=timelen;
		if(its_have_rest_time>=its_attack_rest_time)//如果到攻击时间了
		{
			its_have_rest_time=0;
			if(p->under_attack(its_attack)==true)//如果被僵尸吃掉
				RoadGroup::remove(p);
		}
	}
	else
	{
		its_is_attacking=false;
		its_double_x=its_double_x-its_speed*timelen;
		its_x=(int)its_double_x;

		if(its_x<0)
		{
			return true;//僵尸到达最左端  游戏结束
		}
	}
	return false;
}

bool Zombie::under_attack(int attack)
{
    its_hp=its_hp-attack;
	if(its_hp<=0)
		return true;
	else
		return false;
}
bool Zombie::on_put_down()
{
	return true;//没用，保留
}



BITMAP *CommonZombie::its_attack_bmp=0;
BITMAP *CommonZombie::its_walk_bmp=0;

CommonZombie::CommonZombie(int x,int y)
:Zombie(x,y,0,/* hp= */1000,/* speed= */0.007,/* attack= */100,/* attack_rest_time= */700){}
void CommonZombie::draw()
{
	if(its_is_attacking)
		masked_blit(its_attack_bmp,its_buf,0,0,its_x-its_attack_bmp->w/2,its_y-its_attack_bmp->h/2-20,its_attack_bmp->w,its_attack_bmp->h);
	else
		masked_blit(its_walk_bmp,its_buf,0,0,its_x-its_walk_bmp->w/2,its_y-its_walk_bmp->h/2-20,its_walk_bmp->w,its_walk_bmp->h);
}
void CommonZombie::load_picture()
{
	its_attack_bmp=load_bitmap("common_zombie_attack.bmp",0);
	its_walk_bmp=load_bitmap("common_zombie_walk.bmp",0);
}

BITMAP *BucketZombie::its_attack_bmp=0;
BITMAP *BucketZombie::its_walk_bmp=0;

BucketZombie::BucketZombie(int x,int y)
: Zombie(x,y,0,/* hp= */4000,/* speed= */0.007,/* attack= */100,/* attack_rest_time= */700){}
void BucketZombie::draw()
{
	if(its_is_attacking)
		masked_blit(its_attack_bmp,its_buf,0,0,its_x-its_attack_bmp->w/2,its_y-its_attack_bmp->h/2-20,its_attack_bmp->w,its_attack_bmp->h);
	else
		masked_blit(its_walk_bmp,its_buf,0,0,its_x-its_walk_bmp->w/2,its_y-its_walk_bmp->h/2-20,its_walk_bmp->w,its_walk_bmp->h);
}
void BucketZombie::load_picture()
{
	its_attack_bmp=load_bitmap("bucket_zombie_attack.bmp",0);
	its_walk_bmp=load_bitmap("bucket_zombie_walk.bmp",0);
}

BITMAP *Bullet::its_bmp=0;

Bullet::Bullet(int x,int y,int attack)
	  : ObjectOnRoad(x,y,0,bullet_object_type)
{
	its_attack=attack;
	its_speed=0.3;
	its_attack_zone=10;
	its_double_x=x;
}

bool Bullet::act(int timelen)
{
	Zombie* p;
	p=(Zombie*)RoadGroup::bullet_search_zombie(its_road_index,its_x,its_attack_zone);
	if(p!=0)
	{
		if(p->under_attack(its_attack)==true)//如果僵尸被打死
			RoadGroup::remove(p);

		return true;//通知主函数 删掉自己
	}
	else
	{
		its_double_x=its_double_x+its_speed*timelen;
		its_x=(int)its_double_x;

		if(its_x>850)//如果已经飞出右边屏幕很多  
		{
			return true;//通知主函数 删掉自己
		}
	}
	return false;
}
void Bullet::load_picture()
{
	its_bmp=load_bitmap("bullet.bmp",0);
}
void Bullet::draw()
{
	masked_blit(its_bmp,its_buf,0,0,its_x-its_bmp->w/2,its_y-its_bmp->h/2,its_bmp->w,its_bmp->h);
}
bool Bullet::on_put_down()
{
	return true;//没用， 保留
}


BITMAP *Mouse::its_bmp=0;
Object*	Mouse::its_current_object=0;

void Mouse::drag(Object* current_object)
{
	its_current_object=current_object;
}

void Mouse::put_down()
{
    if(its_current_object->on_put_down()==true)
	{
		delete its_current_object;
	}
	its_current_object=0;
}

bool Mouse::is_drag()
{
	if(its_current_object!=0)
		return true;
	else
		return false;
}

void Mouse::init()
{
	its_current_object=0;
}
void Mouse::load_picture()
{
	its_bmp=load_bitmap("mouse.bmp",0);
}
void Mouse::draw()
{
	if(is_drag())
	{
		its_current_object->set_position(mouse_x,mouse_y);
		its_current_object->draw();
	}
	masked_blit(its_bmp,its_buf,0,0,mouse_x,mouse_y,its_bmp->w,its_bmp->h);
}


BITMAP *Spade::its_bmp=0;

bool Spade::on_put_down()
{
	Plant* p=(Plant*)RoadGroup::spade_search_plant(mouse_x,mouse_y);
	if(p!=0)
		RoadGroup::remove(p);
	return true;
}
void Spade::load_picture()
{
	its_bmp=load_bitmap("spade.bmp",0);
}
void Spade::draw()
{
	blit(its_bmp,its_buf,0,0,mouse_x-its_bmp->w/2,mouse_y-its_bmp->h/2,its_bmp->w,its_bmp->h);
}

class Background:public Draw
{
public:
	static void load_picture();
	static void draw(bool is_win);
private:
	static BITMAP *its_win_bmp;
	static BITMAP *its_lose_bmp;
	static BITMAP *its_che;
};

BITMAP *Background::its_win_bmp=0;
BITMAP *Background::its_lose_bmp=0;
BITMAP *Background::its_che=0;

void Background::load_picture()
{
	its_win_bmp=load_bitmap("background_win.bmp",0);
	its_lose_bmp=load_bitmap("background_lose.bmp",0);
	its_che=load_bitmap("background_check.bmp",0);
}
void Background::draw(bool is_win)
{
	if(is_win)
	{
		blit(its_win_bmp,its_buf,0,0,0,0,its_win_bmp->w,its_win_bmp->h);
		blit(its_che,its_buf_check,0,0,0,0,its_che->w,its_che->h);
	}
	else
	{
		blit(its_lose_bmp,its_buf,0,0,0,0,its_lose_bmp->w,its_lose_bmp->h);
		blit(its_che,its_buf_check,0,0,0,0,its_che->w,its_che->h);
	}
}
#endif