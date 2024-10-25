#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main () {
	const int N = 3;
	int children = 0;
	std::cerr << "main pid=" << getpid() << std::endl;

	for (int i=1, j=N; i<=N && j==N ; i++)
	{
		if(fork()==0)
		{
			children = 0;
			std::cerr << " i:j " << i << ":" << j << std::endl;
			for (int k=1; k<=i && j==N ; k++) {
				if ( fork() == 0) {
					children = 0;
					j=0;
					std::cerr << " k:j " << k << ":" << j << std::endl;
				}
				else
				{
					children++;
				}
			}
		}
		else
		{
			children++;
			break;
		}
	}

	//std::cout << "children : " << children << " pid : " << getpid() << std::endl;
	for(int i = 0; i<children; ++i)
	{
		wait(NULL);
	}
	std::cerr << getpid() << std::endl;
	return 0;
}
