#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main()
{
	cout<<"Started1."<<endl;
		cout<<"I'm the child1"<<endl;
		char *args[] = {"./sample2", NULL};
		execvp(args[0], args);

	return 0;
}