#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	sleep(2);
	int f;
	f=getpid();
	int length = snprintf( NULL, 0, "%d", f );
	char* str = malloc( length + 1 );
	snprintf( str, length + 1, "%d",f);
    if (fork() == 0) 
    {
            if(fork()==0)
            {
				sleep(1);
			}
			else if(fork()==0)
			{
				if(fork()==0)
				{
					sleep(1);
				}
				execlp("pstree","pstree","-p",str,NULL);
			sleep(1);
			}
			else
			{
				sleep(1);
			}
	}
    else if(fork()==0)
    {
		if(fork()==0)
		{
			sleep(1);
		}
		sleep(1);
	}
	else if(fork()==0)
	{
		if(fork()==0)
		{
			if(fork()==0)
			{
				sleep(1);
			}
			else
			{
			sleep(1);
			}
		}
		else if(fork()==0)
		{
			sleep(1);
		}
		else
		{
			sleep(1);
		}
	}
	else
	{
		sleep(1);
	}
  return EXIT_SUCCESS;
}
