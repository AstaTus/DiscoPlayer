#ifndef __PLAYER_STATE_ENUM_H__
#define __PLAYER_STATE_ENUM_H__

enum class PlayerStateEnum {
    INIT,
    PREPARING, //播放器开始创建各种对象，拉视频数据解码变换等
    PREPARED, //frame decode finished
    PLAYING, //播放中
    PAUSED, //用户暂停
    COMPLETED, //播放完成
    SEEKING, //SEEK
    //TODO
    BUFFERING, //当前无帧可以播放-音频和视频帧差的较大时也走这个状态
    STOPPED, //播放器停止？？？
    ERROR, //播放出错
    RELEASING, //播放器开始释放各种对象
    END //播放器释放各种对象完成
};
#endif