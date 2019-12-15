#define __STD_CONSTANT_MACROS

#include<iostream>
#include "SDL2/SDL.h"
#include <errno.h>
#include <unistd.h>

using namespace std;

#include "SDLRenderView.h"
#include "../../CorePlayer.h"
#include "../../PlayItem.h"

const static int SCREEN_WIDTH = 640;
const static int SCREEN_HEIGHT = 360;

int main()
{
    SDL_Window *screen = NULL;
    SDL_Renderer *sdl_renderer = NULL;
    SDL_Event event;

    CorePlayer * core_player = NULL;

    string video_path1 = "/Users/laoganbu/learn/ffmpeg/改名字才让分享啊课程视频/改名字才让分享啊课件资料/代码/(5) 参考答案-FFmpeg+SDL视频播放器/屌丝男士.mov";


	//SDL 2.0 Support for multiple windows
	screen = SDL_CreateWindow("Simplest Video Play SDL2", 
                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

    sdl_renderer = SDL_CreateRenderer(screen, -1, 0);

    RenderView * render_view = new SDLRenderView(sdl_renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    core_player = new CorePlayer();
    core_player->set_render_surface(render_view);

    PlayItem * play_item = new PlayItem(video_path1);
    core_player->set_play_item(play_item);

    core_player->start();
    int ret = 0;
    while (1)
    {
        ret = SDL_PollEvent(&event);
        if(ret == 1 && event.type == SDL_QUIT)
        {
            break;
        }
    }

    delete render_view;
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(screen);
    delete core_player;
    delete play_item;
	SDL_Quit();
}