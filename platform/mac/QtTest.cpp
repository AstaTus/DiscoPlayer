#define __STD_CONSTANT_MACROS

#include<iostream>
#include "SDL.h"
#include <errno.h>
#include <unistd.h>
#include <QApplication>

using namespace std;

#include "SDLRenderView.h"
#include "SDLAudioDevice.h"
#include "../../player/CorePlayer.h"
#include "../../player/PlayItem.h"

#include "QRenderWidget.h"
#include "OpenGLRenderWidget.h"
const static int SCREEN_WIDTH = 1080;
const static int SCREEN_HEIGHT = 720;

int main(int argc, char *argv[])
{
    //有且只有一个应用程序类的对象
    QApplication a(argc, argv);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    // MainWindow 是程序帮我创建一个窗口类。所有继承QWidget类都是窗口类。
    OpenGLRenderWidget w;
    // a.setMainWidget( &w );
    w.resize(1920, 1080);
    //显示窗口
    w.show();

    CorePlayer * core_player = NULL;
    // string video_path1 = "/Users/laoganbu/happy/movie/阳光电影www.ygdy8.com.小丑.HD.720p.韩版英语中字.mkv";
    string video_path1 = "/Users/laoganbu/learn/ffmpeg/改名字才让分享啊课程视频/改名字才让分享啊课件资料/代码/(5) 参考答案-FFmpeg+SDL视频播放器/屌丝男士.mov";
    
    core_player = new CorePlayer();
    // RenderView * render_view = new SDLRenderView(sdl_renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    AudioDevice * audio_device = new SDLAudioDevice();
    
    core_player->set_render_surface(&w);
    core_player->set_audio_device(audio_device);
 
    PlayItem * play_item = new PlayItem(video_path1);
    core_player->set_play_item(play_item);

    core_player->start();

    // a.exec(); 让程序一直执行，等待用户操作，也就是等待事件操作。
    return a.exec();
}