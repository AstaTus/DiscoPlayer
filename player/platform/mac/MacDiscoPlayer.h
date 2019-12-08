#ifndef __MAC_DISCO_PLAYER_H__
#define __MAC_DISCO_PLAYER_H__

#include "../../common/IDiscoPlayer.h"
#include "../../common/PlayItem.h"

#include "../../../stream/MediaInputStream.h"
#include "../../../codec/MediaDecoder.h"
#include "../../../render/video/VideoRender.h"
#include "../../../render/video/IRenderView.h"


class MacDiscoPlayer : public IDiscoPlayer
{
private:
    
    MediaDecoder * pMediaDecoder;
    VideoRender * pVideoRender;

    IRenderView * pRenderView;
    MediaInputStream * pInputStream;

    PlayItem * pCurrentPlayItem;

public:
    MacDiscoPlayer(/* args */);
    ~MacDiscoPlayer();

    void set_render_surface(IRenderView * render_view);

    //set 与 replace 系列函数的不同之处是会影响播放状态
    void set_play_item(PlayItem * play_item);

    void replace_play_item(const PlayItem * const play_item);

    void start();

    void pause();

    void resume();

    void stop();
};

#endif