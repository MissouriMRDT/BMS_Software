#include "lightTrig.h"
#ifdef __cplusplus
extern "C"
{
#endif
double sinLW(float x)
{
  float x2 = x*x;
  float x3 = x2*x;
  float x4 = x2*x2;
  float x5 = x2*x3;
  float x6 = x3*x3;
  return(long double)(x - (x3)/(3*2)+(x5)/(5*4*3*2)-(x6*x)/(7*6*5*4*3*2)+
        (x6*x3)/(9*8*7*6*5*4*3*2)-(x6*x5)/(11*10*9*8*7*6*5*4*3*2));
         //+(long double)(x6*x6*x)/(6227020800)-(long double)(x6*x6*x3)/(1307674368000));
}

double cosLW(float x)
{
  float x2 = x*x;
  float x3 = x2*x;
  float x4 = x2*x2;
  float x5 = x2*x3;
  float x6 = x3*x3;
  return (long double)(1-(x2)/(2)+(x4)/(4*3*2)-(x6)/(6*5*4*3*2)
         +(x6*x2)/(8*7*6*5*4*3*2)-(x5*x5)/(10*9*8*7*6*5*4*3*2));
}

#ifdef __cplusplus
}
#endif
