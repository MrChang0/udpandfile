#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "udpmutlticastsever.h"
#include "fileserver.h"

#include <QMainWindow>
#include <QFileDialog>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_scan_pushButton_clicked();
    void gotaddress(QHostAddress addr);
    void on_sendfile_pushButton_clicked();
    void on_allselet_pushButton_clicked();
    void on_listen_pushButton_clicked();
    void on_clear_pushButton_clicked();
    void gettcpconnectd(tcpclient *client);
    void finishtcpsend(QString addr);
    void clienterror(QString addr);
private:
    Ui::MainWindow *ui;
    UDPMutlticastSever udpsever;
    fileserver *filesever_;
    QList<QHostAddress> address;
    QList<QHostAddress> sendaddress;
    QString filename;
};

#endif // MAINWINDOW_H
