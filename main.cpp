#include "mainwindow.h"

#include <QApplication>
#include <QFile>

#include "hotfolder.h"

QString appStyle = "\
QStatusBar{\
    background-color:qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 rgba(25,25,25,127),stop:1 rgba(53,53,53,75));\
    color:palette(mid);\
}\
QMainWindow::separator{\
    width:6px;\
    height:5px;\
    padding:2px;\
}\
QWidget {\
    background-color: #323232;\
    color: #FFFFFF;\
}\
QTabWidget::pane{\
    background-color:rgba(66,66,66,50%);\
    border:1px solid rgba(25,25,25,50%);\
}\
QTabWidget::tab-bar{\
    left:3px;\
    top:1px;\
}\
QTabBar{\
    background-color:transparent;\
    qproperty-drawBase:0;\
    border-bottom:1px solid rgba(25,25,25,50%);\
}\
QTabBar::tab{\
    padding:4px 6px;\
    background-color:qlineargradient(x1:0,y1:1,x2:0,y2:0,stop:0 rgba(25,25,25,127),stop:1 rgba(53,53,53,75));\
    border:1px solid rgba(25,25,25,75);\
    border-top-left-radius:4px;\
    border-top-right-radius:4px;\
}\
QTabBar::tab:selected,QTabBar::tab:hover{\
    background-color:qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 rgba(53,53,53,127),stop:1 rgba(66,66,66,50%));\
    border-bottom-color:rgba(66,66,66,75%);\
}";

hotfolder h = hotfolder();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // apply theme
    QFile file(":/style/qtdark");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
    }
    app.setStyleSheet(appStyle);

    MainWindow w;
//    w.setWindowFlags(Qt::FramelessWindowHint);
    w.show();
    h.start();
    return app.exec();
}
