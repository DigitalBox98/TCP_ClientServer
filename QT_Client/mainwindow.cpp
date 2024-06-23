#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);
    
    // Set default server address
    ui->lineEdit_serverAddress->setText("127.0.0.1");
    
    // Connect the connect button
    connect(ui->pushButton_connect, &QPushButton::clicked, this, &MainWindow::connectToServer);
    
    // Connect the send message button
    connect(ui->pushButton_send, &QPushButton::clicked, this, &MainWindow::sendMessage);

    // Connect return key on the message line edit to send message
    connect(ui->lineEdit_message, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    
    // Connect QTcpSocket signals
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::readMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectToServer()
{
    QString address = ui->lineEdit_serverAddress->text();
    int port = 8080; // You can also add a QLineEdit for the port if needed

    tcpSocket->abort(); // Disconnect if already connected
    tcpSocket->connectToHost(address, port);

    if (tcpSocket->waitForConnected(3000)) { // Wait for 3 seconds to connect
        ui->statusbar->showMessage("Connected to Server");
    } else {
        ui->statusbar->showMessage("Failed to connect to Server");
    }
}

void MainWindow::sendMessage()
{
    if (tcpSocket->state() == QAbstractSocket::ConnectedState) {
        QString str = ui->lineEdit_message->text();
        QByteArray byteArray = str.toUtf8();
        tcpSocket->write(byteArray);
        ui->lineEdit_message->clear();
    } else {
        QMessageBox::critical(this, "QTCPClient", "Not connected");
    }
}

void MainWindow::readMessage()
{
    QByteArray byteArray = tcpSocket->readAll();
    QString message = QString::fromUtf8(byteArray);
    ui->textBrowser_response->append(message);
}


