#include <allegro.h>
#pragma comment(lib,"alleg.lib")  //连接这个库  不然链接时报错

#include "sit.h"

void my_mouse_callback(int flags)
{
	g_p_cur_sit->mouse_callback(flags);
}
int main(void)
{
	//初始化allegro
	allegro_init();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,800,600,0,0);


	do_all_preparation_work();

	g_p_gaming_sit->init();
	g_p_cur_sit=g_p_gaming_sit;

	//让鼠标好使
	install_mouse();
	show_mouse(NULL);
	mouse_callback=my_mouse_callback;

	TimeCounter tc;

	while(is_should_exit==false)
	{
		g_p_cur_sit->draw();
		g_p_cur_sit->act(tc.get_timelen());
	}
	return 0;
}
END_OF_MAIN()