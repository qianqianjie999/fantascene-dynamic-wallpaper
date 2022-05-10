#include <malloc.h>

#include "loadTranslation.h"
#include "application.h"
#include "wallpaper.h"
#include "dbuswallpaperservice.h"
#include "settingwindow.h"

#include <QObject>
#include <QMainWindow>
#include <QProcess>
#include <QThread>
#include <QDesktopWidget>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QStandardPaths>

/* Translation file path */
#define TRANSALTION_PATH "/usr/share/fantascene-dynamic-wallpaper/translations"

/* instance lock path */
#define INSTANCE_LOCK_PATH ".cache/fantascene"

/* instance lock name */
#define INSTANCE_LOCK "single"

int main(int argc, char *argv[])
{
    mallopt(M_ARENA_MAX, 1);

    Application a(argc, argv);

    a.setApplicationVersion("1.0.0");

#ifdef Q_OS_LINUX
    load_translation_files(TRANSALTION_PATH);
#endif

    setlocale(LC_NUMERIC, "C");

    /*
     * Check if there are multiple instances
     * If there are multiple instances, exit now.
    */
    const QString lock = QDir::homePath() + "/" + INSTANCE_LOCK_PATH + INSTANCE_LOCK;
    QLockFile lockFile(lock);

    if (!lockFile.tryLock(300))
    {
        qDebug() << "The application is already running!";
        QDBusInterface iface("com.deepin.dde.fantascene",
                             "/com/deepin/dde/fantascene",
                             "com.deepin.dde.fantascene",
                             QDBusConnection::sessionBus());
        iface.asyncCall("activeWindow");
        return 0;
    }

    QMainWindow *mainwindw = new QMainWindow();
    settingWindow *window = new settingWindow(mainwindw, mainwindw);
    mainwindw->setCentralWidget(window);
    mainwindw->setFixedSize(QSize(640, 500));
    mainwindw->setWindowTitle(QObject::tr("dynamic-wallpaper"));
    mainwindw->setWindowIcon(QIcon(":/install/wallpaper.png"));
    mainwindw->move(qApp->desktop()->screen()->rect().center() - mainwindw->rect().center());

    QString envName("DDE_SESSION_PROCESS_COOKIE_ID");
    QByteArray cookie = qgetenv(envName.toUtf8().data());
    qunsetenv(envName.toUtf8().data());
    if (cookie.isEmpty()) {
        QDBusInterface iface("com.deepin.SessionManager",
                             "/com/deepin/SessionManager",
                             "com.deepin.SessionManager",
                             QDBusConnection::sessionBus());
        iface.asyncCall("Register", QString(cookie));
    }


    return a.exec();
}


