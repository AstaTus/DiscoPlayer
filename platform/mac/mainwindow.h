#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
private:

    void init();
    void start();
    void pause();
    void resume();

    Ui::MainWindow *ui;
    OpenGLRenderWidget* mpOpenGLRenderWidget;
    CorePlayer* mpCorePlayer;
    AudioDevice* mpAudioDevice;
};
#endif // MAINWINDOW_H
