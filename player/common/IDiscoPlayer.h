#ifndef __IDISCO_PLAYER_H__
#define __IDISCO_PLAYER_H__

#include <string>
using namespace std;

class IRenderView;
class PlayItem;

class IDiscoPlayer
{
private:
    /* data */
public:
    virtual void set_render_surface(const IRenderView * const render_view) = 0;

    virtual void set_play_item(PlayItem * play_item) = 0;

    virtual void replace_play_item(const PlayItem * const play_item) = 0;
    
    //TODO 预加载PlayItem

    virtual void start() = 0;

    virtual void pause() = 0;

    virtual void resume() = 0;

    virtual void stop() = 0;
};

#endif