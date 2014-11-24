#ifndef _CLIENTCOMMAND_H
#define _CLIENTCOMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <dirent.h>
#include <string>
#include <set>

#include <iostream>

#define RECV_BUF_SIZE         4096
#define SEND_BUF_SIZE         4096

typedef struct sockaddr SA ;

#define COMMAND_LS            "ls"
#define COMMAND_CD            "cd"
#define COMMAND_GET           "get"
#define COMMAND_PUT           "put"
#define COMMAND_QUIT          "by"
#define COMMAND_HELP          "help"
#define COMMAND_LOGIN         "login"
#define COMMAND_REGISTER      "register"
#define COMMAND_SHARE         "share"
#define COMMAND_RM            "rm"

using namespace std ;

class ClientCommand
{
public:
    ClientCommand( const char *ip, int port ) ;
    ~ClientCommand( void ) ;
    int GetCommand( string file, string localpath = "" ) const ;
    int PutCommand( string fileName ) const ;
    int HelpCommand( void ) const ;
    int CdCommand( string path ) ;
    int LoginCommand( std::string username, std::string password ) ;
    int RegisterCommand( std::string username, std::string password ) ;
    int ShareCommand( string file, string user ) ;
    int RmCommand( string strfilename ) ;
    int manager( void ) ;
    int QuitCommand( void ) ;
    std::set<std::string> LsCommand( void ) const ;
private:
    struct UserData
    {
        char username[256] ;
        char password[256] ;
        UserData( const std::string &name = "",
                  const std::string &passwd = "" )
        {
            strcpy( username, name.c_str() ) ;
            strcpy( password, passwd.c_str() ) ;
        }
        bool operator<( const UserData &that ) const
        {
            return strcmp( username, that.username ) < 0 ;
        }
    } ;
    int  m_sockfd ;   // socket
    bool m_bstart ;   // server status
    UserData m_user ; // user info
} ;

#endif // _CLIENTCOMMAND_H
