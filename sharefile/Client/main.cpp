#include "clientcommand.h"

int main( int argc, char **argv )
{
	if ( argc < 3 )
	{
		std::cout << argv[0] << "<IP><Port>" << std::endl ;
		return - 1 ;
	}
	ClientCommand clientcommand( argv[1], atoi(argv[2]) )  ;
	clientcommand.manager() ;
}
