#include "clientcommand.h"
#include <QDebug>
#define COMMAND_BUF_SIZE  256
#define END 1234
using namespace std ;

ClientCommand::ClientCommand( const char *ip, int port )
{
    struct sockaddr_in addr ;
    bzero( &addr, sizeof(addr) ) ;
    m_sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ;
    if ( -1 == m_sockfd )
    {
        perror( "socket" ) ;
        return ;
    }
    addr.sin_family = AF_INET ;
    addr.sin_port = htons( port ) ;
    addr.sin_addr.s_addr = inet_addr( ip ) ;
    if ( -1 == connect( m_sockfd, (SA*)&addr, sizeof(addr) ) )
    {
        perror( "connect" ) ;
        return ;
    }
    m_bstart = true ;
}

ClientCommand::~ClientCommand( void )
{
    if ( m_bstart )
    {
        close( m_sockfd ) ;
    }
}

int ClientCommand::QuitCommand( void )
{
    int retval = 0 ;
    if ( write( m_sockfd, COMMAND_QUIT, strlen(COMMAND_QUIT) ) < 0 )
    {
        perror( "write" ) ;
        goto error ;
    }
    if ( m_bstart )
    {
        close( m_sockfd ) ;
        m_bstart = false ;
    }
done:
    return retval ;
error:
    retval = -1 ;
    goto done ;
}

// downloading file
int ClientCommand::GetCommand( string fileName, string localpath ) const
{
    int retval = 0 ;
    char command[COMMAND_BUF_SIZE] ;
    bzero( command, sizeof(command) ) ;
    char buffer[RECV_BUF_SIZE] ;
    bzero( buffer, sizeof(buffer) ) ;
    strcpy( command, COMMAND_GET ) ;
    strcat( command, fileName.c_str() ) ;
    ssize_t rbyte = 0 ;
    if ( "" == localpath )
    {
        localpath = "./" ;
        localpath = localpath + m_user.username ;
    }
    string path = localpath + "/" + fileName ;

    // open file
    int fd = open( path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644 ) ;
    if ( write( m_sockfd, command, strlen( command ) ) < 0 )
    {
        perror( "write" ) ;
        goto error ;
    }
    if ( -1 == fd )
    {
        perror( "open" ) ;
        goto error ;
    }
    while ( (rbyte = read( m_sockfd, buffer, sizeof(buffer) ) ) > 0 )
    {
        if ( write( fd, buffer, rbyte ) < 0 )
        {
            perror( "write" ) ;
            goto error ;
        }
        if ( (rbyte < RECV_BUF_SIZE) )
        {
            goto done ;
        }

    }
done:
    close( fd ) ;
    cout << "done downloading" << endl ;
    return retval ;
error:
    retval = - 1 ;
    goto done ;
}

// uploading file
int ClientCommand::PutCommand( string fileName ) const
{
    int retval = 0 ;
    char command[COMMAND_BUF_SIZE] ;
    bzero( command, sizeof(command) ) ;
    char sendbuf[SEND_BUF_SIZE] ;
    bzero( sendbuf, sizeof(sendbuf) ) ;
    // command
    string name = fileName ;
    string::size_type end = name.find_last_of( "/" ) ;
    if ( string::npos != end )
    {
        name = name.substr( end+1, name.length() - end - 1 ) ;
    }

    else
    {
        fileName = string(m_user.username) + "/" + fileName ;
    }
    cout << "fileName:" << fileName << endl ;
    strcpy( command, COMMAND_PUT ) ;
    strcat( command, name.c_str() ) ;
    int fd = open( fileName.c_str(), O_RDONLY ) ;
    ssize_t rbyte = 0 ;
    strcpy( sendbuf, command ) ;
    if ( write( m_sockfd, sendbuf, sizeof(sendbuf) ) < 0 )
    {
        perror( "write" ) ;
        goto error ;
    }
    if ( -1 == fd )
    {
        perror( "open" ) ;
        goto error ;
    }
    while ( (rbyte = read( fd, sendbuf, sizeof(sendbuf) )) >= 0 )
    {
        if ( write( m_sockfd, sendbuf, rbyte ) < 0 )
        {
            perror( "write" ) ;
            goto error ;
        }
        if ( (rbyte < SEND_BUF_SIZE) )
        {
            goto done ;
        }
    }
done:
    close( fd ) ;
    cout << "Done uploading" << endl ;
    return retval ;
error:
    retval = -1 ;
    goto done ;
}

//　helper
int ClientCommand::HelpCommand( void ) const
{
    cout << "help                  show help" << endl ;
    cout << "ls                    show current directory" << endl ;
    cout << "get filename          download file from server" << endl ;
    cout << "put filename          upload file to server" << endl ;
    cout << "cd  director          switch directory" << endl ;
    cout << "by                    quit" << endl;
    cout << "share filename user 　share to user" << endl ;
    cout << "rm    filename        delete file" << endl ;
    return 0 ;
}

int ClientCommand::RmCommand( string filename )
{
    int retval = 0 ;
    char command[COMMAND_BUF_SIZE] ;
    bzero( command, sizeof(command) ) ;
    strcpy( command, COMMAND_RM ) ;
    strcat( command, filename.c_str() ) ;
    // send command
    if ( write( m_sockfd, command, sizeof(command) ) < 0 )
    {
        perror( "write" ) ;
        goto error ;
    }
done:
    return retval ;
error:
    retval = -1 ;
    goto done ;

}


std::set<std::string> ClientCommand::LsCommand( void ) const
{
        std::set<std::string> returnSet;
        char filename[RECV_BUF_SIZE] ;
        bzero( filename, sizeof(filename) ) ;
        //　send command
        if ( write( m_sockfd, COMMAND_LS, strlen(COMMAND_LS) ) < 0 )
        {
            perror( "write" ) ;
                goto error ;
        }
        while( read( m_sockfd, filename, sizeof(filename) ) > 0  )
        {
                if ( END == *(int*)filename )
                {
//                        printf( "\n" ) ;
                        goto done ;
                }
                else if ( DT_DIR == *(int*)filename )
                {
//                        printf( "\033[0;34m%s\033[0m \t", filename+sizeof(int) ) ; TODO WHAT TO DO WITH THIS???
//                        returnSet.insert((std::string) filename+sizeof(int));
                }
                else
                {
//                        printf( "%s \t", filename+sizeof(int) ) ;
//                    qDebug("adding to returnSet: ");
//                    qDebug(filename+sizeof(int));
                        returnSet.insert((std::string) (filename+sizeof(int)));
                }
                fflush( stdout ) ;
                bzero( filename, sizeof(filename) ) ;
        }
done:
        return returnSet ;
error:
//        retval = - 1;
        goto done ;
}


// change direcotry
int ClientCommand::CdCommand( string path )
{
    int retval = 0 ;
    char command[COMMAND_BUF_SIZE] ;
    bzero( command, sizeof(command) ) ;
    // change directory
    strcpy( command, COMMAND_CD ) ;
    strcat( command, path.c_str() ) ;
    if ( write( m_sockfd, command, strlen(command) ) < 0 )
    {
        perror( "write" ) ;
        goto error ;
    }
done:
    return retval ;
error:
    goto done ;
}

// register
int ClientCommand::LogonCommand( std::string username, std::string password )
{
    int retval = 0 ;
    UserData user ;
    int replay = 0 ;
    int trycount = 3 ;
    // send command
    char command[COMMAND_BUF_SIZE] ;
    bzero( command, sizeof(command) ) ;
    strcpy( command, COMMAND_LOGON ) ;
    if ( write( m_sockfd, command, strlen(command) ) < 0 )
    {
        perror( "write" ) ;
        goto error ;
    }
retry:
//    cout << "Please enter your username:" ;
//    cin >> user.username ;
//    cout << "Please enter your password:" ;
//    cin >> user.password ;
    strncpy(user.username, username.c_str(), sizeof(user.username));
    strncpy(user.password, password.c_str(), sizeof(user.password));
    if ( write( m_sockfd, &user, sizeof(user) ) < 0 )
    {
        perror( "write" ) ;
        goto error ;
    }
    // read verification result
    read( m_sockfd, &replay, sizeof(replay) ) ;
    if ( -1 == replay && trycount )
    {
        cout << "user already exists" << endl;
        --trycount ;
        goto retry ;
    }
    if ( -1 == replay )
    {
        goto error ;
    }
    if ( -1 == mkdir( user.username, 0755 ) )
    {
        perror( "mkdir" ) ;
        goto error ;
    }
    strcpy( m_user.username, user.username ) ;
done:
    return retval ;
error:
    retval = -1 ;
    goto done ;
}

// login
int ClientCommand::LoginCommand( std::string username, std::string password )
{
    int retval = 0 ;
    int replay = 0 ;
    UserData user ;
    int trycount = 3 ;
    // send command
    char command[COMMAND_BUF_SIZE] ;
    bzero( command, sizeof(command) ) ;
    strcpy( command, COMMAND_LOGIN ) ;
    if ( write( m_sockfd, command, strlen(command) ) < 0 )
    {
        perror( "write" ) ;
        goto error ;
    }
retry:
//    cout << "Please enter your username:" ;
//    cin >> user.username ;
//    cout << "Please enter password:" ;
//    cin >> user.password ;
    strncpy(user.username, username.c_str(), sizeof(user.username));
    strncpy(user.password, password.c_str(), sizeof(user.password));
    // send username and password to socket
    if ( write( m_sockfd, &user, sizeof(user) ) < 0 )
    {
        perror( "write" ) ;
        goto error ;
    }
    // receiving verificaion result
    if ( read( m_sockfd, &replay, sizeof(replay) ) < 0 )
    {
        perror( "read" ) ;
        goto error ;
    }
    if ( -1 == replay && trycount )
    {
        cout << "incorrect username or password" << endl ;
        goto retry ;
    }
    strcpy( m_user.username, user.username ) ;
done:
    return retval ;
error:
    retval = -1;
    goto done ;
}

// share
int ClientCommand::ShareCommand( string file, string user )
{
    int retval = 0 ;
    // send command
    char command[COMMAND_BUF_SIZE] ;
    bzero( command, sizeof(command) ) ;
    strcpy( command, COMMAND_SHARE ) ;
    char szfile[256] ;
    bzero( szfile, sizeof(szfile) ) ;
    char szuser[256] ;
    bzero( szuser, sizeof(szuser) ) ;
    strcpy( szfile, file.c_str() ) ;
    strcpy( szuser, user.c_str() ) ;
    if ( write( m_sockfd, command, strlen(command) ) < 0 )
    {
        perror( "write" ) ;
        goto error ;
    }
    // sending file and username
    if ( write( m_sockfd, szfile, sizeof(szfile) ) < 0 )
    {
        perror( "write filename" ) ;
        goto error ;
    }
    if ( write( m_sockfd, szuser, sizeof(szuser) ) < 0 )
    {
        perror( "write username" ) ;
        goto error ;
    }
done:
    return retval ;
error:
    retval = -1 ;
    goto done ;
}

