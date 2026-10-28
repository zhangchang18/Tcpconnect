#include<stdio.h>
#include<signal.h>
int main(int argc, const char *argv[])
{
	int n=atoi(argv[1]);
	int n1=atoi(argv[2]);
	kill(n,n1);
	return 0;
}
