#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "signupdialog.h"
#include "connectdialog.h"
#include "clientcommand.h"
#include "clientcommandmanager.h"
#include <iostream>
#include <string>
#include <set>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    message = "";
    errorPopup = new ErrorPopup();
    attach(errorPopup); //IMPORTANT!!! observer pattern
}

MainWindow::~MainWindow()
{
    delete ui;
}

//TODO: move local files to server
void MainWindow::on_movefiles_button_clicked()
{
    if (ClientCommandManager::clientCommand == NULL) {
        changeMessage("please enter IP first!");
        return;
    }
//    else if (signinDialog == NULL) { //TODO keep track of whether the user is signed in
//        changeMessage("please sign in first");
//        return;
//    } else {
//    //TODO

//    }
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
    qDebug("signup botton clicked\n");
    if (ClientCommandManager::clientCommand == NULL) {
        changeMessage("please enter ip first!");
        return;
    } else {
       signupDialog = new SignUpDialog();
       signupDialog->setModal(true);
       signupDialog->exec();
//       return;
    }
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
    if (ClientCommandManager::clientCommand == NULL) {
        changeMessage("please enter ip first!");
        return;
    }
//    else if (signupDialog == NULL) {
//        changeMessage("please enter ip first!");
//        return;
//    } else {
        std::string path = ui->cd_textedit->toPlainText().toStdString();
        ClientCommandManager::clientCommand->CdCommand(path) ;
        //add local files: call the lscommand method in clientcommand, and then add all of them to the list widget
        ClientCommand * clientCommand = ClientCommandManager::clientCommand;
        std::set<std::string> filesSet = clientCommand->LsCommand();
        int size = filesSet.size();
        std::set<std::string>::iterator it = filesSet.begin();
        for (int i=0;i<size;i++){
            std::advance(it, i);
            std::string thisFile = *it;
            QString qstr = QString::fromStdString(thisFile);
            ui->localfiles_list->addItem(qstr);
        }
//    }
}

void MainWindow::on_signin_button_clicked()
{
    //TODO user log in
    //TODO new an instance of SignInDialog
}

void MainWindow::changeMessage(std::string _message) {
    message = _message;
    notify(message);
}

std::string MainWindow::getMessage() {
    return message;
}
