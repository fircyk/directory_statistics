#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QTableWidget>
#include <QHeaderView>
#include <math.h>



namespace Ui {
class MainWindow;
}

struct Person{
    uint8_t PersonID;
    QString Name;
    QString Surname;
};

struct InOutHistory{
    Person person;
    QDateTime inOutDate;
};

enum eFingerCmd{
    cmdNull = 0,
    cmdGetUserCount = 1,
    cmdAddUserAutoIncrement = 2,
    cmdAddUserAutoIncrement2 = 3,
    cmdDeleteAllUsers = 4,
    cmdDeleteUserID = 5,
    cmdMatchFingerprint = 6,
    cmdEnableAutoMatch = 7,
    cmdDisableAutoMatch = 8,
    cmdAddUserAutoIncrement3 = 9,
    cmdHead = 144,
};

enum eFingerAck{
    ackNull = 10,
    ackSuccess = 11,
    ackFail = 12,
    ackUserOccupied = 13,
    ackFingerOccupied = 14,
    ackTimeout = 15,
    ackNoUser = 16,
    ackFull = 17,
    ackCheckSumFail = 18,
    ackGoOut = 19,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:



    void serialSendQbyte(const QByteArray array);
    void serialRead(void);
    void on_OpenCOMPortButton_clicked(void);
    void on_CloseCOMPortButton_clicked(void);
    void on_AddNewUserAutoIncButton_clicked(void);
    void on_DeleteAllUsersButton_clicked(void);
    void on_DeleteUserIDButton_clicked(void);
    void on_MatchUserButton_clicked(void);
    void on_EnableAutoMatchModeButton_clicked(void);
    void on_DisableAutoMatchModeButton_clicked(void);
    void on_GetUserCountButton_clicked(void);
    void on_ShowUserListButton_clicked(void);
    void on_ShowEntryLeaveListButton_clicked(void);
    void redrawButtons(void);
    uint8_t QtCheckSum(uint8_t* buf, uint8_t length);
    void ZeroBuf(void);
    void UpdateFiles(void);
    void ReadFilesAndUpdateData(void);

private:
    Ui::MainWindow *ui;
    uint8_t UserCount;
    uint8_t UserTrack;
    uint8_t IDtemp;
    QList<uint8_t> IDList;
    QSerialPort serial;
    QTimer *timerSerial;
    QTimer *timerLabel;
    QTimer *timerCmd;
    QByteArray bufread;
    bool ButtonFlag;
    bool SleepFlag;
    QMap <eFingerAck, QString> ACKTextMap;
    QList<InOutHistory> InOutHistoryList;
    QList<Person> UserList;
    QStringList textList;
    uint8_t buf[100];

};
#endif // MAINWINDOW_H
