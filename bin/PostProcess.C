#include <iostream>
#include <cstdio>
#include <bitset>
#include <fstream>
#include <string> 
#include <vector>

#define BUFFSIZE 256

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;

int main(int argc, const char* argv[])
{
  if(argc < 2) 
    return -1;
  string ifName(argv[1]);
  string oldLine;
  string line;
  int time=0;
  int period=-1;
  vector<string> data;
  char buffer [BUFFSIZE];//BE CAREFUL of overflows
  cout<<"Processing: "<<ifName<<endl;
  ifstream inFile;

  inFile.open(ifName.c_str());
  if(inFile.is_open())
    {
      while(inFile.good()) 
	{
	  inFile.getline(buffer,BUFFSIZE);
	  data.push_back(string(buffer));
	}
      for(vector<string>::iterator it=data.begin(); it!=data.end(); it++)
	{
	  time++;
	  if(it->at(it->size()-1)=='3')
	    {
	  /*
	      oldLine=*it;
	      time-=2;
	      for(vector<string>::iterator jt=it; jt!=data.end(); ++jt)
		{
		  if (*jt==oldLine && jt!=it)
		    {

		      cout <<"Transient time: "<<time;
		      cout <<" Period: "<<jt-it<<endl;

		    }
		}
	      //cout<<*it<<endl;
	    */  
	    }
	}
    }
  else
    cout<<"Could not open file: "<<ifName<<endl;
      
      
  return 0;
}
