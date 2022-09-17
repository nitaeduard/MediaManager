QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    eimageviewer.cpp \
    exif.cpp \
    heifreader.cpp \
    hotfolder.cpp \
    main.cpp \
    mainwindow.cpp \
    mmfilemodel.cpp

HEADERS += \
    eimageviewer.h \
    exif.h \
    heifreader.h \
    hotfolder.h \
    mainwindow.h \
    mmfilemodel.h

FORMS += \
    mainwindow.ui

config_libheif {
    unix|win32-g++*: LIBS += -lheif
    else:win32: LIBS += libheif.lib
}

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../vcpkg/installed/x64-windows/lib/ -lheif
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../vcpkg/installed/x64-windows/debug/lib/ -lheif
#else:unix: LIBS += -L$$OUT_PWD/../../../projects/mylib/ -lmylib

INCLUDEPATH += $$PWD/../vcpkg/installed/x64-windows/include
LIBS += -L$$OUT_PWD/../vcpkg/installed/x64-windows/debug/lib/ -lheif
DEPENDPATH += $$PWD/../../vcpkg/installed/x64-windows/include

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../vcpkg/installed/x64-windows/lib/ -lheif
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../vcpkg/installed/x64-windows/debug/lib/ -lheif
#else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../projects/mylib/ -lmylib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    MediaManager.qrc
