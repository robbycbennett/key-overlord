#include <signal.h>


static bool running = true;


static void handle_signal(int)
{
	running = false;
}


int main()
{
	signal(SIGHUP,  handle_signal);
	signal(SIGINT,  handle_signal);
	signal(SIGQUIT, handle_signal);
	signal(SIGABRT, handle_signal);
	signal(SIGTERM, handle_signal);
}
