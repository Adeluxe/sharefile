#include "servercommand.h"
using namespace std ;
#define END 1234


ServerCommand::ServerCommand( int sockfd ) :
	m_sockfd( sockfd )
{
	m_path = "." ;
}

ServerCommand::~ServerCommand( void )
{
	close( m_sockfd ) ;
}


int ServerCommand::LsCommand( void ) const
{
	// std::cout << "LsCommand" << std::endl ;
	DIR *mydir = NULL ;
	int retval = 0 ;
	char fileName[SEND_BUF_SIZE] ;
	bzero( fileName, sizeof(fileName) ) ;
	struct dirent *pdir = NULL ;
	if ( !(mydir = opendir( m_path.c_str() )) )
	{
		perror( "opendir" ) ;
		goto error ;
	}
	while ( (pdir = readdir(mydir)) )
	{
		bzero( fileName, sizeof(fileName) ) ;
		*(int*)fileName = pdir->d_type ;
		if ( !memcpy( fileName+sizeof(int), pdir->d_name, strlen(pdir->d_name) ) )
		{
			perror( "memcpy" ) ;
			goto error ;
		}
		if ( write( m_sockfd, fileName, SEND_BUF_SIZE ) < 0 )
		{
			perror( "write" ) ;
			goto error ;
		}
		// printf( "while\n" ) ; 
	}
	*(int*)fileName = END ;
	if ( write( m_sockfd, fileName, SEND_BUF_SIZE ) < 0 )
	{
		perror( "write" ) ;
		goto error ;
	}
done:
	closedir( mydir ) ;
	// std::cout << "LsCommand:End" << std::endl ;
	return retval ;
error:
	retval = -1 ;
	goto done ;
}

int ServerCommand::QuitCommand( ) const
{
	std::cout << "有客户端退出" << std::endl; 
	return close( m_sockfd ) ;
}

int ServerCommand::GetCommand( const char *fileName ) const
{
	std::cout << "GetCommand" << std::endl ;
	std::string file = m_path + "/" + fileName ;
	char buffer[SEND_BUF_SIZE] ;
	bzero( buffer, sizeof(buffer) ) ;
	cout << "[get]" << file << endl ;
	int fd = open( file.c_str(), O_RDONLY ) ;
	if ( -1 == fd )
	{
		cerr << "openfile:" << file << endl ;
		// close( m_sockfd ) ;
		return -1 ;
	}
	ssize_t rbyte = 0 ;
	while ( (rbyte = read( fd, buffer, sizeof(buffer)) ) >= 0 )
	{
		if ( (write( m_sockfd, buffer, rbyte) ) < 0 )
		{
			perror( "write" ) ;
			close( fd ) ;
			close( m_sockfd ) ;
			return -1 ;
		}
		if ( (rbyte < SEND_BUF_SIZE) )
		{
			// std::cout << "end" << endl ;
	 		// write( m_sockfd, "end12345", strlen("end12345") ) ;
			goto done ;
		}
	
	}
done:
	close( fd ) ;
	std::cout << "GetCommand:End" << std::endl ;
	return 0 ;
}


int ServerCommand::PutCommand( const char *fileName ) const
{
	char buffer[RECV_BUF_SIZE] ;
	bzero( buffer, sizeof(buffer) ) ;
	std::string file = m_path + "/" + fileName ;
	cout << "[put]" << file << endl ;
	int fd = open( file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644 ) ;
	if ( -1 == fd )
	{
		cerr << "openfile:" << file << endl ;
		return -1 ;
	}
	ssize_t rbyte = 0 ;
	while ( (rbyte = read( m_sockfd, buffer, RECV_BUF_SIZE )) > 0 )
	{
		if ( write( fd, buffer, rbyte ) < 0 )
		{
			perror( "writefile" ) ;
			close( fd ) ;
			return -1 ;
		}
		std::cout << "rbyte:" << rbyte << std::endl ;
		if ( (rbyte < RECV_BUF_SIZE) )
		{
			goto done ;
		}
	}
done:
	return close( fd ) ;
}

int ServerCommand::HelpCommand( void ) const
{
	cout << "help" << endl ;
	return 0 ;
}

int ServerCommand::CdCommand( const char *dir )
{
	if ( ('/' == dir[0]) )
	{
		m_path = dir ;
	}
	else
	{
		m_path = m_path + "/" + dir ;
	}
	// std::cout << "m_path:" << m_path << std::endl ;
	return 0 ;
}
