#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QSlider>
#include "debugwidget.h"
#include "../../player/CorePlayerStateChangedListener.h"
#include <qevent.h>
#include <QPushButton>
#include <QAction>
class OpenGLRenderWidget;
class CorePlayer;
class AudioDevice;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public CorePlayerStateChangedListener
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void onPlayAndPauseButtonClicked();
    void keyPressEvent(QKeyEvent *ev) override;

private:
    void init();
    void start(const std::string & path);
    void pause();
    void resume();

    void customEvent(QEvent * event) override;

    void on_state_changed(PlayerStateEnum state, CorePlayer * player) override;

    void on_handle_player_state_prepared();

    void on_handle_player_state_playing();

    void on_handle_player_state_pause();

    QString formatTime(int64_t ms, bool has_msec = false);

    QTimer *mpTimer;
    QTimer *mpDebugTimer;
    Ui::MainWindow *ui;
    OpenGLRenderWidget* mpOpenGLRenderWidget;
    DebugWidget * mpDebugWidget;
    QLabel * mpProgressLabel;
    QPushButton * mpPlayAndPauseBtn;

    QAction *mpOpenFileAction;

    CorePlayer* mpCorePlayer;
    AudioDevice* mpAudioDevice;

    QSlider *mpSeekBar;
    PlayerStateEnum mLastPlayerState;

    bool mIsSeeking;
private slots:
    void refresh_play_progress_time();
    void refresh_debug_info();
    void seek_start();
    void seek_end();
    void open_video();
};
#endif // MAINWINDOW_H
