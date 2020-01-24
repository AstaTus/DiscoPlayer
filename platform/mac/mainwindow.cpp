#include "mainwindow.h"
#include "./resources/ui_mainwindow.h"

#include<iostream>
#include "SDL.h"
#include <errno.h>
#include <unistd.h>

#include "SDLRenderView.h"
#include "SDLAudioDevice.h"
#include "../../player/CorePlayer.h"
#include "../../player/PlayItem.h"
#include "mainwindow.h"

#include "QRenderWidget.h"
#include "OpenGLRenderWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
const static int SCREEN_WIDTH = 1080;
const static int SCREEN_HEIGHT = 720;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    mpOpenGLRenderWidget = new OpenGLRenderWidget();
    QVBoxLayout * layout = findChild<QVBoxLayout*>("verticalLayout");
    layout->insertWidget(0, mpOpenGLRenderWidget, 16);
    // MainWindow 是程序帮我创建一个窗口类。所有继承QWidget类都是窗口类。
    mpOpenGLRenderWidget->resize(1920, 1080);

    QPushButton * button = findChild<QPushButton *>("pushButton");
    connect(button, &QPushButton::clicked, this, &MainWindow::onPlayAndPauseButtonClicked);
}

void MainWindow::start()
{
    // string video_path1 = "/Users/laoganbu/happy/movie/阳光电影www.ygdy8.com.小丑.HD.720p.韩版英语中字.mkv";
    string video_path1 = "/Users/laoganbu/learn/ffmpeg/改名字才让分享啊课程视频/改名字才让分享啊课件资料/代码/(5) 参考答案-FFmpeg+SDL视频播放器/屌丝男士.mov";
    
    mpCorePlayer = new CorePlayer();
    mpAudioDevice = new SDLAudioDevice();
    
    mpCorePlayer->set_render_surface(mpOpenGLRenderWidget);
    mpCorePlayer->set_audio_device(mpAudioDevice);
 
    PlayItem * play_item = new PlayItem(video_path1);
    mpCorePlayer->set_play_item(play_item);

    mpCorePlayer->start();
}

void MainWindow::onPlayAndPauseButtonClicked()
{
    if (true)
    {
        pause();
    }
    
}
void MainWindow::pause()
{
    mpCorePlayer->pause();
}

void MainWindow::resume()
{
    mpCorePlayer->resume();
}

