#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QString>
#include <QPushButton>
#include <QByteArray>
#include <QComboBox>
#include <QTextEdit>

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
    void ReadData();
    void sendData();

    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;

    QLabel *temperature;
    QLabel *windspeed;
    QLabel *mode;

    QTimer *timer;

    QPushButton *sendButton;

    QSerialPort *serial_port;

    QComboBox *sendWind;
    QComboBox *sendMode;
    QTextEdit *sendTem;
};

#endif // MAINWINDOW_H
