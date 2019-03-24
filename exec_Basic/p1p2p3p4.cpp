#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main()
{
	pid_t p = fork();

	cout<<"Started."<<endl;

	if(p > 0)
	{
		cout<<"I'm the parent"<<endl;

		for(int i = 0; i < 20; i++)
			cout<<i<<" ";

		pid_t chld = wait(NULL);
	}
	else
	{
		cout<<"I'm the original child"<<endl;
		char *args[] = {"./sample1", NULL};
		execvp(args[0], args);
	}

	return 0;
}