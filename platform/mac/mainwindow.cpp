#include "mainwindow.h"
#include "./resources/ui_mainwindow.h"

#include <iostream>
#include "SDL.h"
#include <errno.h>
#include <unistd.h>

#include "SDLRenderView.h"
#include "SDLAudioDevice.h"
#include "../../player/CorePlayer.h"
#include "../../player/PlayItem.h"
#include "../../player/DebugInfo.h"
#include "mainwindow.h"

#include "QRenderWidget.h"
#include "OpenGLRenderWidget.h"
#include <QVBoxLayout>

#include <QTime>
#include <QKeyEvent>
#include <QStackedLayout>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include "PlayrerStateChangedEvent.h"

const static int SCREEN_WIDTH = 1080;
const static int SCREEN_HEIGHT = 720;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      mpProgressLabel(nullptr),
      mpTimer(nullptr), ui(new Ui::MainWindow),
      mpSeekBar(nullptr),
      mpVolumeBar(nullptr),
      mLastPlayerState(PlayerStateEnum::INIT),
      mIsSeeking(false)
{
    ui->setupUi(this);
    this->grabKeyboard();
    init();
    string video_path1 = "/Users/laoganbu/happy/movie/阳光电影www.ygdy8.com.地久天长.HD.720p.国语中字.mkv";
    start(video_path1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Right)
    {
        int64_t position = mpCorePlayer->get_current_position() + 3000;
        if (position > mpCorePlayer->get_duration())
        {
            position = mpCorePlayer->get_duration();
        }
        mpCorePlayer->seek(position);
        return;
    }
    else if (ev->key() == Qt::Key_Left)
    {
        int64_t position = mpCorePlayer->get_current_position() - 3000;
        if (position < 0)
        {
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
    //定时器触发信号槽
    connect(mpTimer, SIGNAL(timeout()), this, SLOT(refresh_play_progress_time()));

    mpDebugTimer = new QTimer();
    mpDebugTimer->setSingleShot(false);
    connect(mpDebugTimer, SIGNAL(timeout()), this, SLOT(refresh_debug_info()));

    QVBoxLayout *layout = findChild<QVBoxLayout *>("verticalLayout");
    mpProgressLabel = findChild<QLabel *>("label");
    mpSeekBar = findChild<QSlider *>("seekBar");
    connect(mpSeekBar, SIGNAL(sliderReleased()), this, SLOT(seek_end()));
    connect(mpSeekBar, SIGNAL(sliderPressed()), this, SLOT(seek_start()));

    mpVolumeBar = findChild<QSlider *>("volumeBar");
    connect(mpVolumeBar, SIGNAL(valueChanged(int)), this, SLOT(on_volume_changed(int)));

    QStackedLayout *stacked_layout = new QStackedLayout(this);
    mpOpenGLRenderWidget = new OpenGLRenderWidget(this);
    mpDebugWidget = new DebugWidget(this);
    QPalette debug_palette(mpDebugWidget->palette());
    debug_palette.setColor(QPalette::Background, QColor(128, 128, 128, 128));
    mpDebugWidget->setAutoFillBackground(true);
    mpDebugWidget->setPalette(debug_palette);
    stacked_layout->setStackingMode(QStackedLayout::StackingMode::StackAll);
    stacked_layout->addWidget(mpOpenGLRenderWidget);
    stacked_layout->addWidget(mpDebugWidget);

    mpOpenFileAction = findChild<QAction *>("action_open_video");
    connect(mpOpenFileAction, &QAction::triggered, this, &MainWindow::open_video);

    layout->insertLayout(0, stacked_layout, 16);
    // MainWindow 是程序帮我创建一个窗口类。所有继承QWidget类都是窗口类。
    mpOpenGLRenderWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mpOpenGLRenderWidget->resize(1920, 1080);

    mpPlayAndPauseBtn = findChild<QPushButton *>("pushButton");
    mpPlayAndPauseBtn->setToolTip(tr("Pause"));
    mpPlayAndPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    connect(mpPlayAndPauseBtn, &QPushButton::clicked, this, &MainWindow::onPlayAndPauseButtonClicked);
}

void MainWindow::start(const std::string &path)
{
    // string video_path1 = "/Users/laoganbu/learn/ffmpeg/改名字才让分享啊课程视频/改名字才让分享啊课件资料/代码/(5) 参考答案-FFmpeg+SDL视频播放器/屌丝男士.mov";

    mpCorePlayer = new CorePlayer();
    mpAudioDevice = new SDLAudioDevice();

    mpCorePlayer->set_render_surface(mpOpenGLRenderWidget);
    mpCorePlayer->set_audio_device(mpAudioDevice);
    mpCorePlayer->set_player_state_change_listener(this);

    PlayItem *play_item = new PlayItem(path);
    mpCorePlayer->set_play_item(play_item);

    mpCorePlayer->start();
}

void MainWindow::onPlayAndPauseButtonClicked()
{
    if (mpCorePlayer->get_current_play_state() == PlayerStateEnum::PLAYING)
    {
        pause();
    }
    else if (mpCorePlayer->get_current_play_state() == PlayerStateEnum::PAUSED)
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

void MainWindow::refresh_debug_info()
{
    if (mpCorePlayer != nullptr && mpDebugWidget != nullptr)
    {
        const DebugInfo *debugInfo = mpCorePlayer->get_debug_info();
        //TODO 更新数据
        mpDebugWidget->refresh(debugInfo->video_pts,
                               debugInfo->audio_pts,
                               formatTime(debugInfo->video_time, true),
                               formatTime(debugInfo->audio_time, true));
    }
}

void MainWindow::refresh_play_progress_time()
{
    if (mpCorePlayer != nullptr && mpProgressLabel != nullptr)
    {
        QString duration_str = formatTime(mpCorePlayer->get_duration());
        QString current_pos_str = formatTime(mpCorePlayer->get_current_position());
        mpProgressLabel->setText(current_pos_str + " / " + duration_str);

        if (!mIsSeeking)
        {
            mpSeekBar->setValue(mpCorePlayer->get_current_position() / 1000);
        }
    }
}

void MainWindow::on_state_changed(PlayerStateEnum state, CorePlayer *player)
{
    QApplication::postEvent(this, new PlayrerStateChangedEvent(state, player));
}

void MainWindow::on_handle_player_state_prepared()
{
    mpSeekBar->setOrientation(Qt::Horizontal);                  // 水平方向
    mpSeekBar->setMinimum(0);                                   // 最小值
    mpSeekBar->setMaximum(mpCorePlayer->get_duration() / 1000); // 最大值
    mpSeekBar->setSingleStep(1);                                // 步长

    mpVolumeBar->setMinimum(0);
    mpVolumeBar->setMaximum(mpCorePlayer->get_max_volume());
    mpVolumeBar->setSingleStep(1);
    mpVolumeBar->setValue(mpCorePlayer->get_volume());
}

void MainWindow::on_handle_player_state_playing()
{
    mpPlayAndPauseBtn->setToolTip(tr("Pause"));
    mpPlayAndPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
}

void MainWindow::on_handle_player_state_pause()
{
    mpPlayAndPauseBtn->setToolTip(tr("Play"));
    mpPlayAndPauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}

void MainWindow::seek_start()
{
    mIsSeeking = true;
}

void MainWindow::seek_end()
{
    mIsSeeking = false;
    mpCorePlayer->seek(mpSeekBar->value() * 1000);
}

void MainWindow::on_volume_changed(int value)
{
    mpCorePlayer->set_volume(value);
}

void MainWindow::open_video()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("打开视频文件"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    //fileDialog->setNameFilter(tr("Images(*.png *.jpg *.jpeg *.bmp)"));
    fileDialog->setNameFilter(tr("video(*.mp4 *.mkv *.avi *.mov *)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    QStringList fileNames;
    if (fileDialog->exec() == QFileDialog::Accepted)
    {
        mpCorePlayer->stop();
        mpCorePlayer = nullptr;
        fileNames = fileDialog->selectedFiles();
        start(fileNames.at(0).toStdString());
    }
}

void MainWindow::customEvent(QEvent *event)
{
    if (event->type() == PlayrerStateChangedEvent::sType)
    { // It must be a ColorChangeEvent
        PlayrerStateChangedEvent *ce = dynamic_cast<PlayrerStateChangedEvent *>(event);
        PlayerStateEnum state = ce->get_state();

        if (state == PlayerStateEnum::PREPARED)
        {
            on_handle_player_state_prepared();
        }
        else if (state == PlayerStateEnum::PLAYING)
        {
            mpTimer->start(1000);
            mpDebugTimer->start(1);
            on_handle_player_state_playing();
        }
        else if (state == PlayerStateEnum::PAUSED)
        {
            on_handle_player_state_pause();
        }
        else if (state == PlayerStateEnum::STOPPED)
        {
            mpTimer->stop();
            mpDebugTimer->stop();
            /* code */
        }

        else if (state == PlayerStateEnum::END)
        {
            CorePlayer *player = ce->get_core_player();
            delete player;
        }

        mLastPlayerState = state;
    }
}

QString MainWindow::formatTime(int64_t ms, bool has_msec)
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

    QString hou = QString::number(hour, 10);
    QString min = QString::number(minute, 10);
    QString sec = QString::number(second, 10);
    QString msec = QString::number(milliSecond, 10);

    //qDebug() << "minute:" << min << "second" << sec << "ms" << msec <<endl;
    if (has_msec)
    {
        return hou + ":" + min + ":" + sec + "." + msec;
    }
    else
    {
        return hou + ":" + min + ":" + sec;
    }
}
