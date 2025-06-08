#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots : void onAddResourceClicked();
                void updateCodes();
                void removeResource(int index);

private:
    Ui::MainWindow *ui;
    struct AuthResource {
        QString name;
        QString secret;
    };
    QVector<AuthResource> resources;  // список всех ресурсов
    QTimer *updateTimer;              // таймер для обновления кодов

};
#endif // MAINWINDOW_H
