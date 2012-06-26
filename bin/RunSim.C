#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <bitset>
#include <map>
#include <ctime>

//length of Cellular Automata
//IMPORTANT: MUST BE POWER OF 2
#define SIZE 256

//How many generations should be iterated
// Can be any number, but if equal to size, produced picture is a nice square
#define TMAX 256

using std::cout;
using std::endl;
using std::map;
using std::cerr;
using std::endl;
using std::bitset;
using std::ofstream;
using std::string;

void printXpmHeader(ofstream& xpmOutput, string fname)
{
  xpmOutput.open(fname.c_str());
  xpmOutput<< "/* XPM */\n";
  xpmOutput<<"static char * test_xpm[] = {"<<endl;
  xpmOutput<<"\""<<SIZE<<" "<<TMAX<<" 2 1\","<<endl;
  xpmOutput<<"\"1 c black\","<<endl;
  xpmOutput<<"\"0 c white\","<<endl;
  return;
}
void writeXpm(ofstream& xpmOutput)
{
  xpmOutput<<"};"<<endl;
  if(xpmOutput.is_open())
    xpmOutput.close();
}
bitset<SIZE> updateState(const bitset<SIZE> currentState, const unsigned int size, const bitset<8> truthMask)
{
  bitset<SIZE> nextState;

  //update mechanism borrowed from: 
  //Paul Bourke 
  //http://local.wasp.uwa.edu.au/~pbourke/fractals/ca/
  int k=0;
  for(unsigned int i=0; i < size; i++)
    {
      //compute position in "truthtable" based on previous state. Also is super fast. 
      k=4*currentState[(i-1+size)%size] + 
	2*currentState[i] + 
	currentState[(i+1)%size];
      nextState.set(i,truthMask[k]);
    }
  return nextState;
}
void printCurrentState(const bitset<SIZE> state,ofstream& xpmOutput, const unsigned int size)
{
  if(size==SIZE)
    xpmOutput<<"\""<<state<<"\","<<endl;
  else
    {
      int cellSize=SIZE/size; //map each of states to a chunk of the total picture size
      xpmOutput<<"\"";
      for(unsigned int i=0; i < size; i++)
	for(int j=0; j < cellSize; j++)
	  xpmOutput <<state[i];
      xpmOutput<<"\","<<endl;
    }
}

void runSim(int ruleNum,bitset<SIZE> currentState,int expandGen)
{
  ofstream xpmOutput;
  std::stringstream ofName;
  unsigned int size=SIZE;
  bitset<SIZE> expandState;
  if(expandGen!=0)
    size=2;
  ofName <<"rule_num_"<<ruleNum<<".xpm";
  printXpmHeader(xpmOutput,ofName.str());

  bitset<8> truthMask=ruleNum;
  cout <<"Starting with Rule "<<ruleNum<<endl;
  for(unsigned int j=0; j < TMAX; j++)
    {
      printCurrentState(currentState,xpmOutput,size);
      currentState=updateState(currentState,size,truthMask);
      if(j!=0 && size!=SIZE && j%expandGen==0)
	{
	for(unsigned int i=0; i < size; i++)
	    {
	      expandState.set(2*i,currentState[i]);
	      expandState.set(2*i+1,currentState[i]);
	    }
	  //cout<<"Expanding state of size "<<size<<" at time step "<<j<<endl;
	  size*=2;
	  currentState=expandState;
	}
      else if(j!=0 && j%expandGen==0)
	{
	  for(unsigned int i=SIZE/4; i < 3*SIZE/4; i++)
	    {
	      expandState.set(2*(i-SIZE/4),currentState[i]);
	      expandState.set(2*(i-SIZE/4)+1,currentState[i]);
	    }
	  currentState=expandState;
	}
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
  printf("-e --expand [num] (run with lattice doubling in size every [num] generations\n");
  printf("\t if [num] is zero the lattice doesn't expand (default behavior))\n\n");
  printf("If no options are specified the simulation is run in fixed mode with rule number 30\n");
  printf("Files are output in the form \"rule_num_#.xpm\"\n");
}
void initializeState(bitset<SIZE>& state, unsigned int size, bool initRand)
{
  if(initRand)
    {
      cout <<"Randomly initializing first " <<size<<" states\n"<<endl;
      for(unsigned int i=0; i < size; i++)
	state.set(i,rand()%2);
    }
  else
    state.set((int)(size/2),1);
}
int main(int argc, const char* argv[])
{
//Options
  int ruleNum=30;
  int expandGen=0; //How many generations before lattice expands
  bool initRand=false;
  bool batchMode=false;
  string opt;
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
    }
  bitset<SIZE> currentState;

  if(expandGen!=0)
    initializeState(currentState,2,initRand);
  else
    initializeState(currentState,SIZE,initRand);
  
  if(batchMode)
    {
      ofstream org_table;//store results in a table to view from emacs
      org_table.open("batch-table.org");
      org_table << "| <c> | <c> | <c> | <c> |"<<endl;
      for(unsigned int i=0; i<256; i++)
	{
	  org_table <<"| "<<"[[file:rule_num_"<<i<<".gif]] Rule Number "<<i;
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
