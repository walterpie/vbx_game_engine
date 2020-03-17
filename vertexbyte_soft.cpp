// NOTE(vertexbyte): These is were the software rendering routines are.
// TODO(vertexbyte): Use the draw pixel function that uses the u32
// as the color because its faster and it does not compute the pixel
// color every time the funcion is called
struct Color
{
  r32 r;
  r32 g;
  r32 b;
};

Color make_color(r32 r, r32 g, r32 b)
{
  Color result = {};
  result.r = r;
  result.g = g;
  result.b = b;
  return(result);
}

internal
void draw_pixel(Bitmap *buffer, s32 x, s32 y, u32 color)
{
  if((x > 0) && (x < buffer->width) && (y > 0) && (y < buffer->height))
  {
    u32 *pixel = ((u32 *)buffer->memory + x + y*buffer->width);
    *pixel = color;
  }
}


// NOTE(faruk): We want in some case that our shape wrapes "around"
// the screen
internal void
wrap_coordinates(s32 x, s32 y, s32 *wx, s32 *wy, s32 width, s32 height)
{
  *wx = x;
  *wy = y;

  if(x > width)
  {
    *wx = x - width;
  }
  else if(x < 0)
  {
    *wx = width + x;
  }
  if(y > height)
  {
    *wy = y - height;
  }
  else if(y < 0)
  {
    *wy = height + y;
  }
}

internal void
wrap_coordinates(r32 x, r32 y, r32 *wx, r32 *wy, s32 width, s32 height)
{
  *wx = x;
  *wy = y;

  if(x > width)
  {
    *wx = x - width;
  }
  else if(x < 0)
  {
    *wx = width + x;
  }
  if(y > height)
  {
    *wy = y - height;
  }
  else if(y < 0)
  {
    *wy = height + y;
  }
}

// NOTE(faruk): Just uses the wrap_coordinates function before drawing pixels
internal
void draw_pixel_wrap(Bitmap *buffer, s32 x, s32 y, u32 color)
{
  wrap_coordinates(x, y, &x, &y, buffer->width, buffer->height);
  u32 *pixel = ((u32 *)buffer->memory + x + y*buffer->width);
  *pixel = color;
}

internal
void draw_pixel_wrap(Bitmap *buffer, s32 x, s32 y, Color color)
{
  
  u32 color32 = (((u32)(255.0f * color.r)) << 16 |
		 ((u32)(255.0f * color.g)) << 8 |
		 ((u32)(255.0f * color.b)) << 0);
  
  wrap_coordinates(x, y, &x, &y, buffer->width, buffer->height);
  u32 *pixel = ((u32 *)buffer->memory + x + y*buffer->width);
  *pixel = color32;
}

internal
void draw_pixel(Bitmap *buffer, s32 x, s32 y, Color c)
{
  u32 color = (((u32)(255.0f * c.r)) << 16 |
	       ((u32)(255.0f * c.g)) << 8 |
	       ((u32)(255.0f * c.b)) << 0);

  if((x > 0) && (x < buffer->width) && (y > 0) && (y < buffer->height))
  {
    u32 *pixel = ((u32 *)buffer->memory + x + y*buffer->width);
    *pixel = color;
  }
}

internal
void clear_screen(Bitmap *buffer, Color color)
{
  u32 color32 = (((u32)(255.0f * color.r)) << 16 |
		 ((u32)(255.0f * color.g)) << 8 |
		 ((u32)(255.0f * color.b)) << 0);

  u8 *row = (u8 *)buffer->memory;
  
  for(s32 y = 0;
      y < buffer->height;
      ++y)
  {
    u32 *pixels = (u32 *)row;
    
    for(s32 x = 0;
	x < buffer->width;
	++x)
    {
      *pixels++ = color32;
    }

    row += buffer->pitch;
  }
}

//
// Rectangle rasterizer
//
internal
void fill_rect(Bitmap *buffer,
	       r32 real_min_x, r32 real_min_y,
	       r32 real_max_x, r32 real_max_y,
	       Color color)
{
  s32 min_x = round_real32_to_signed32(real_min_x);
  s32 min_y = round_real32_to_signed32(real_min_y);
  s32 max_x = round_real32_to_signed32(real_min_x + real_max_x);
  s32 max_y = round_real32_to_signed32(real_min_y + real_max_y);

  if((max_y < 0) || (max_x < 0) ||
     (max_y > buffer->height) || (max_x > buffer->width))
  {
    return;
  }
  
  //NOTE(vertexbyte): Clipping!!!
  if(min_x < 0)
  {
    min_x = 0;
  }
  if(min_y < 0)
  {
    min_y = 0;
  }
  if(max_x > buffer->width)
  {
    min_x = buffer->width;
  }
  if(max_y > buffer->height)
  {
    min_x = buffer->height;
  }
  
  u32 color32 = (((u32)(255.0f * color.r)) << 16 |
		 ((u32)(255.0f * color.g)) << 8 |
		 ((u32)(255.0f * color.b)) << 0);

  u8 *row = ((u8 *)buffer->memory + min_x*BYTES_PER_PIXEL +
	     min_y*buffer->pitch);

  for(s32 y = min_y;
      y < max_y;
      ++y)
  {
    u32 *pixels = (u32 *)row;
    
    for(s32 x = min_x;
	x < max_x;
	++x)
    {
      *pixels++ = color32;
    }

    row += buffer->pitch;
  }
}


internal
void fill_rect_wrap(Bitmap *buffer,
		    r32 real_min_x, r32 real_min_y,
		    r32 real_max_x, r32 real_max_y,
		    Color color)
{
  s32 min_x = round_real32_to_signed32(real_min_x);
  s32 min_y = round_real32_to_signed32(real_min_y);
  s32 max_x = round_real32_to_signed32(real_min_x + real_max_x);
  s32 max_y = round_real32_to_signed32(real_min_y + real_max_y);
  
  for(s32 y = min_y;
      y < max_y;
      ++y)
  {
    for(s32 x = min_x;
	x < max_x;
	++x)
    {
      draw_pixel_wrap(buffer, x, y, color);
    }
  }
}

//
// Line rasterizer
//
void draw_line(Bitmap *buffer,
	       r32 real_x1, r32 real_y1, r32 real_x2, r32 real_y2,
	       Color color)
{
  s32 x1 = round_real32_to_signed32(real_x1);
  s32 y1 = round_real32_to_signed32(real_y1);
  s32 x2 = round_real32_to_signed32(real_x2);
  s32 y2 = round_real32_to_signed32(real_y2);

  s32 run = absolute_value_s32((x2 - x1));
  s32 rise = absolute_value_s32((y2 - y1));

  u32 color32 = (((u32)(255.0f * color.r)) << 16 |
		 ((u32)(255.0f * color.g)) << 8 |
		 ((u32)(255.0f * color.b)) << 0);
  
  s32 x_go = 1;
  s32 y_go = 1;

  if(x2 < x1)
  {
    x_go = -1;
  }
  if(y2 < y1)
  {
    y_go = -1;
  }

  if(run > rise)
  {
    r32 slope = (r32)rise/(r32)run;

    s32 draw_x = x1;
    s32 draw_y = y1;
    
    r32 real_draw_y = y1;
    
    for(s32 x = 0;
	x < run;
	++x)
    {
      real_draw_y += slope*y_go;
      draw_x += x_go;
      draw_y = (s32)real_draw_y;
      
      draw_pixel(buffer, draw_x, draw_y, color32);
    }
  }
  else
  {
    r32 slope = (r32)run/(r32)rise;

    s32 draw_x = x1;
    s32 draw_y = y1;
    
    r32 real_draw_x = x1;
    
    for(s32 y = 0;
	y < rise;
	++y)
    {
      real_draw_x += slope*x_go;
      
      draw_y += y_go;
      draw_x = (s32)real_draw_x;
      
      draw_pixel(buffer, draw_x, draw_y, color32);
    }
  }
}

void draw_line_wrap(Bitmap *buffer,
		    r32 real_x1, r32 real_y1, r32 real_x2, r32 real_y2,
		    Color color)
{
  s32 x1 = round_real32_to_signed32(real_x1);
  s32 y1 = round_real32_to_signed32(real_y1);
  s32 x2 = round_real32_to_signed32(real_x2);
  s32 y2 = round_real32_to_signed32(real_y2);
  
  s32 run = absolute_value_s32((x2 - x1));
  s32 rise = absolute_value_s32((y2 - y1));

  u32 color32 = (((u32)(255.0f * color.r)) << 16 |
		 ((u32)(255.0f * color.g)) << 8 |
		 ((u32)(255.0f * color.b)) << 0);
  
  
  s32 x_go = 1;
  s32 y_go = 1;

  if(x2 < x1)
  {
    x_go = -1;
  }
  if(y2 < y1)
  {
    y_go = -1;
  }

  if(run > rise)
  {
    r32 slope = (r32)rise/(r32)run;

    s32 draw_x = x1;
    s32 draw_y = y1;
    
    r32 real_draw_y = y1;
    
    for(s32 x = 0;
	x < run;
	++x)
    {
      real_draw_y += slope*y_go;
      draw_x += x_go;
      draw_y = (s32)real_draw_y;
      
      draw_pixel_wrap(buffer, draw_x, draw_y, color32);
    }
  }
  else
  {
    r32 slope = (r32)run/(r32)rise;

    s32 draw_x = x1;
    s32 draw_y = y1;
    
    r32 real_draw_x = x1;
    
    for(s32 y = 0;
	y < rise;
	++y)
    {
      real_draw_x += slope*x_go;
      
      draw_y += y_go;
      draw_x = (s32)real_draw_x;
      
      draw_pixel_wrap(buffer, draw_x, draw_y, color32);
    }
  }
}


//
// Circle rasterizer
//
void draw_circle_slow(Bitmap *buffer, r32 real_center_x, r32 real_center_y,
		      r32 real_radius, Color color)
{
  s32 center_x = round_real32_to_signed32(real_center_x);
  s32 center_y = round_real32_to_signed32(real_center_y);
  s32 radius = round_real32_to_signed32(real_radius);
  s32 radius_squared = radius*radius;

  u32 color32 = (((u32)(255.0f * color.r)) << 16 |
		 ((u32)(255.0f * color.g)) << 8 |
		 ((u32)(255.0f * color.b)) << 0);

  draw_pixel(buffer, center_x + radius, center_y, color32);
  draw_pixel(buffer, center_x - radius, center_y, color32);
  draw_pixel(buffer, center_x, center_y - radius, color32);
  draw_pixel(buffer, center_x, center_y + radius, color32);


  s32 x = 0;
  s32 y = round_real32_to_signed32(sqrt(radius_squared));
  
  while(x < y)
  {
    draw_pixel(buffer, center_x + x, center_y + y, color32);
    draw_pixel(buffer, center_x + x, center_y - y, color32);
    draw_pixel(buffer, center_x - x, center_y + y, color32);
    draw_pixel(buffer, center_x - x, center_y - y, color32);
    draw_pixel(buffer, center_x + y, center_y + x, color32);
    draw_pixel(buffer, center_x + y, center_y - x, color32);
    draw_pixel(buffer, center_x - y, center_y + x, color32);
    draw_pixel(buffer, center_x - y, center_y - x, color32);
    
    ++x;
    y = round_real32_to_signed32(sqrt((radius_squared) - x*x));
  }
}

void fill_circle_slow(Bitmap *buffer, r32 real_center_x, r32 real_center_y,
		      r32 real_radius, Color color)
{
  s32 center_x = round_real32_to_signed32(real_center_x);
  s32 center_y = round_real32_to_signed32(real_center_y);
  s32 radius = round_real32_to_signed32(real_radius);
  s32 radius_squared = radius*radius;
  
  s32 x = 0;
  s32 y = round_real32_to_signed32(sqrt(radius_squared));
  
  while(x < y)
  {
    draw_line(buffer, center_x + x, center_y + y,
	      center_x + x, center_y - y, color);
    
    draw_line(buffer, center_x - x, center_y + y,
	      center_x - x, center_y - y, color);
    
    draw_line(buffer, center_x + y, center_y + x,
	      center_x + y, center_y - x, color);
    
    draw_line(buffer, center_x - y, center_y + x,
	      center_x - y, center_y - x, color);
        
    ++x;
    y = round_real32_to_signed32(sqrt((radius_squared) - x*x));
  }
}

//
// Trinalge rasterizer
//
void fill_triangle_bottom_flat(Bitmap *buffer, Vec3r v1,
			       Vec3r v2, Vec3r v3, Color color)
{
  r32 y_start = v1.y;
  
  // NOTE(vertexbyte): This can ba left.y or right.y they have the same y
  r32 y_end = v2.y;

  r32 slope1 = (v2.x - v1.x)/(v2.y - v1.y);
  r32 slope2 = (v3.x - v1.x)/(v3.y - v1.y);

  r32 current_x1 = v1.x;
  r32 current_x2 = v1.x;

  for(r32 y = y_start;
      y <= y_end;
      ++y)
  {
    draw_line(buffer, current_x1, y, current_x2, y, color);
    
    current_x1 += slope1;
    current_x2 += slope2;
  }
}

void fill_triangle_top_flat(Bitmap *buffer, Vec3r v1,
			    Vec3r v2, Vec3r v3, Color color)
{
  // NOTE(vertexbyte): This can ba left.y or right.y they have the same y
  r32 y_start = v3.y;
  r32 y_end = v1.y;
  
  r32 slope1 = (v3.x - v1.x)/(v3.y - v1.y);
  r32 slope2 = (v3.x - v2.x)/(v3.y - v2.y);

  r32 current_x1 = v3.x;
  r32 current_x2 = v3.x;

  for(r32 y = y_start;
      y > y_end;
      --y)
  {
    draw_line(buffer, current_x1, y, current_x2, y, color);
    
    current_x1 -= slope1;
    current_x2 -= slope2;
  }
}

void fill_triangle(Bitmap *buffer, Vec3r v1, Vec3r v2, Vec3r v3,
		   Color color)
{
  // NOTE(vertexbyte): We need to sort the verteces by y cordinate
  if(v3.y < v2.y)
  {
    Vec3r temp = v3;
    v3 = v2;
    v2 = temp;
  }
  if(v2.y < v1.y)
  {
    Vec3r temp = v2;
    v2 = v1;
    v1 = temp;
  }

  if(v1.y == v2.y)
  {
    fill_triangle_top_flat(buffer, v1, v2, v3, color);
  }
  else if(v2.y == v3.y)
  {
    fill_triangle_bottom_flat(buffer, v1, v2, v3, color);
  }
  else
  {
    Vec3r v4 = {};
    v4.y = v2.y;
    v4.x = v1.x + ((v3.x - v1.x)*(v2.y - v1.y))/(v3.y - v1.y);

    fill_triangle_bottom_flat(buffer, v1, v2, v4, color);
    fill_triangle_top_flat(buffer, v2, v4, v3, color);
  }
}
