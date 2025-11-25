#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>

void handler(int sig)
{
	printf("\nRecieved Signal : %s\n", strsignal(sig));
	
	if (sig == SIGTSTP)
	{
		signal(SIGCONT, handler);  // Make sure SIGCONT handler is set
		signal(SIGTSTP, SIG_DFL);
		raise(SIGTSTP);
	}
	else if (sig == SIGCONT)
	{
		signal(SIGTSTP, handler);  // Reinstate SIGTSTP handler
		signal(SIGCONT, SIG_DFL);
		raise(SIGCONT);
	}
	else if (sig == SIGINT)
	{
		signal(SIGINT, SIG_DFL);
		raise(SIGINT);
	}
}

int main(int argc, char **argv)
{

	printf("Starting the program\n");
	signal(SIGINT, handler);
	signal(SIGTSTP, handler);
	signal(SIGCONT, handler);

	while (1)
	{
		sleep(1);
	}

	return 0;
}