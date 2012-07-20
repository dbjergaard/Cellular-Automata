#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <bitset>

//length of Cellular Automata
//IMPORTANT: MUST BE POWER OF 2
#define SIZE 32

//How many generations should be iterated
// Can be any number, but if equal to size, produced picture is a nice square
#define TMAX 64

using std::cout;
using std::map;
using std::pair;
using std::endl;
using std::cerr;
using std::endl;
using std::bitset;
using std::ofstream;
using std::string;

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
double calcInformation(const bitset<SIZE>& state)
{
  //Calculate and return information of bitset defined as 
  // \sum_i^n p_i log(1/p_i)
  // where log is in base 2. 
  //double information=0;
  
  int index=0;
  double norm=0;
  double information;
  typedef map<int,double> histo;
  histo pDist; //estimate p_i distribution for information calculation. 

  for(unsigned int i=0; i<SIZE; i++)
    {
      index=
	64*state[(i-3+SIZE)%SIZE]+
	32*state[(i-2+SIZE)%SIZE]+
	16*state[(i-1+SIZE)%SIZE]+
	8*state[i]+
	4*state[(i+1)%SIZE]+
	2*state[(i+2)%SIZE]+
	state[(i+1)%SIZE];
      pDist[index]+=1.0;
    }
  
  //cout<<"Histogram: "<<endl;
  for (histo::iterator it=pDist.begin(); it!=pDist.end(); ++it)
    {
      norm+=it->second;
    }
  for (histo::iterator it=pDist.begin(); it!=pDist.end(); ++it)
    {
      it->second/=norm;
      //cout <<it->first<<","<<it->second<<endl;
      information+=it->second*log2(1/it->second);
    }
  
  return information;
}

bitset<SIZE> updateState(const bitset<SIZE> currentState, const bitset<8> truthMask)
{
  bitset<SIZE> nextState;

  //update mechanism borrowed from: 
  //Paul Bourke 
  //http://local.wasp.uwa.edu.au/~pbourke/fractals/ca/
  int k=0;
  for(unsigned int i=0; i < SIZE; i++)
    {
      //compute position in "truthtable" based on previous state. Also is super fast. 
	k=4*currentState[(i-1+SIZE)%SIZE] + 
	  2*currentState[i] + 
	  currentState[(i+1)%SIZE];
	
	/*
	if(edgeBits)
	  {
	    if(i==0)
	      k=4*edgeBits->first+2*currentState[i]+currentState[(i+1)%SIZE];
	    else if(i+1 == SIZE)
	      k=4*currentState[(i-1+SIZE)%SIZE] + 2*currentState[i] + edgeBits->second;
	  }
	*/
      nextState.set(i,truthMask[k]);
    }
  return nextState;
}
void printCurrentState(const bitset<SIZE> state,ofstream& xpmOutput,bool doubleGen)
{
  for(unsigned int i=0; i < SIZE; i++)
    {
      xpmOutput<<state[i];
      if(i!=SIZE-1)
	xpmOutput <<",";
      else
	xpmOutput<<","<<doubleGen+2;
    }
  xpmOutput<<endl;
}

void runSim(int ruleNum,bitset<SIZE> currentState,int expandGen, int seed)
{
  ofstream xpmOutput;
  //pair<bool,bool>* edgeBits= new pair<bool,bool>(currentState[SIZE/4],currentState[3*SIZE/4]);
  ofstream informationOutput;
  std::stringstream ofName;
  bitset<SIZE> expandState;
  ofName <<ruleNum<<"_time_series_seed_"<<seed<<".csv";
  xpmOutput.open(ofName.str().c_str());
  ofName.str("");
  ofName <<ruleNum<<"_info_series_seed_"<<seed<<".csv";
  informationOutput.open(ofName.str().c_str());
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
      	  //currentState=updateState(expandState,truthMask,edgeBits);
      	}
      //else
      currentState=updateState(currentState,truthMask);
      informationOutput<<j<<","<<calcInformation(currentState)<<endl;;
    }  
  /*
  if(edgeBits)
    delete edgeBits;
  */
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
  printf("-b, --batch (run over first 128 rules from the Wolfram Code)\n");
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
  //all 88 unique fules
  int uniqueRuleTable[]=
    {0,1,2,3,4,5,6,7,8,9,
     10,11,12,13,14,15,18,19,22,23,
     24,25,26,27,28,29,30,32,33,34,
     35,36,37,38,40,41,42,43,44,45,
     46,50,51,54,56,57,58,60,62,72,
     73,74,76,77,78,90,94,104,105,106,
     108,110,122,126,128,130,132,134,136,138,
     140,142,146,150,152,154,156,160,162,164,
     168,170,172,178,184,200,204,232};
  
//Options
  int ruleNum=30;
  int initNum=-1;
  int expandGen=0; //How many generations before lattice expands
  int seed=0;
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
	  seed=time(NULL);
	  srand(seed);
	}
      else if(opt=="-s" || opt=="--seed")
	{
	  initRand=true;
	  seed=atoi(argv[++i]);
	  srand(seed);
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
  bitset<SIZE> currentState;

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
      for(unsigned int i=0; i<88; i++)
	{
	  org_table <<"| "<<"[[file:"<<uniqueRuleTable[i]<<"_time.gif]] Rule Number "<<uniqueRuleTable[i];
	  if((i+1)%4==0)
	    org_table<<"|\n";
	  runSim(uniqueRuleTable[i],currentState,expandGen,seed);
	}
      org_table.close();
    }
  else
    runSim(ruleNum,currentState,expandGen,seed);

  return 0;
}
