#define __STD_CONSTANT_MACROS

#include<iostream>
#include "SDL2/SDL.h"
#include <errno.h>
#include <unistd.h>

extern "C"
{
    #include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
	#include "libavutil/imgutils.h"
}

using namespace std;

const static int SCREEN_WIDTH = 640;
const static int SCREEN_HEIGHT = 360;

const static int PIXEL_WIDTH = 640;
const static int PIXEL_HEIGHT = 360;

/* TODO: what's BPP????
 * 
 */

AVFormatContext	*pFormatCtx = 0;
AVCodecContext	*pCodecCtx = 0;
AVCodec			*pCodec = 0;
AVFrame	*pFrame = 0, *pFrameYUV = 0;
struct SwsContext *img_convert_ctx = 0;
uint8_t *out_buffer = 0;
AVPacket *pPacket = 0;
int ret, got_picture;

SDL_Thread *video_tid;

const static int BPP = 12;
const static int TEXTURE_SIZE = PIXEL_WIDTH * PIXEL_HEIGHT * BPP / 8;
unsigned char buffer[TEXTURE_SIZE];
string video_path = "/Users/laoganbu/Movies/vlog.mp4";
string video_path1 = "/Users/laoganbu/learn/ffmpeg/改名字才让分享啊课程视频/改名字才让分享啊课件资料/代码/(5) 参考答案-FFmpeg+SDL视频播放器/屌丝男士.mov";

int thread_exit=0;
int thread_pause=0;

SDL_Event event;

//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)

#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)

int sfp_refresh_thread(void *opaque);

int main()
{
	cout << avcodec_configuration() << endl;
    if(SDL_Init(SDL_INIT_VIDEO)) 
    {  
		printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
		return -1;
	}

	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&pFormatCtx, video_path1.c_str(), 0, 0) != 0){
		return -1;
	}

	if (avformat_find_stream_info(pFormatCtx, 0) < 0){
		return -1;
	}

	AVCodec *video_codec = 0;
	int video_index = -1;
	for(int i = 0; i < pFormatCtx->nb_streams; ++i){
		AVCodec *codec = avcodec_find_decoder(pFormatCtx->streams[i]->codecpar->codec_id);
		if (codec != 0)
		{
			if(codec->type == AVMEDIA_TYPE_VIDEO)
			{
				video_codec = codec;
				video_index = i;

				break;
			}
		}
	}

	if(video_codec == 0)
	{
		return -1;
	}

	pCodecCtx = avcodec_alloc_context3(video_codec);
	if (pCodecCtx == 0)
	{
		return -1;
	}
	avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[video_index]->codecpar);

	if(avcodec_open2(pCodecCtx, video_codec, NULL) < 0){
			return -1;
	}

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	//TODO  align 1 ???
	out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, 
		pCodecCtx->width, pCodecCtx->height, 1));
	
	//将FrameYUV的data指向out_buffer
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P, 
		pCodecCtx->width, pCodecCtx->height, 1);
	

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
			pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    SDL_Window *screen = NULL; 
	//SDL 2.0 Support for multiple windows
	screen = SDL_CreateWindow("Simplest Video Play SDL2", 
                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		            pCodecCtx->width, pCodecCtx->height, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	if(!screen) {  
		printf("SDL: could not create window - exiting:%s\n",SDL_GetError());  
		return -1;
	}

    Uint32 pixformat = 0;
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	pixformat = ;

    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(screen, -1, 0);  
	SDL_Texture *sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat, 
                                SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);

	SDL_Rect sdlRect;  
	sdlRect.x = 0;  
	sdlRect.y = 0;  
	sdlRect.w = pCodecCtx->width;  
	sdlRect.h = pCodecCtx->height;  
	pPacket = (AVPacket *)av_malloc(sizeof(AVPacket));

	video_tid = SDL_CreateThread(sfp_refresh_thread,NULL,NULL);

	while(1)
	{
		SDL_WaitEvent(&event);
		if(event.type == SFM_REFRESH_EVENT)
		{
			if (av_read_frame(pFormatCtx, pPacket) >= 0)
			{
				if (pPacket->stream_index == video_index)
				{
					ret = avcodec_send_packet(pCodecCtx, pPacket);
					if(ret != 0){
						break;
					}
					got_picture = avcodec_receive_frame(pCodecCtx, pFrame);
					if (got_picture != 0)
					{
						// SDL_ShowMessageBox()
					} 
					else
					{
						sws_scale(img_convert_ctx, 
							(const uint8_t* const*)pFrame->data, pFrame->linesize, 0, 
							pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

						SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
						SDL_RenderClear(sdlRenderer);
						SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, &sdlRect);  
						SDL_RenderPresent( sdlRenderer ); 
						// TRACE("Decode 1 frame\n");
					}
				}
				av_packet_unref(pPacket);
			}else{
				thread_exit = 1;
			}
		}else if(event.type == SDL_QUIT){
			thread_exit = 1;
		}else if(event.type == SFM_BREAK_EVENT){
			break;
		}
	}
	sws_freeContext(img_convert_ctx);
	SDL_DestroyWindow(screen);
	SDL_Quit();

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;

    // FILE *fp = NULL;
	// fp = fopen("/Users/laoganbu/project/player/sintel_640_360.yuv", "rb+");

	// if(fp == NULL)
    // {
	// 	printf("cannot open this file\n");
	// 	printf("fopen: %s\n", strerror(errno));
	// 	char buf[80];   
    //   	getcwd(buf,sizeof(buf));   
    //   	printf("current working directory: %s\n", buf);


	// 	return -1;
	// }

    // SDL_Rect sdlRect;  

    // while(1)
    // {
	// 		if (fread(buffer, 1, TEXTURE_SIZE, fp) != TEXTURE_SIZE){
	// 			// Loop
	// 			fseek(fp, 0, SEEK_SET);
	// 			fread(buffer, 1, TEXTURE_SIZE, fp);
	// 		}

	// 		SDL_UpdateTexture( sdlTexture, NULL, buffer, PIXEL_WIDTH);  

	// 		sdlRect.x = 0;  
	// 		sdlRect.y = 0;  
	// 		sdlRect.w = SCREEN_WIDTH;  
	// 		sdlRect.h = SCREEN_HEIGHT;  
			
	// 		SDL_RenderClear( sdlRenderer );   
	// 		SDL_RenderCopy( sdlRenderer, sdlTexture, NULL, &sdlRect);  
	// 		SDL_RenderPresent( sdlRenderer );  
	// 		//Delay 40ms
	// 		SDL_Delay(40);
			
	// }
	// SDL_Quit();

    // return 0;
}


int sfp_refresh_thread(void *opaque){

	thread_exit=0;
	thread_pause=0;

	while (thread_exit == 0) {
		if(!thread_pause){
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);
		}
		SDL_Delay(5);
	}
	//Quit
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);
	thread_exit=0;
	thread_pause=0;
	return 0;
}