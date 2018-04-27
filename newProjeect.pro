#-------------------------------------------------
#
# Project created by QtCreator 2017-05-18T15:50:36
#
#-------------------------------------------------

QT += core sql script scripttools svg opengl widgets gui location quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = newProjeect
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#opencv库
INCLUDEPATH += C:\opencv\opencv\build\include\opencv2 \
               C:\opencv\opencv\build\include\opencv  \
               C:\opencv\opencv\build\include
LIBS +=  C:\opencv\opencv\build\x64\vc14\lib\opencv_world310d.lib \





SOURCES += main.cpp\
        newproject.cpp \
    emptychild.cpp \
    filedeal.cpp \
    operate.cpp \
    database.cpp \
    sealine.cpp \
    MyLabel.cpp \
    classify.cpp \
    combineline.cpp \
    dbsetting.cpp \
    MyDialog.cpp \
    showinfo.cpp \
    progressbar.cpp \
    imagefile.cpp \
    zonedeal.cpp \
    svm.cpp \
    svmclassify.cpp \
    loginfo.cpp \
    zoneform.cpp \
    newzoneform.cpp \
    cutarea.cpp \
    erodinganddilating.cpp \
    bolckclassification.cpp \
    splitjoint.cpp \
    postprocess.cpp


HEADERS  += newproject.h \
    emptychild.h \
    filedeal.h \
    operate.h \
    database.h \
    sealine.h \
    MyLabel.h \
    classify.h \
    combineline.h \
    dbsetting.h \
    MyDialog.h \
    showinfo.h \
    progressbar.h \
    imagefile.h \
    zonedeal.h \
    svm.h \
    svmclassify.h \
    loginfo.h \
    zoneform.h \
    newzoneform.h \
    cutarea.h \
    erodinganddilating.h \
    bolckclassification.h \
    splitjoint.h \
    postprocess.h


FORMS    += newproject.ui \
    operate.ui \
    database.ui \
    MyDialog.ui \
    sealine.ui \
    MyLabel.ui \
    dbsetting.ui \
    showinfo.ui \
    progressbar.ui \
    loginfo.ui \
    zoneform.ui

unix|win32: LIBS += -L$$PWD/../../../../../warmerda/bld/lib/ -lgdal_i

INCLUDEPATH += $$PWD/../../../../../warmerda/bld/include
DEPENDPATH += $$PWD/../../../../../warmerda/bld/include

RESOURCES += \
    styleresource.qrc

DISTFILES +=

