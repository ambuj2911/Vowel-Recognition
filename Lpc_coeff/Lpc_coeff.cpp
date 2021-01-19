// Lpc_coeff.cpp : Defines the entry point for the console application.


#include "stdafx.h"
#include "vector"
#include  "stdio.h"
#include <sstream>
#include "iostream"
#include <fstream>
#include <cmath>
#define ld long double
using namespace std;
int count_r = 0;
vector<string> ch;

vector <long int> find_utterance()
{
	ifstream myfile, temp;
	myfile.open("example.txt");
	char str[60];
	FILE *fp = fopen("example.txt", "r");
	vector <long int> marker;
	long int one, two, three, ptr;
	ld ste_frame = 0, max = 0, x;
	long int flag = 0, line = 0;
	while( fgets (str, 60, fp)!=NULL ) { // Iterating over lines of files.    
	  myfile >> x;
	  if (line % 320 == 0)
	  {
				ste_frame = ste_frame / 320;
				if(ste_frame > 10000)
				{
					
					if(flag == 1)
					{
						if(ste_frame > max)
						{
							max = ste_frame;
							ptr = one;	
						}
					}
					else
					{
						max = ste_frame;
						one = myfile.tellg();
						two = myfile.tellg();
						three = myfile.tellg();
						flag = 1;
					}
					
					ste_frame = x * x;
					three = two;
					two = one;
					one = myfile.tellg();
					line++;
		
				}
		
				else
				{
							if(flag == 1)
							{
								marker.push_back(ptr);
								flag = 0;
								max = 0;
							}
							ste_frame = x * x;
							one = myfile.tellg();
							two = myfile.tellg();
							three = myfile.tellg();	
							line++;
				}
	  
	  }
	  
	  else
	  {
		ste_frame = ste_frame + x*x;
		line++;
	  }

	}
	return marker;
}




vector<ld> Ri_Calculate(vector <ld> s, int p)
{
	 int k, i;
	 vector <ld> r;
	 for(k=0;k <= p; k++)
	 {
		 ld temp = 1;
		 ld sum = 0;
		 for(i = 0; i <= 319 - k ; i++)
		 {
		   temp = s[i] * s[i+k];
		   sum = sum + temp;
		 }
		 r.push_back(sum);
	 }
	 return r;
}



vector<ld> Ai_Calculate(vector <ld> r, int p)
{
	vector<ld> ai;
	ai.push_back(1);
	ai.push_back(0);
	int i , j, t = 1;
	ld k = (r[1] / r[0])* -1;
	ai[1] = k;
	ld a = r[0] * (1 - (k * k));
	vector <ld> temp_a;
	
	for(i = 2;i <= p;i++)
	{
		ld s = 0;
		for(j = 1; j <= i-1; j++)
		{
			s = s + (r[j] * ai[i-j]);
		}
	
		s = s + r[i];
		k = (s/a) * -1;

		temp_a.clear();
		temp_a.push_back(0);

		for(j = 1; j <= i-1; j++)
		{
			temp_a.push_back( ai[j] + k * ai[i-j]);
		}

		for(j = 1; j<= i-1;j++)
		{
			ai[j] = temp_a[j];
		}
		
		ai.push_back(k);
		a = a * (1 - (k*k));

	}

   

	for(i = 0; i< ai.size() ; i++)
	{
		ai[i] = ai[i] * -1;
	}
	
	
	return ai;

}



vector<ld> Ci_Calculate(vector <ld> a, int p)
{
	int i,k;
	ld temp;
	vector<ld> c;
	c.push_back(0);
	for(i = 1; i<=p; i++)
	{
		temp = a[i];
		for(k = 1; k <= i- 1; k++)
		{
			temp += (c[k]* a[i-k]*k) / i ;
		}
		c.push_back(temp);
	}
	c.erase(c.begin());
	return c;
}



vector<ld> to_vector(long int frame_start,long int frame_end)
{
	vector<ld> s;
	ld ham_weight;
	ifstream file;
	file.open("example.txt");
	int line = 0;
	ld x = 0;
	file.seekg(frame_start);
	for(int j = 0; j< frame_end * 320; j++)
		file >> x;
	
	for(int i = 0; i<= 319; i++)
	{
		file >> x;	
		ham_weight = 0.54 - 0.46 * cos(2 * 3.14 * line++ / 319);
		x = x* ham_weight;
		s.push_back(x);
		line++;	
	}

	return s;
}




void create_reffile(vector<vector<ld>> ci, string n)
{
	string file = "Ref_files/204101005_ref";
	file = file + n;
	ofstream myfile(file);
	int i ,temp;
	vector<vector<ld>> avg_ci;
	vector <ld> sum;

	for(i=0; i<5;i++)
	{
		temp = i %5;
		sum.clear();
		int j;
		for(int k = 0; k < 12; k++)
		{
			sum.push_back(0);
		}

		for(j=0; j< ci.size(); j++)
		{
			if(j%5 == temp)
			{
				int k;
				for(k = 0; k < 12; k++)
				{
					sum[k] = sum[k] + ci[j][k];

				}
			}
		
		}
		for(int k = 0; k < 12; k++)
		{
			sum[k] = sum[k] / 5;

		}
		avg_ci.push_back(sum);
	}
	
	
	for(i=0;i < avg_ci.size();i++)
	{
		for(int k = 0; k<12;k++)
		{
			myfile<<avg_ci[i][k] <<"\n";
		}
	
	}
}



vector<vector<ld>> training(vector<long int> marker, string name, int flag)
{
	
	long int i ,frame_start, frame_end;
	vector<vector<ld>> ci;
	vector<ld> s,a,r,c;
	FILE *fp = fopen("example.txt", "r");
	
	for(i=0; i < marker.size();i++)
	{
		for(int j = 0; j < 5 ; j++)
		{
			frame_start = marker[i];
			frame_end =  j;
			s = to_vector(frame_start, frame_end);
			r = Ri_Calculate(s, 12);
			a = Ai_Calculate(r, 12);
			c = Ci_Calculate(a,12);
			ci.push_back(c);
			
		}
	}
	
	if (flag == 0)
	{
		create_reffile(ci, name);
		return ci;
	}
	else
		return ci;

	
}




ld normalise(FILE * fp, ld dc)
{
	char str[60];
	ld max = 0, min = 0;    
	if(fp == NULL) {
      perror("Error opening file");
      return(-1);
      }
	
	while( fgets (str, 60, fp)!=NULL ) { // Iterating over lines of files.    
	  stringstream num(str);// Lines of file are readed as strings so a converter is used to convert stringg to integer.
      ld x = 0;
	  num >> x;   
	  x = x - dc;
	
	  if(max < x)
	  {
		max = x;
	  }
	  if (min > x)
	  {
		min = x;
	  }
	   
	}

	if (max > min * -1)
	{
	  return(10000 / max);
	}
	else
	{
	  return(10000 / min);
	}

}



void normalise_file(FILE *fp, ld norm_fact, ld dc)
{
	char str[60];
	ofstream myfile;
	if(fp == NULL) {
      perror("Error opening file");
      
      }
	myfile.open ("example.txt");
	if(myfile == NULL) {
      perror("Error opening file example");
      
      }
	while( fgets (str, 60, fp)!=NULL ) { // Iterating over lines of files.
    
	  stringstream num(str);// Lines of file are readed as strings so a converter is used to convert stringg to integer.
      ld x = 0;
	  num >> x;    
	  myfile << (x - dc) * norm_fact;
	  myfile<<"\n";
	}

}



ld DC_Shift(FILE *fp)
{
  
  ld dc = 0;
  char str[60];
  int line = 0, flag = 0, k = 1;
  
  while( fgets (str, 60, fp)!=NULL && k == 1 ) { // Iterating over lines of files.
    
	  stringstream num(str);// Lines of file are readed as strings so a converter is used to convert stringg to integer.
      ld x = 0;
	  num >> x;    
	  if (flag == 3)
	  {
		dc = dc / (3 * 320);
		k = 0;
	  }
	  else
	  {
		 if (line % 320 == 0 && line != 0)
		 {
			dc = dc + x;
			flag++;
			line++;
			
		 }
		 else
		 {
		   dc = dc + x;
		   line++;
		 }
	  }
	}
  return dc;

}

vector<vector <ld>> ci_a, ci_e, ci_i, ci_o, ci_u;

void convert_vector(char a)
{
	vector< vector<ld> > c;
	c.clear();
	vector<ld> temp;
	int i = 0, line = 0;
	char str[60];
	string file_name = "Ref_files/204101005_ref";
	file_name.push_back(a);
	const char* s = file_name.c_str();
	FILE * fp = fopen(s, "r");
	while( fgets (str, 60, fp)!=NULL ) { // Iterating over lines of files.
	  stringstream num(str);// Lines of file are readed as strings so a converter is used to convert stringg to integer.
      ld x = 0;
	  num >> x; 
	  temp.push_back(x);
	  line++;  

	  if(line % 12 == 0)
	  {
		  c.push_back(temp);
		  temp.clear();
	  }
	}

	
	switch (a) {
	  case 'a':
		ci_a = c;
		break;
	  case 'e':
		ci_e = c;
		break;
	  case 'i':
		ci_i = c;
		break;
	  case 'o':
		ci_o = c;
		break;
	  case 'u':
		ci_u = c;
		break;
	   }
}


ld tokuhara(vector<vector<ld>> ci_train, vector<vector<ld>> ci_test)
{
	ld difference,tokuhara_distance, final_distance = 0;
	ld weight_T[]={1.0,3.0,7.0,13.0,19.0,22.0,25.0,33.0,42.0,50.0,56.0,61.0};
	for(long unsigned int i=0; i<ci_train.size();i++)
	{
		tokuhara_distance=0;
		for(int j=0;j<12;j++)
		{
			difference=ci_train[i][j]-ci_test[i][j];
			tokuhara_distance+=(difference*difference*weight_T[j]);
		}
		final_distance+=tokuhara_distance;
	}
	return final_distance/5;
}



vector <char> testing(FILE *fp)
{
	int i;
	char label;
	vector <char> c;
	c.clear();
	ld dc_corr, norm_fact;
	dc_corr = DC_Shift(fp);
	norm_fact = normalise(fp, dc_corr);
	rewind(fp);
	char str[60];
	normalise_file(fp, norm_fact, dc_corr);
	vector <long int> marker;
	marker = find_utterance();
	vector<vector<ld>> ci, ci_ref, ci_temp;
	ci = training(marker, " ", 1);
	char file[5] = {'a', 'e', 'i', 'o', 'u'};
	ld min = 0, dist;
	int iter = ci.size();
	int k = 0, count = 0, count1 = 0;
	while(k < iter) // Repetitions in testing file controller
	{
		ci_temp.clear();
		for(int l = k; l < k + 5 ; l++) // One utterance ke liye ci of testing
		{
			ci_temp.push_back(ci[l]);
		}

		dist = tokuhara(ci_a, ci_temp);
		//cout << dist << endl;
		min = dist;
		label = 'a';
	
		dist = tokuhara(ci_e, ci_temp);
		//cout << dist << endl;
		if(min > dist)
		{
			min = dist;
			label = 'e';
		}
		dist = tokuhara(ci_i, ci_temp);
		//cout << dist << endl;
		if(min > dist)
		{
			min = dist;
			label = 'i';
		}
		dist = tokuhara(ci_o, ci_temp);
		//cout << dist << endl;
		if(min > dist)
		{
			min = dist;
			label = 'o';
		}
		dist = tokuhara(ci_u, ci_temp);
		//cout << dist << endl;
		if(min > dist)
		{
			min = dist;
			label = 'u';
		}
		
		c.push_back(label);
	
	k = k + 5;
	}
	return c;
}





int _tmain(int argc, _TCHAR* argv[])
{
	
		FILE *fp; // file pointer to access contents of text file.
		vector<vector<ld>> temp;
		char train[5] = {'a','e', 'i', 'o', 'u'};
		for(int l = 0; l< 5; l++)
		{

			string name = "Training_files/";
			name.push_back(train[l]);
			name += ".txt";
			const char* s = name.c_str();
			vector<long int> marker;
			ld dc_corr = 0, norm_fact;
			fp = fopen(s, "r");
			dc_corr = DC_Shift(fp); // DC_shift calculations.
			norm_fact = normalise(fp, dc_corr);
			rewind(fp);
			normalise_file(fp, norm_fact, dc_corr);
			marker = find_utterance();
			string j = "";
			j.push_back(train[l]);
			training(marker, j, 0);
			
		}
		convert_vector('a');
		convert_vector('e');
		convert_vector('i');
		convert_vector('o');
		convert_vector('u');
		vector<char> label;
		cout << "a testing file" << endl;
		label = testing(fopen("Test_files/test_a.txt", "r"));
		for(int p = 0; p< label.size(); p++)
			cout << label[p] << " ";
		cout << endl;
		cout << "e testing file" << endl;
		label = testing(fopen("Test_files/test_e.txt", "r"));
		for(int p = 0; p< label.size(); p++)
			cout << label[p] << " ";
		cout << endl;
		cout << "i testing file" << endl;
		label = testing(fopen("Test_files/test_i.txt", "r"));
		for(int p = 0; p< label.size(); p++)
			cout << label[p] << " ";
		cout << endl;
		cout << "o testing file" << endl;
		label = testing(fopen("Test_files/test_o.txt", "r"));
		for(int p = 0; p< label.size()-1; p++)
			cout << label[p] << " ";
		cout << endl;
		cout << "u testing file" << endl;
		label = testing(fopen("Test_files/test_u.txt", "r"));
		for(int p = 0; p< label.size(); p++)
			cout << label[p] << " ";
	    
		return 0;



}

