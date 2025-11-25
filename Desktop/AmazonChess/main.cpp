#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QString>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file("style.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QString::fromUtf8(file.readAll());
        qDebug() << QDir::currentPath();

        qApp->setStyleSheet(styleSheet);
    }

    QString style = file.readAll();
    a.setStyleSheet(style);
    MainWindow w;
    w.show();
    return a.exec();
}
