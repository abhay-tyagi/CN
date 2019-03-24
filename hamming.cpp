#include <iostream>
#include <cmath>
#include <vector>
#include <climits>
#include <ctime>
#include <random>

using namespace std;

int main()
{
	srand(time(NULL));

	int n, count = 0;
	cout<<"\nEnter size of binary number: ";
	cin>>n;

	vector<int> a(n);
	cout<<"\nEnter binary number: ";
	for(int i = 0; i < n; i++)
		cin>>a[i];

	int red_bits = ceil(log2((n + ceil(log2(n)))));

	vector<int> code(red_bits + n, INT_MAX);
	for(int i = 0, j = 0; i < code.size(); i++, j++)
	{
		int position = i + 1;
		if(pow(2, count) == position)
		{
			count++;
			j--;
			continue;
		}

		code[i] = a[j];
	}

	for(int i = 0; i < red_bits; i++)
	{
		vector<int> parity_pos;
		for(int j = 1; j <= code.size(); j++)
			if((j >> (i)) & 1)
				parity_pos.push_back(j);

		int c = 0;
		for(int j = 0; j < parity_pos.size(); j++)
			if(code[parity_pos[j] - 1] == 1)
				c++;

		if(c % 2)
			code[pow(2, i) - 1] = 1;
		else
			code[pow(2, i) - 1] = 0;

	}

	cout<<"\nOriginal code: ";
	for(int i = 0; i < code.size(); i++)
		cout<<code[i]<<" ";

	cout<<"\nIntroducing random error";
	int pos = rand() % (code.size() + 1);
	vector<int> wrong_code = code;
	wrong_code[pos - 1] = 1 - wrong_code[pos - 1];


	cout<<"\nWrong code: ";
	for(int i = 0; i < wrong_code.size(); i++)
		cout<<wrong_code[i]<<" ";

	// Correction

	int sum = 0;
	for(int i = 0; i < red_bits; i++)
	{
		vector<int> parity_pos;
		for(int j = 1; j <= code.size(); j++)
			if((j >> (i)) & 1)
				parity_pos.push_back(j);

		int c = 0;
		for(int j = 0; j < parity_pos.size(); j++)
			if(wrong_code[parity_pos[j] - 1] == 1)
				c++;

		if(c % 2)
			sum += pow(2, i);
	}

	cout<<"\nError was at "<<sum;
	cout<<"\n"<<pos<<endl;

	return 0;
}