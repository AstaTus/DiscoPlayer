#include "MacDiscoPlayer.h"
#include "SDLRenderView.h"

MacDiscoPlayer::MacDiscoPlayer(/* args */)
: pMediaDecoder(NULL),
pVideoRender(NULL),
pRenderView(NULL),
pInputStream(NULL)
{
}

MacDiscoPlayer::~MacDiscoPlayer()
{
}

void MacDiscoPlayer::set_play_item(PlayItem * play_item)
{
    if (pInputStream == NULL && play_item->get_data_source().size() > 0)
    {
        pCurrentPlayItem = play_item;
        pInputStream = new MediaInputStream();
    } else {

    }
}

void MacDiscoPlayer::replace_play_item(const PlayItem * const play_item)
{
    
}


void MacDiscoPlayer::set_render_surface(IRenderView * render_view)
{
    if (pRenderView == NULL)
    {
        pRenderView = render_view;
    } else {

    }
}

void MacDiscoPlayer::start()
{
    if (pRenderView == NULL || pInputStream == NULL || pCurrentPlayItem == NULL)
    {
        //throw exception
    } else {
        pMediaDecoder = new MediaDecoder(pInputStream);
    }
    
    


}

void MacDiscoPlayer::pause()
{

}

void MacDiscoPlayer::resume()
{

}

void MacDiscoPlayer::stop()
{

}