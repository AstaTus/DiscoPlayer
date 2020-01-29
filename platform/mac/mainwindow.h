#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
class OpenGLRenderWidget;
class CorePlayer;
class AudioDevice;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
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
    void start();
    void pause();
    void resume();

    QString formatTime(int ms);

    QTimer *mpTimer;
    Ui::MainWindow *ui;
    OpenGLRenderWidget* mpOpenGLRenderWidget;
    QLabel * mpProgressLabel;
    CorePlayer* mpCorePlayer;
    AudioDevice* mpAudioDevice;
private slots:
    void PlayProgressTimerTimeOut();
};
#endif // MAINWINDOW_H
