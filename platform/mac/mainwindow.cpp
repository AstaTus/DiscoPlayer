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
#include <QTime>
#include <QKeyEvent>
const static int SCREEN_WIDTH = 1080;
const static int SCREEN_HEIGHT = 720;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    mpProgressLabel(nullptr),
    mpTimer(nullptr)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->grabKeyboard();
    init();
    start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Right)
    {
        int64_t position = mpCorePlayer->get_current_position() + 3000;
        if(position > mpCorePlayer->get_duration()) {
            position = mpCorePlayer->get_duration();
        }
       mpCorePlayer->seek(position);
       return;
    } else if (ev->key() == Qt::Key_Left)
    {
        int64_t position = mpCorePlayer->get_current_position() - 3000;
        if(position < 0) {
            position = 0;
        }
       mpCorePlayer->seek(position);
        return;
    }

    QWidget::keyPressEvent(ev);
}

void MainWindow::init()
{
    mpTimer = new QTimer();
    mpTimer->setSingleShot(false);
    //启动或重启定时器, 并设置定时器时间：毫秒
    mpTimer->start(1000);
    //定时器触发信号槽
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(PlayProgressTimerTimeOut()));

    mpOpenGLRenderWidget = new OpenGLRenderWidget();
    QVBoxLayout * layout = findChild<QVBoxLayout*>("verticalLayout");
    mpProgressLabel = findChild<QLabel*>("label");
    layout->insertWidget(0, mpOpenGLRenderWidget, 16);
    // MainWindow 是程序帮我创建一个窗口类。所有继承QWidget类都是窗口类。
    mpOpenGLRenderWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mpOpenGLRenderWidget->resize(1920, 1080);

    QPushButton * button = findChild<QPushButton *>("pushButton");
    connect(button, &QPushButton::clicked, this, &MainWindow::onPlayAndPauseButtonClicked);
}

void MainWindow::start()
{
    string video_path1 = "/Users/laoganbu/happy/movie/阳光电影www.ygdy8.com.地久天长.HD.720p.国语中字.mkv";
    // string video_path1 = "/Users/laoganbu/learn/ffmpeg/改名字才让分享啊课程视频/改名字才让分享啊课件资料/代码/(5) 参考答案-FFmpeg+SDL视频播放器/屌丝男士.mov";
    
    mpCorePlayer = new CorePlayer();
    mpAudioDevice = new SDLAudioDevice();
    
    mpCorePlayer->set_render_surface(mpOpenGLRenderWidget);
    mpCorePlayer->set_audio_device(mpAudioDevice);
 
    PlayItem * play_item = new PlayItem(video_path1);
    mpCorePlayer->set_play_item(play_item);

    mpCorePlayer->start();
    mpCorePlayer->resume();
}

void MainWindow::onPlayAndPauseButtonClicked()
{
    if (mpCorePlayer->get_current_play_state() == StateManager::PlayState::PLAYING)
    {
        pause();
    }
    else if (mpCorePlayer->get_current_play_state() == StateManager::PlayState::PAUSED)
    {
        resume();
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

void MainWindow::PlayProgressTimerTimeOut()
{
    if (mpCorePlayer != nullptr && mpProgressLabel != nullptr)
    {
        QString duration_str = formatTime(mpCorePlayer->get_duration());
        QString current_pos_str = formatTime(mpCorePlayer->get_current_position());
        mpProgressLabel->setText(current_pos_str + " / " + duration_str);
    }
}

QString MainWindow::formatTime(int ms)
{
    int ss = 1000;
    int mi = ss * 60;
    int hh = mi * 60;
    int dd = hh * 24;
 
    long day = ms / dd;
    long hour = (ms - day * dd) / hh;
    long minute = (ms - day * dd - hour * hh) / mi;
    long second = (ms - day * dd - hour * hh - minute * mi) / ss;
    long milliSecond = ms - day * dd - hour * hh - minute * mi - second * ss;
 
    QString hou = QString::number(hour,10);
    QString min = QString::number(minute,10);
    QString sec = QString::number(second,10);
    QString msec = QString::number(milliSecond,10);
 
    //qDebug() << "minute:" << min << "second" << sec << "ms" << msec <<endl;
 
    return hou + ":" + min + ":" + sec ;
}

