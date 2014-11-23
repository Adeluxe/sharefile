#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "signupdialog.h"
#include "connectdialog.h"
#include "clientcommand.h"
#include "clientcommandmanager.h"
#include <iostream>
#include <string>
#include <set>

void add_local_files() {
    //call the lscommand method in clientcommand, and then add all of them to the list widget
    ClientCommand * clientCommand = ClientCommandManager.getClientCommand();
    std::set<std::string> filesSet = clientCommand->LsCommand();
    int size = filesSet.size();
    for (int i=0;i<size;i++){
        ui.localfiles_list->addItem(filesSet[i]);
    }
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    add_local_files();
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
    ConnectDialog connect;
    connect.setModal(true);
    connect.exec();
}
