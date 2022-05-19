#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    temperature = centralWidget()->findChild<QLabel*>("temperature");
    mode = centralWidget()->findChild<QLabel*>("mode");
    windspeed = centralWidget()->findChild<QLabel*>("windspeed");

    sendButton = centralWidget()->findChild<QPushButton*>("sendButton");

    timer = new QTimer;

    serial_port = new QSerialPort;
    serial_port->setPortName("COM4");//选取串口
    serial_port->open(QIODevice::ReadWrite);             //打开串口
    serial_port->setBaudRate(9600);
    //qDebug() << ui.BaudRate->currentText().toInt() << endl;
    serial_port->setDataBits(QSerialPort::Data8);
    serial_port->setParity(QSerialPort::NoParity);
    serial_port->setStopBits(QSerialPort::OneStop);
    serial_port->setFlowControl(QSerialPort::NoFlowControl);

    QObject::connect(serial_port,SIGNAL(readyRead()),this,SLOT(ReadData()));

    sendTem =  centralWidget()->findChild<QTextEdit*>("sendTem");
    sendWind =  centralWidget()->findChild<QComboBox*>("sendWind");
    sendMode =  centralWidget()->findChild<QComboBox*>("sendMode");
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReadData()
{
    static QString head = "<html><head/><body><p><span style=\" font-size:28pt;\">";
    static QString tail = "</span></p></body></html>";
    char data[3];
    serial_port->read(data,1);
    if(data[0]==(char)0xF1) {
        return;
    }
    serial_port->read(data+1,2);
    if((data[0]&0x0F)>0x01)
        temperature->setText(head+(QString::number((uint)((data[0]&0x0F)+15)))+(tail));
    QString temp;
    switch ((data[1])&0x07) {
    case 0:
        temp.append("固定风");
        break;
    case 1:
        temp.append("高风");
        break;
    case 2:
        temp.append("中风");
        break;
    case 3:
        temp.append("低风");
        break;
    case 4:
        temp.append("自动风");
        break;
    default:
        //temp = windspeed->text();
        break;
    }
    if(((data[1])&0x07)<0x05)
        windspeed->setText(head+temp+tail);
    temp.clear();
    switch((data[2])&0x03){
        case 0:
           temp.append("制冷");
            break;
        case 1:
            temp.append("抽湿");
            break;
        case 2:
            temp.append("自动");
            break;
        case 3:
            temp.append("制热");
            break;
        default:
//            temp = mode->text();
            break;
    }
    mode->setText(head+temp+tail);
}

void MainWindow::sendData()
{
    QByteArray bytes;
//    bytes.push_back(0x11);
    bytes.push_back(0xA1);
    bytes.push_back(0xF1);
    bytes.push_back(sendTem->toPlainText().toInt()-15);
    bytes.push_back(sendWind->currentIndex());
    bytes.push_back(sendMode->currentIndex());
    serial_port->write(bytes);
//    serial_port->write();
}


void MainWindow::on_sendButton_clicked()
{
    sendData();
}
