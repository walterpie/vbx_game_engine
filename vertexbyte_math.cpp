//
// Vectors
//

struct Vec2r
{
  r32 x;
  r32 y;
};

internal
Vec2r make_vec2r(r32 x, r32 y)
{
  Vec2r result = {};
  result.x = x;
  result.y = y;
  return(result);
}

Vec2r operator+(Vec2r a, Vec2r b)
{
  Vec2r result = {};
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return(result);
}

Vec2r operator-(Vec2r a, Vec2r b)
{
  Vec2r result = {};
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  return(result);
}

Vec2r operator*(Vec2r a, Vec2r b)
{
  Vec2r result = {};
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  return(result);
}

Vec2r operator*(Vec2r a, r32 real_b)
{
  Vec2r result = {};
  result.x = a.x * real_b;
  result.y = a.y * real_b;
  return(result);
}


Vec2r operator/(Vec2r a, Vec2r b)
{
  Vec2r result = {};
  result.x = a.x / b.x;
  result.y = a.y / b.y;
  return(result);
}

