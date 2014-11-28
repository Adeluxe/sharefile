#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "signupdialog.h"
#include "signindialog.h"
#include "messagesubject.h"
#include "userscontroller.h"
#include "friendrequestpopup.h"
#include "addafrienddialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public MessageSubject
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void destroyAll();
    ~MainWindow();

private slots:
    void on_upload_button_clicked();

    void on_signup_button_clicked();

    void on_connect_button_clicked();

    void on_signin_button_clicked();

    void changeMessage(std::string string);

    std::string getMessage();

    void on_exit_button_clicked();

    void on_add_a_friend_button_clicked();

    void on_browse_button_clicked();

    void on_sharefile_button_clicked();

    void on_refresh_button_clicked();

    void on_download_button_clicked();

    void on_refresh_messages_button_clicked();

private:
    Ui::MainWindow *ui;
    AddAFriendDialog * addAFriendDialog;
    SignUpDialog * signupDialog;
    SignInDialog * signinDialog;
    std::string message;
    ErrorPopup * errorPopup;
    FriendRequestPopup * friendRequest;
    UsersController * usersController;
};

#endif // MAINWINDOW_H
