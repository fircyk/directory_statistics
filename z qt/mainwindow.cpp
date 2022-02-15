#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QThread>
#include <QString>
#include <QInputDialog>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QTextStream>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        ui->COMPortComboBox->addItem(serialPortInfo.portName());
    timerSerial = new QTimer(this);

    timerLabel = new QTimer(this);

    ui->MatchModeStatusLine->setText("Auto Match Disabled");

    connect(timerLabel, SIGNAL(timeout()), this, SLOT(redrawButtons()));


    timerLabel->start(100);
    timerSerial->setInterval(50);
    IDList.clear();
    IDList={0};

    SleepFlag = false;
    ButtonFlag = true;
    ACKTextMap[ackNull]="ACK NULL";
    ACKTextMap[ackSuccess]="ACK SUCCESS";
    ACKTextMap[ackFail]="ACK FAIL";
    ACKTextMap[ackUserOccupied]="ACK USER OCCUPIED";
    ACKTextMap[ackFingerOccupied]="ACK FINGER OCCUPIED";
    ACKTextMap[ackTimeout]="ACK TIMEOUT";
    ACKTextMap[ackNoUser]="ACK NO USER";
    ACKTextMap[ackFull]="ACK FULL";
    ACKTextMap[ackCheckSumFail]="ACK CHECKSUM FAIL";
    ACKTextMap[ackGoOut]="ACK GO OUT";
    ui->UserTable->setVisible(false);


}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_OpenCOMPortButton_clicked(void)
{
    serial.setPortName(ui->COMPortComboBox->currentText());
    serial.setBaudRate(QSerialPort::Baud57600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    connect(&serial, SIGNAL(readyRead()) , this, SLOT(serialRead()));
    if(!(serial.isOpen()))
    {
        serial.open(QIODevice::ReadWrite);
        if(serial.isOpen())
        {


            ui->OpenCOMPortButton->setEnabled(false);
            ui->CloseCOMPortButton->setEnabled(true);
            ButtonFlag = false;
            timerSerial->start(50);
            QByteArray init;
            init.resize(1);
            init[0]=cmdGetUserCount;
            serialSendQbyte(init);



            ui->StatusCOMPortLine->setText("Open");

            qDebug() << "The COM port has been opened.";
            ReadFilesAndUpdateData();


        }
        else
        {
            qDebug() << "The COM port has not been opened.";
        }
    }
    else
    {
        qDebug() << "The COM port is already open.";

    }
}
void MainWindow::on_CloseCOMPortButton_clicked(void)
{
    serial.close();
    if(!(serial.isOpen()))
    {
        timerSerial->stop();
        ui->OpenCOMPortButton->setEnabled(true);
        ui->CloseCOMPortButton->setEnabled(false);
        ButtonFlag = true;

        ui->StatusCOMPortLine->setText("Closed");
        qDebug() << "The COM port has been closed.";
    }
}

void MainWindow::serialRead()
{

         bufread.append(serial.readAll());
         qDebug() << "test " << bufread;
         if(bufread.isNull())
             qDebug() << "null";

        if(bufread.size()>4){
            for(int i=0;i<bufread.size();i++)
                buf[i] = static_cast<uint8_t>(bufread[i]);

            qDebug() << "bufer " << buf[0] << " " << buf[1] << " " << buf[2] << " " << buf[3] << " " << buf[4];
            bufread.clear();
        }


        if(buf[0]==cmdHead){

            if(buf[1]==cmdAddUserAutoIncrement){
                if(buf[2]==ackSuccess){
                    uint8_t checksum = QtCheckSum(buf, 3);

                    if(buf[4]==checksum){

                        QMessageBox::information(0, "Success", "User "
                        +textList[0] + " " + textList[1] + "\nID: " + QString::number(UserTrack+1)
                                + " finished ADD1.");

                    }


                }else{
                    eFingerAck ack = (eFingerAck)buf[2];
                    if(ACKTextMap.contains(ack)){
                        QString ackstring = ACKTextMap.value(ack);
                        QMessageBox::information(0, "ERROR", ackstring);
                    }else{
                        QMessageBox::information(0, "ERROR", "Try again");
                    }

                }

            }

            else if(buf[1]==cmdAddUserAutoIncrement2){
                if(buf[2]==ackSuccess){
                    uint8_t checksum = QtCheckSum(buf, 3);

                    if(buf[4]==checksum){

                        QMessageBox::information(0, "Success", "User "
                        +textList[0] + " " + textList[1] + "\nID: " + QString::number(UserTrack+1)
                                + " finished ADD2.");
                    }


                }else{
                    eFingerAck ack = (eFingerAck)buf[2];
                    if(ACKTextMap.contains(ack)){
                        QString ackstring = ACKTextMap.value(ack);
                        QMessageBox::information(0, "ERROR", ackstring);
                    }else{
                        QMessageBox::information(0, "ERROR", "Try again");
                    }

                }

            }

            else if(buf[1]==cmdAddUserAutoIncrement3){
                if(buf[2]==ackSuccess){
                    uint8_t checksum = QtCheckSum(buf, 3);

                    if(buf[4]==checksum){

                        QMessageBox::information(0, "Success", "User "
                        +textList[0] + " " + textList[1] + "\nID: " + QString::number(UserTrack+1)
                                + " finished ADD3.");
                        IDList.append((UserTrack+1));
                        UserList.append({(uint8_t)(UserTrack+1), textList[0], textList[1]});
                        UserCount++;
                        ui->UserCountLine->setText(QString::number(UserCount));
                        qDebug() << "size " << IDList.count() << " size2 " << UserList.count();
                        UpdateFiles();
                    }


                }else{
                    eFingerAck ack = (eFingerAck)buf[2];
                    if(ACKTextMap.contains(ack)){
                        QString ackstring = ACKTextMap.value(ack);
                        QMessageBox::information(0, "ERROR", ackstring);
                    }else{
                        QMessageBox::information(0, "ERROR", "Try again");
                    }

                }




            }

            else if(buf[1]==cmdDeleteAllUsers){
                if(buf[2]==ackSuccess){
                    uint8_t checksum = QtCheckSum(buf, 2);
                    if(buf[3]==checksum){
                        UserCount=0;
                        ui->UserCountLine->setText(QString::number(UserCount));
                        QMessageBox::information(0, "Success", "All users removed succesfully");
                        IDList.clear();
                        UserList.clear();
                        InOutHistoryList.clear();
                        IDList={0};
                        for(int i=0; i<10; i++){
                            buf[i]=0;
                        }
                        UpdateFiles();
                    }

                }else{
                    eFingerAck ack = (eFingerAck)buf[2];
                    if(ACKTextMap.contains(ack)){
                        QString ackstring = ACKTextMap.value(ack);
                        QMessageBox::information(0, "ERROR", ackstring);
                    }else{
                        QMessageBox::information(0, "ERROR", "Try again");
                    }
                }


            }


            else if(buf[1]==cmdDeleteUserID){
                if(buf[2]==ackSuccess){
                    uint8_t checksum = QtCheckSum(buf, 2);
                    if(buf[3]==checksum){
                        UserCount--;
                        ui->UserCountLine->setText(QString::number(UserCount));
                        QMessageBox::information(0, "Success", "User ID "
                        + QString::number(IDtemp) + " removed succesfully.");
                        IDList.remove(IDtemp);
                        for(int i=0; i<UserList.count(); i++){
                            if(UserList.at(i).PersonID==IDtemp){
                                UserList.removeAt(i);
                                break;
                            }
                        }

                        for(int i=0; i<InOutHistoryList.count(); i++){
                            if(InOutHistoryList.at(i).person.PersonID==IDtemp){
                                InOutHistoryList.removeAt(i);

                            }
                        }
                        UpdateFiles();
                    }
                }else{
                    eFingerAck ack = (eFingerAck)buf[2];
                    if(ACKTextMap.contains(ack)){
                        QString ackstring = ACKTextMap.value(ack);
                        QMessageBox::information(0, "ERROR", ackstring);
                    }else{
                        QMessageBox::information(0, "ERROR", "Try again");
                    }
                }
            }

            else if(buf[1]==cmdGetUserCount){

                if(buf[2]==ackSuccess){
                    uint8_t checksum = QtCheckSum(buf, 3);
                    if(buf[4]==checksum){
                        UserCount=buf[3];
                          QMessageBox::information (0, "User Count", "User count: " + QString::number(UserCount));
                          ui->UserCountLine->setText(QString::number(UserCount));
                          for(int i=0; i<10; i++){
                              buf[i]=0;
                          }
                    }

                  }else{
                    eFingerAck ack = (eFingerAck)buf[2];
                    if(ACKTextMap.contains(ack)){
                        QString ackstring = ACKTextMap.value(ack);
                        QMessageBox::information(0, "ERROR", ackstring);
                    }else{
                        QMessageBox::information(0, "ERROR", "Try again");
                    }
                  }
            }

            else if(buf[1]==cmdMatchFingerprint){
                uint8_t ID;
                uint8_t iterator;
                if(buf[2]==ackSuccess){
                    //ID=buf[3];
                    uint8_t checksum = QtCheckSum(buf, 3);
                    if(buf[4]==checksum){
                        ID=buf[3];
                        for(int i=0; i<UserList.count(); i++){
                            if(UserList.at(i).PersonID==ID){
                                iterator=i;

                                Person person;
                                QDateTime dt = QDateTime::currentDateTime();

                                person.PersonID = UserList.at(i).PersonID;
                                person.Name = UserList.at(i).Name;
                                person.Surname = UserList.at(i).Surname;
                                InOutHistoryList.append({person, dt});

                                QMessageBox::information(0, "Success", "Matched ID " + QString::number(ID)
                                                         + "\nMatched User: " + UserList.at(iterator).Name + " "
                                                         + UserList.at(iterator).Surname
                                                         + "\n" + QDateTime::currentDateTime().toString());
                                break;
                            }
                        }
                        UpdateFiles();
                    }


                }else{
                    eFingerAck ack = (eFingerAck)buf[2];
                    if(ACKTextMap.contains(ack)){
                        QString ackstring = ACKTextMap.value(ack);
                        QMessageBox::information(0, "ERROR", ackstring);
                    }else{
                        QMessageBox::information(0, "ERROR", "Try again");
                    }
                }
            }

            else if(buf[1]==cmdEnableAutoMatch){
                if(buf[2]==ackSuccess){
                    uint8_t checksum = QtCheckSum(buf, 2);
                    if(buf[3]==checksum){
                        SleepFlag = true;
                        QMessageBox::information(0, "Auto mode", "Auto mode enabled");
                        ui->MatchModeStatusLine->setText("Auto Mode Enabled");
                    }

                }else{
                    eFingerAck ack = (eFingerAck)buf[2];
                    if(ACKTextMap.contains(ack)){
                        QString ackstring = ACKTextMap.value(ack);
                        QMessageBox::information(0, "ERROR", ackstring);
                    }else{
                        QMessageBox::information(0, "ERROR", "Try again");
                    }
                }
            }

            else if(buf[1]==cmdDisableAutoMatch){

                if(buf[2]==ackSuccess){
                    uint8_t checksum = QtCheckSum(buf, 2);
                    if(buf[3]==checksum){
                        SleepFlag = false;
                        QMessageBox::information(0, "Auto mode", "Auto mode disabled");
                        ui->MatchModeStatusLine->setText("Auto Mode Disabled");
                    }
                }else{
                    eFingerAck ack = (eFingerAck)buf[2];
                    if(ACKTextMap.contains(ack)){
                        QString ackstring = ACKTextMap.value(ack);
                        QMessageBox::information(0, "ERROR", ackstring);
                    }else{
                        QMessageBox::information(0, "ERROR", "Try again");
                    }
                }
            }
        }

        ZeroBuf();


}




void MainWindow::serialSendQbyte(const QByteArray array){
   // serialRead();
    serial.write(array);
}

void MainWindow::on_GetUserCountButton_clicked(){

    ui->UserTable->setVisible(false);
    QByteArray arr;
    arr.resize(1);
    arr[0]=cmdGetUserCount;
    serialSendQbyte(arr);

}

void MainWindow::on_AddNewUserAutoIncButton_clicked(){

    ui->UserTable->setVisible(false);

    bool ok;

    UserTrack = *std::max_element(IDList.begin(), IDList.end());
    QString text = QInputDialog::getMultiLineText(0, "Input name and surname", "Please input"
                                                  "\nname \nsurname", "", &ok);
    if (ok && !text.isEmpty()) {
        textList = text.split("\n");

        QMessageBox msgBox;
        msgBox.setText("Add user with"
                       "\nID: " + QString::number(UserTrack+1)
                       + "\nName: " + textList[0]
                       + "\nSurname: " + textList[1]);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if(msgBox.exec() == QMessageBox::Yes)
        {

            QByteArray arr;
            arr.resize(2);
            arr[0]=cmdAddUserAutoIncrement;
            arr[1]=(UserTrack+1);
            serialSendQbyte(arr);

        }
    }


}

void MainWindow::on_DeleteUserIDButton_clicked(){

      ui->UserTable->setVisible(false);
      QString availableID;

      foreach(uint8_t index, IDList){
          availableID += QString::number(index) + " ";
      }
      UserTrack = *std::max_element(IDList.begin(), IDList.end());

      QMessageBox::information(0, "Available IDs", availableID);
      bool ok;
      IDtemp = QInputDialog::getInt(this, tr("ID"), tr("ID:"), 0, 0, UserTrack, 1, &ok);
      if (ok)

      QMessageBox::information (0, "ID ", "ID to delete:" + QString::number(IDtemp));

      QByteArray arr;
      arr.resize(2);
      arr[0]=cmdDeleteUserID;
      arr[1]=IDtemp;
      serialSendQbyte(arr);

}

void MainWindow::on_DeleteAllUsersButton_clicked(){

    ui->UserTable->setVisible(false);

    QMessageBox msgBox;

    msgBox.setText("Delete All Users");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    if(msgBox.exec() == QMessageBox::Yes)
    {

        QByteArray arr;
        arr.resize(1);
        arr[0]=cmdDeleteAllUsers;
        serialSendQbyte(arr);

    }

}

void MainWindow::on_MatchUserButton_clicked(){

    ui->UserTable->setVisible(false);

    QByteArray arr;
    arr.resize(1);
    arr[0]=cmdMatchFingerprint;
    serialSendQbyte(arr);

}

void MainWindow::on_EnableAutoMatchModeButton_clicked(){

    ui->UserTable->setVisible(false);

    QMessageBox msgBox;
    msgBox.setText("Enable Auto Match Mode");
    msgBox.setInformativeText("No commands available until it is disabled");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    if(msgBox.exec() == QMessageBox::Yes)
    {

        QByteArray arr;
        arr.resize(1);
        arr[0]=cmdEnableAutoMatch;
        serialSendQbyte(arr);

    }

}

void MainWindow::on_DisableAutoMatchModeButton_clicked(){

    ui->UserTable->setVisible(false);

    QMessageBox msgBox;
    msgBox.setText("Disable Auto Match Mode");
    msgBox.setInformativeText("No commands available until it is disabled");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    if(msgBox.exec() == QMessageBox::Yes)
    {
        QByteArray arr;
        arr.resize(1);
        arr[0]=cmdDisableAutoMatch;
        serialSendQbyte(arr);

    }

}

void MainWindow::on_ShowUserListButton_clicked(void){

    ui->UserTable->setColumnCount(3);
    ui->UserTable->setRowCount(UserList.count());


    QStringList UserTableLabel;
    UserTableLabel << "ID" << "Name" << "Surname";

    ui->UserTable->setHorizontalHeaderLabels(UserTableLabel);

    for(int i=0; i < ui->UserTable->rowCount(); i++){
        QTableWidgetItem *UserTableItem;
        for(int j=0; j < ui->UserTable->columnCount(); j++) {
            UserTableItem = new QTableWidgetItem;
            if(j==0)
                UserTableItem->setText(QString::number(UserList.at(i).PersonID));
            if(j==1)
                UserTableItem->setText(UserList.at(i).Name);
            if(j==2)
                UserTableItem->setText(UserList.at(i).Surname);
            ui->UserTable->setItem(i, j, UserTableItem);
        }
    }

    ui->UserTable->setAlternatingRowColors(true);
    ui->UserTable->setShowGrid(true);
    ui->UserTable->horizontalHeader()->setDefaultSectionSize(120);
    ui->UserTable->horizontalHeader()->resizeSection(0, 50);
    ui->UserTable->verticalHeader()->setDefaultSectionSize(30);
    if(ui->UserTable->rowCount()<4){
        ui->UserTable->setFixedSize(307, ui->UserTable->rowCount()*50);
    }else{
        ui->UserTable->setFixedSize(307, 180);
    }




    ui->UserTable->setVisible(true);



}


void MainWindow::on_ShowEntryLeaveListButton_clicked(void){



    ui->UserTable->setColumnCount(4);
    ui->UserTable->setRowCount(InOutHistoryList.count());

    QStringList EntryLeaveLabel;
    EntryLeaveLabel << "ID" << "Name" << "Surname" << "Date and Time";

    ui->UserTable->setHorizontalHeaderLabels(EntryLeaveLabel);

    for(int i=0; i<ui->UserTable->rowCount(); i++){
        QTableWidgetItem *EntryLeaveItem;
        for(int j=0; j<ui->UserTable->columnCount(); j++) {
            EntryLeaveItem = new QTableWidgetItem;
            if(j==0)
                EntryLeaveItem->setText(QString::number(InOutHistoryList.at(i).person.PersonID));
            if(j==1)
                EntryLeaveItem->setText(InOutHistoryList.at(i).person.Name);
            if(j==2)
                EntryLeaveItem->setText(InOutHistoryList.at(i).person.Surname);
            if(j==3)
                EntryLeaveItem->setText(InOutHistoryList.at(i).inOutDate.toString());
            ui->UserTable->setItem(i, j, EntryLeaveItem);

        }
    }


    ui->UserTable->setAlternatingRowColors(true);
    ui->UserTable->setShowGrid(true);
    ui->UserTable->horizontalHeader()->setDefaultSectionSize(120);
    ui->UserTable->horizontalHeader()->resizeSection(0, 50);
    ui->UserTable->verticalHeader()->setDefaultSectionSize(30);


    ui->UserTable->resizeRowsToContents();
    ui->UserTable->resizeColumnsToContents();

    qDebug() << ui->UserTable->columnCount();
    /*
    if(ui->UserTable->rowCount()<4){


        ui->UserTable->setFixedSize(427, ui->UserTable->rowCount()*45);
    }else{
        ui->UserTable->setFixedSize(427, 180);
    }
    */
    ui->UserTable->adjustSize();
    int width = ui->UserTable->width();
    ui->UserTable->setFixedWidth(width+30);


    ui->UserTable->setVisible(true);


}

void MainWindow::redrawButtons(void){

    if(ButtonFlag){
        ui->AddNewUserAutoIncButton->setDisabled(true);
        ui->DeleteAllUsersButton->setDisabled(true);
        ui->DeleteUserIDButton->setDisabled(true);
        ui->MatchUserButton->setDisabled(true);
        ui->EnableAutoMatchModeButton->setDisabled(true);
        ui->GetUserCountButton->setDisabled(true);
        ui->DisableAutoMatchModeButton->setDisabled(true);
        ui->ShowEntryLeaveListButton->setDisabled(true);
        ui->ShowUserListButton->setDisabled(true);
    }else{
        ui->AddNewUserAutoIncButton->setDisabled(false);
        ui->DeleteAllUsersButton->setDisabled(false);
        ui->DeleteUserIDButton->setDisabled(false);
        ui->MatchUserButton->setDisabled(false);
        ui->EnableAutoMatchModeButton->setDisabled(false);
        ui->GetUserCountButton->setDisabled(false);
        ui->DisableAutoMatchModeButton->setDisabled(false);
        ui->ShowEntryLeaveListButton->setDisabled(false);
        ui->ShowUserListButton->setDisabled(false);
    }

    if(SleepFlag){

        ui->AddNewUserAutoIncButton->setDisabled(true);
        ui->DeleteAllUsersButton->setDisabled(true);
        ui->DeleteUserIDButton->setDisabled(true);
        ui->MatchUserButton->setDisabled(true);
        ui->EnableAutoMatchModeButton->setDisabled(true);
        ui->GetUserCountButton->setDisabled(true);
        ui->DisableAutoMatchModeButton->setDisabled(false);
    }else{

        ui->AddNewUserAutoIncButton->setDisabled(false);
        ui->DeleteAllUsersButton->setDisabled(false);
        ui->DeleteUserIDButton->setDisabled(false);
        ui->MatchUserButton->setDisabled(false);
        ui->EnableAutoMatchModeButton->setDisabled(false);
        ui->GetUserCountButton->setDisabled(false);
        ui->DisableAutoMatchModeButton->setDisabled(true);
    }




}

uint8_t MainWindow::QtCheckSum(uint8_t* buf, uint8_t length){

    uint8_t QtSum=0;
          for(int i=0; i<(length+1); i++){
               QtSum ^= buf[i];
          }
    return QtSum;

}

void MainWindow::ZeroBuf(void){
    for(int i=0; i<10; i++){
        buf[i]=0;
    }
}


void MainWindow::UpdateFiles(void){
    QFile IDListtxt("./IDListtxt.txt");
    QFile UserListtxt("./UserListtxt.txt");
    QFile EntryLeaveListtxt("./EntryLeaveListtxt.txt");



    QString textLine;

    if(IDListtxt.exists()){
        qDebug() << "exists";
    }else{
        qDebug() << "no nie";
    }

    IDListtxt.resize(0);
    IDListtxt.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream writeID(&IDListtxt);

    foreach(uint8_t ID, IDList){
        textLine = QString::number(ID) + "\r\n";
        writeID << textLine;
        textLine.clear();
    }
    writeID.flush();
    IDListtxt.close();

    UserListtxt.resize(0);
    UserListtxt.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream writeUser(&UserListtxt);

    foreach(Person person, UserList){
        textLine = QString::number(person.PersonID)+";"+person.Name+";"+person.Surname + "\r\n";
        writeUser << textLine;
        textLine.clear();
    }
    writeUser.flush();
    UserListtxt.close();

    EntryLeaveListtxt.resize(0);
    EntryLeaveListtxt.open(QIODevice::WriteOnly | QIODevice::Truncate);


    QTextStream writeEntryLeave(&EntryLeaveListtxt);

    foreach(InOutHistory inout, InOutHistoryList){
        textLine = QString::number(inout.person.PersonID)+";"+inout.person.Name+";"+inout.person.Surname+";"+inout.inOutDate.toString()+"\r\n";
        writeEntryLeave << textLine;
        textLine.clear();
    }
    writeEntryLeave.flush();
    EntryLeaveListtxt.close();

}


void MainWindow::ReadFilesAndUpdateData(void){
    QFile IDListtxt("./IDListtxt.txt");
    QFile UserListtxt("./UserListtxt.txt");
    QFile EntryLeaveListtxt("./EntryLeaveListtxt.txt");

    if(!(IDListtxt.exists())){
        IDListtxt.open(QIODevice::ReadWrite);
        QTextStream ReadStream(&IDListtxt);
       // ReadStream << "0\r\n";
        ReadStream.flush();
        IDListtxt.close();
    }else{
        IDListtxt.open(QIODevice::ReadOnly);
        QTextStream ReadStream(&IDListtxt);
        QString fileLine;
        while(!ReadStream.atEnd()){
            fileLine = ReadStream.readLine();
            uint8_t fileID = fileLine.toUInt();
            IDList.append(fileID);
            qDebug() << "appenduje id " << fileID;
            fileLine.clear();
        }
        ReadStream.flush();
        IDListtxt.close();
    }
    if(!(UserListtxt.exists())){
        UserListtxt.open(QIODevice::ReadWrite);
        UserListtxt.close();
    }else{
        UserListtxt.open(QIODevice::ReadOnly);
        QTextStream ReadStream(&UserListtxt);
        QString fileLine;
        QStringList fileLineSplit;
        while(!ReadStream.atEnd()){
            fileLine = ReadStream.readLine();
            fileLineSplit = fileLine.split(";");
            uint8_t fileID = fileLineSplit[0].toUInt();
            QString Name = fileLineSplit[1];
            qDebug() << "appenduje name " << Name;
            QString Surname = fileLineSplit[2];
            UserList.append({fileID, Name, Surname});
            fileLine.clear();
            fileLineSplit.clear();
        }
        ReadStream.flush();
        UserListtxt.close();
    }
    if(!(EntryLeaveListtxt.exists())){
        EntryLeaveListtxt.open(QIODevice::ReadWrite);
        EntryLeaveListtxt.close();
    }else{
        EntryLeaveListtxt.open(QIODevice::ReadOnly);
        QTextStream ReadStream(&EntryLeaveListtxt);
        QString fileLine;
        QStringList fileLineSplit;
        while(!ReadStream.atEnd()){
            fileLine = ReadStream.readLine();
            fileLineSplit = fileLine.split(";");
            uint8_t fileID = fileLineSplit[0].toUInt();
            QString Name = fileLineSplit[1];
            QString Surname = fileLineSplit[2];
            QDateTime dateTime = QDateTime::fromString(fileLineSplit[3]);
            InOutHistoryList.append({{fileID, Name, Surname}, dateTime});
            fileLine.clear();
            fileLineSplit.clear();
        }
        ReadStream.flush();
        EntryLeaveListtxt.close();
    }
}

