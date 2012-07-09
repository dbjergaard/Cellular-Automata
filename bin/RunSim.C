#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <bitset>
#include <map>
#include <ctime>

//length of Cellular Automata
//IMPORTANT: MUST BE POWER OF 2
#define SIZE 16

//How many generations should be iterated
// Can be any number, but if equal to size, produced picture is a nice square
#define TMAX 32

using std::cout;
using std::endl;
using std::map;
using std::cerr;
using std::endl;
using std::bitset;
using std::ofstream;
using std::string;

void printXpmHeader(ofstream& xpmOutput, string fname, int expandGen)
{
  if(!xpmOutput.is_open())
    xpmOutput.open(fname.c_str());
  xpmOutput<< "/* XPM */\n";
  xpmOutput<<"static char * test_xpm[] = {"<<endl;
  if(expandGen!=0)
    {
      xpmOutput<<"\""<<SIZE<<" "<<TMAX+(TMAX/expandGen)<<" 3 1\","<<endl;
    }
  else
    xpmOutput<<"\""<<SIZE<<" "<<TMAX<<" 2 1\","<<endl;
  xpmOutput<<"\"1 c black\","<<endl;
  xpmOutput<<"\"0 c white\","<<endl;
  xpmOutput<<"\"2 c red,"<<endl;

  return;
}
void writeXpm(ofstream& xpmOutput)
{
  if(xpmOutput.is_open())
    {
      //xpmOutput<<"};"<<endl;
      xpmOutput.close();
    }
  else
    cerr<<"ERROR: could not write file!\n";
}
bitset<SIZE> updateState(const bitset<SIZE> currentState, const bitset<8> truthMask)
{
  bitset<SIZE> nextState;
  //update mechanism borrowed from: 
  //Paul Bourke 
  //http://local.wasp.uwa.edu.au/~pbourke/fractals/ca/
  int k=0;
  bool fixedBc=false;
  for(unsigned int i=0; i < SIZE; i++)
    {
      //compute position in "truthtable" based on previous state. Also is super fast. 
	k=4*currentState[(i-1+SIZE)%SIZE] + 
	  2*currentState[i] + 
	  currentState[(i+1)%SIZE];
	if(fixedBc)
	  {
	    if(i==0)
	      k=4+2*currentState[i]+currentState[(i+1)%SIZE];
	    else if(i+1 == SIZE)
	      k=4*currentState[(i-1+SIZE)%SIZE] + 2*currentState[i] + 1;
	  }
      nextState.set(i,truthMask[k]);
    }
  return nextState;
}
void printTrapezoid(ofstream& xpmOutput)
{
  double m = (32+0.0)/SIZE;
  double diffLeft=0.0;
  double diffRight=0.0;
  for(int j=0; j >= -8; --j)
    { 
      xpmOutput<<"\"";
      for(unsigned int i=0; i < SIZE; i++)
	{
	  diffLeft=j+8.-m*i;
	  diffRight=j+m*(i-0.75*SIZE);
	  if( ((diffLeft <1) && (diffLeft >=0 )) ||  ((diffRight <1) && (diffRight >=0 )))
	    {
	      xpmOutput<<1;
	    }
	  else
	    {
	      xpmOutput<<0;
	    }
	}
      xpmOutput<<"\",";
      xpmOutput<<endl;
    }
}
void printCurrentState(const bitset<SIZE> state,ofstream& xpmOutput,bool doubleGen)
{
  //xpmOutput<<"\"";
  for(unsigned int i=0; i < SIZE; i++)
    {
      xpmOutput<<state[i];
      if(i!=SIZE-1)
	xpmOutput <<",";
      else
	xpmOutput<<","<<doubleGen+2;
    }
  xpmOutput<<endl;
  //xpmOutput<<"\","<<endl;
}

void runSim(int ruleNum,bitset<SIZE> currentState,int expandGen)
{
  ofstream xpmOutput;

  std::stringstream ofName;
  bitset<SIZE> expandState;
  ofName <<"rule_num_"<<ruleNum<<".csv";
  xpmOutput.open(ofName.str().c_str());
  //printXpmHeader(xpmOutput,ofName.str(),expandGen);

  bitset<8> truthMask=ruleNum;
  cout <<"Starting with Rule "<<ruleNum<<endl;
  for(unsigned int j=0; j < TMAX; j++)
    {
      printCurrentState(currentState,xpmOutput,false);
	
      if(expandGen!=0 && j!=0 && j%expandGen==0)
      	{
      	  for(unsigned int i=SIZE/4; i < 3*SIZE/4; i++)
      	    {
      	      expandState.set(2*(i-SIZE/4),currentState[i]);
      	      expandState.set(2*(i-SIZE/4)+1,currentState[i]);
      	    }
	  printCurrentState(currentState,xpmOutput,false);
	  printCurrentState(expandState,xpmOutput,true);
      	  currentState=expandState;
      	}
      currentState=updateState(currentState,truthMask);
    }  
  writeXpm(xpmOutput);
  return;
}
void usage(const char* name)
{
  printf("Usage: %s [args]\n",name);
  printf("Where [args] is one of: \n");
  printf("-h, --help (Display this message and exit)\n");
  printf("-r, --rand (use time to set seed for random initial conditions)\n");
  printf("-s [num], --seed [num] (use num as a seed for random initial conditions)\n");
  printf("-f, --fixed (default, set middle bit only for intial conditions)\n");
  printf("-rn [0-255], --rule-num [0-255] (use rule number according to Wolfram Code)\n");
  printf("-b, --batch (run over all 256 rules from the Wolfram Code)\n");
  printf("-e --expand [num] (run with lattice doubling in size every [num] generations)\n");
  printf("-in --init-num [num] (sets initial bits using integer)");
  printf("\t if [num] is zero the lattice doesn't expand (default behavior))\n\n");
  printf("If no options are specified the simulation is run in fixed mode with rule number 30\n");
  printf("Files are output in the form \"rule_num_#.csv\"\n");
}
void initializeState(bitset<SIZE>& state, bool initRand, int initNum)
{
  if(initRand)
    {
      cout<<"Randomly initializing states"<<endl;
      for(unsigned int i=0; i < SIZE; i++)
	state.set(i,rand()%2);
    }
  else if(initNum >= 0)
    {
      state=bitset<SIZE>(initNum);
    }
  else
    state.set((unsigned int)(SIZE/2),1);
}
int main(int argc, const char* argv[])
{
//Options
  int ruleNum=30;
  int initNum=-1;
  int expandGen=0; //How many generations before lattice expands
  bool initRand=false;
  bool batchMode=false;
  string opt;
  // this can be easily broken (no strong type checking), but I don't
  // care because I'm the only one running it now
  for(int i=1; i<argc; i++)
    {
      opt=(string)argv[i];
      if(opt=="-h" || opt=="--help")
	{
	  usage(argv[0]);
	  return -1;
	}
      else if(opt=="-r" || opt=="--rand")
	{
	  initRand=true;
	  srand(time(NULL));
	}
      else if(opt=="-s" || opt=="--seed")
	{
	  initRand=true;
	  srand(atoi(argv[++i]));
	}
      else if(opt=="-f" || opt=="--fixed")
	initRand=false;
      else if(opt=="-rn" || opt=="--rule-num")
	ruleNum=atoi(argv[++i]);
      else if(opt=="-b" || opt=="--batch")
	batchMode=true;
      else if(opt=="-e" || opt=="--expand")
	expandGen=atoi(argv[++i]);
      else if(opt=="-in" || opt=="--init-num")
	initNum=atoi(argv[++i]);
    }
  bitset<SIZE> currentState;//(string("00000000000000001111000000001111"));

  initializeState(currentState,initRand,initNum);
  if(expandGen !=0 && TMAX%expandGen!=0)
    {
      cout <<"Warning: expandGen: "<<expandGen<<" is not an integral multiple of TMAX: "<<TMAX<<endl;
    }

  if(batchMode)
    {
      ofstream org_table;//store results in a table to view from emacs
      org_table.open("batch-table.org");
      org_table << "| <c> | <c> | <c> | <c> |"<<endl;
      for(unsigned int i=0; i<256; i++)
	{
	  org_table <<"| "<<"[[file:rule_num_"<<i<<".csv]] Rule Number "<<i;
	  if((i+1)%4==0)
	    org_table<<"|\n";
	  runSim(i,currentState,expandGen);
	}
      org_table.close();
    }
  else
    runSim(ruleNum,currentState,expandGen);

  return 0;
}
