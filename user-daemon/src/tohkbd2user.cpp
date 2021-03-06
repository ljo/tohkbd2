/*
 * (C) 2014 Kimmo Lindholm <kimmo.lindholm@gmail.com> Kimmoli
 *
 * Main, Daemon functions
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <QtGui/QGuiApplication>
#include <QtQml>
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QScopedPointer>
#include <QTimer>

#include <sailfishapp.h>

#include "userdaemon.h"
#include "adaptor.h"
#include "viewhelper.h"


int main(int argc, char **argv)
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());
    QObject::connect(view->engine(), SIGNAL(quit()), app.data(), SLOT(quit()));
    QScopedPointer<ViewHelper> helper(new ViewHelper(view.data()));
    view->rootContext()->setContextProperty("viewHelper", helper.data());

    QColor color;
    color.setRedF(0.0);
    color.setGreenF(0.0);
    color.setBlueF(0.0);
    color.setAlphaF(0.0);
    view->setColor(color);

    view->setClearBeforeRendering(true);

    view->setSource(SailfishApp::pathTo("qml/taskswitcher.qml"));

    QTimer::singleShot(1, helper.data(), SLOT(detachWindow()));

    setlinebuf(stdout);
    setlinebuf(stderr);

    printf("tohkbd2-user: starting daemon version %s\n", APPVERSION);

    printf("tohkbd2-user: Locale is %s\n", qPrintable(QLocale::system().name()));

    UserDaemon rw;
    new Tohkbd2userAdaptor(&rw);

    rw.registerDBus();

    QObject::connect(&rw, SIGNAL(_showTaskSwitcher()), helper.data(), SLOT(showWindow()));
    QObject::connect(&rw, SIGNAL(_hideTaskSwitcher()), helper.data(), SLOT(hideWindow()));
    QObject::connect(&rw, SIGNAL(_nextAppTaskSwitcher()), helper.data(), SLOT(nextApp()));

    QTranslator translator;
    translator.load("translations_" + QLocale::system().name(), "/usr/share/harbour-tohkbd2-user/i18n");
    app->installTranslator(&translator);

    return app->exec();
}
