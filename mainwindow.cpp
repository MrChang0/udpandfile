#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    filesever_(new fileserver(this))
{
    ui->setupUi(this);
    ui->scan_pushButton->setDisabled(true);
    QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
     foreach(QHostAddress address, AddressList){
         if(address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::Null && address != QHostAddress::LocalHost)
         {
             if (address.toString().contains("127.0.")){
               continue;
             }
             ui->address_comboBox->addItem(address.toString());
         }
     }
    connect(&this->udpsever,&UDPMutlticastSever::getclient,this,&MainWindow::gotaddress);
    connect(this->filesever_,&fileserver::getconneted,this,&MainWindow::gettcpconnectd);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::gotaddress(QHostAddress addr)
{
    this->address.push_back(addr);

    ui->clients_listWidget->insertItem(ui->clients_listWidget->currentRow() + 1,addr.toString());
}

void MainWindow::on_scan_pushButton_clicked()
{
    this->udpsever.scan();
    ui->clients_listWidget->clear();
}

void MainWindow::on_sendfile_pushButton_clicked()
{
    filename = QFileDialog::getOpenFileName(this,tr("选择文件"));
    if(filename.isNull() || !ui->scan_pushButton->isEnabled())
    {
        return ;
    }
    QList<QListWidgetItem *> items = ui->clients_listWidget->selectedItems();
    foreach(QListWidgetItem *Item,items)
    {
        qDebug()<<Item->text();
        sendaddress.push_back(QHostAddress(Item->text()));
    }
    this->udpsever.sendfile(sendaddress);
    this->filesever_->sendfile(filename,sendaddress.count());
}

void MainWindow::on_allselet_pushButton_clicked()
{
    ui->clients_listWidget->selectAll();
}

void MainWindow::on_listen_pushButton_clicked()
{
    QHostAddress addr = QHostAddress(ui->address_comboBox->currentText());
    this->udpsever.bind(addr);
    this->filesever_->listen(addr);
    ui->scan_pushButton->setEnabled(true);
}

void MainWindow::on_clear_pushButton_clicked()
{
    ui->clients_listWidget->clear();

}

void MainWindow::gettcpconnectd(tcpclient *client)
{
    QList<QListWidgetItem *> items = ui->clients_listWidget->findItems(client->getaddr(),Qt::MatchCaseSensitive);
    if(items.isEmpty()){
        qDebug()<<"not find";
        return;
    }
    foreach(QListWidgetItem *item,items)
    {
        item->setBackgroundColor(QColor(Qt::yellow));
    }
    connect(client,&tcpclient::finishsend,this,&MainWindow::finishtcpsend);
    connect(client,&tcpclient::error,this,&MainWindow::clienterror);
}

void MainWindow::finishtcpsend(QString addr)
{
    QList<QListWidgetItem *> items = ui->clients_listWidget->findItems(addr,Qt::MatchCaseSensitive);
    if(items.isEmpty()){
        qDebug()<<"not find";
        return;
    }
    foreach(QListWidgetItem *item,items)
    {
        item->setBackgroundColor(QColor(Qt::green));
    }
}

void MainWindow::clienterror(QString addr)
{
    QList<QListWidgetItem *> items = ui->clients_listWidget->findItems(addr,Qt::MatchCaseSensitive);
    if(items.isEmpty()){
        qDebug()<<"not find";
        return;
    }
    foreach(QListWidgetItem *item,items)
    {
        item->setBackgroundColor(QColor(Qt::red));
    }
}
