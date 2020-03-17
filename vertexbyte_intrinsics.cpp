#include <intrin.h>
#pragma intrinsic(__rdtsc)

r32 round_real32_to_signed32(r32 value)
{
  s32 result = (s32)(value + 0.5f);
  return(result);
}

s32 absolute_value_s32(s32 value)
{
  s32 result = value;
  if(result < 0)
  {
    result = -result;
  }
  return(result);
}


r32 absolute_value_r32(r32 value)
{
  r32 result = value;
  if(result < 0)
  {
    result = -result;
  }
  return(result);
}
