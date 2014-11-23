#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "signupdialog.h"
#include "connectdialog.h"
#include "clientcommand.h"
#include "clientcommandmanager.h"
#include <iostream>
#include <string>
#include <set>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//TODO: move local files to server
void MainWindow::on_movefiles_button_clicked()
{

}

//example of adding an item to list widget
//connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(newItem()));
//void MyForm::newItem()
//{
//ui.listWidget->addItem(ui.lineEdit->text());
//}


//open the signup widget
void MainWindow::on_signup_button_clicked()
{
    SignUpDialog signup;
    signup.setModal(true);
    signup.exec();
}

//open dialog to enter ip address
void MainWindow::on_connect_button_clicked()
{
    //connect
    ConnectDialog connect;
    connect.setModal(true);
    connect.exec();
}

void MainWindow::on_cd_button_clicked()
{
    //call this method: int ClientCommand::CdCommand( const char *path )
    // get the path from
    char * path = strstr((char *)ui->cd_textedit->toPlainText().toStdString().c_str(),"550");
    ClientCommandManager::clientCommand->CdCommand(path) ;
    //add local files: call the lscommand method in clientcommand, and then add all of them to the list widget
    ClientCommand * clientCommand = ClientCommandManager::clientCommand;
    //TODO: MOVE THESE TO THE ONCLICK OF THE ENTER DIRECTORY CONFIRM BUTTON
    std::set<std::string> filesSet = clientCommand->LsCommand();
    int size = filesSet.size();
    std::set<std::string>::iterator it = filesSet.begin();
    for (int i=0;i<size;i++){
        std::advance(it, i);
        std::string thisFile = *it;
        QString qstr = QString::fromStdString(thisFile);
        ui->localfiles_list->addItem(qstr);
    }
}
