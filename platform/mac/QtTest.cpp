#define __STD_CONSTANT_MACROS

#include<iostream>
#include "SDL.h"
#include <errno.h>
#include <unistd.h>
#include <QApplication>
#include "mainwindow.h"
using namespace std;


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    //有且只有一个应用程序类的对象
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    // a.exec(); 让程序一直执行，等待用户操作，也就是等待事件操作。
    return a.exec();
}