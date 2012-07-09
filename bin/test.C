#include <iostream>
#include <cstdio>
#include <bitset>

using std::cout;
using std::endl;
int main(void)
{
  std::bitset<8> bits;
  for(unsigned int i=0; i < 255; ++i)
    {
      bits=std::bitset<8>(i);
      printf("i: %d bit: %d",i,bits);
      //cout<<"i: "<<i<<"bit: "<<bits<<" flipped: ";
      bits.flip();
      printf(" flipped: %d \n",bits);
      //cout<<bits<<endl;
    }
  return 0;
}
