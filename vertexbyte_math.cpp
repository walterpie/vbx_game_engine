#define PI32 3.14159265359 // PI

//
// Vectors
//

// 2d
struct V2
{
  r32 x;
  r32 y;
};

r32 get_length_v2(V2 v)
{
  r32 result = sqrtf(v.x*v.x + v.y*v.y);
  return(result);
}

void set_angle_v2(V2 *v, r32 angle)
{
  r32 lenght = get_length_v2(*v);
  v->x = cosf(angle)*lenght;
  v->y = sinf(angle)*lenght;
}

r32 get_angle_v2(V2 v)
{
  r32 result = atan2f(v.y, v.x);
  return(result);
}

void set_length_v2(V2 *v, r32 length)
{
  r32 angle = get_angle_v2(*v);
  v->x = cosf(angle)*length;
  v->y = sinf(angle)*length;
}


V2 make_v2(r32 x, r32 y)
{
  V2 result = {};
  result.x = x;
  result.y = y;
  return(result);
}

V2 operator+(V2 a, V2 b)
{
  V2 result = {};
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return(result);
}

V2 operator-(V2 a, V2 b)
{
  V2 result = {};
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  return(result);
}

V2 operator*(V2 a, r32 b)
{
  V2 result = {};
  result.x = a.x * b;
  result.y = a.y * b;
  return(result);
}

V2 operator/(V2 a, r32 b)
{
  V2 result = {};
  result.x = a.x / b;
  result.y = a.y / b;
  return(result);
}

// 3d
struct V3
{
  r32 x;
  r32 y;
  r32 z;
};

V3 make_v3(r32 x, r32 y, r32 z)
{
  V3 result = {};
  result.x = x;
  result.y = y;
  result.z = z;
  return(result);
}

