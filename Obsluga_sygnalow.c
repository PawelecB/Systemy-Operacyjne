#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int global;

void catch(int signo) 
{
	printf("Odebrano sygnal S%d\n", signo);
	if(signo == SIGHUP)
	{
		pause();
	}
	if(signo == SIGILL)
	{
		raise(SIGKILL);
	}
	if(signo == SIGQUIT)
	{
		if(fork()==0)
		{
			sleep(2);
			raise(SIGKILL);
			return;
		};
	}
	return;
}

int main(void) 
{
	int i=1;
	while(i<30)
	{
		signal(i, catch);
		i++;
	}
	while(1)
	{
		fflush(stdout);					
		sleep(5);
		printf("*");
	}
  return 0;
}

