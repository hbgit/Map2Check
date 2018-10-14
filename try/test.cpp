#include <stdlib.h> 
#include <iostream> 
 
int main() { 
 
// get and print shell environmental variable home 
std::cout << "PATH = " << getenv("PATH") << std::endl; 
 
//set new shell environmental variable using putenv 
char mypath[]="PATH=$PATH:../"; 
putenv( mypath ); 
 
std::cout << "PATH = " << getenv("PATH") << std::endl; 
 
return 0; 
 
}
