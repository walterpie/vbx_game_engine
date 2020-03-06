#include "vertexbyte.h"

// NOTE(vertexbyte): This will mostly be used for a wrap effect of like in
// asteroids when the ship or asteroid goes off the screen and is wrapes
// on the other side
internal
void draw_pixel_slow(Bitmap *buffer, s32 x, s32 y, u32 color)
{
  if((x > 0) && (x < buffer->width) && (y > 0) && (y < buffer->height))
  {
    
    u8 *pixel = ((u8 *)buffer->memory + x*BYTES_PER_PIXEL
		 + y*buffer->pitch);
    
    *pixel = color;
  }
}

internal
void clear_screen(Bitmap *buffer,
		  r32 r, r32 g, r32 b)
{
  u32 color = (((u32)(255.0f * r)) << 16 |
	       ((u32)(255.0f * g)) << 8 |
	       ((u32)(255.0f * b)) << 0);

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
      *pixels++ = color;
    }

    row += buffer->pitch;
  }
}

internal
void draw_rectangle(Bitmap *buffer,
		    r32 real_min_x, r32 real_min_y,
		    r32 real_max_x, r32 real_max_y,
		    r32 r, r32 g, r32 b)
{
  s32 min_x = round_real32_to_signed32(real_min_x);
  s32 min_y = round_real32_to_signed32(real_min_y);
  s32 max_x = round_real32_to_signed32(real_min_x + real_max_x);
  s32 max_y = round_real32_to_signed32(real_min_y + real_max_y);

  if((max_y < 0) || (max_x < 0))
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
  
  u32 color = (((u32)(255.0f * r)) << 16 |
	       ((u32)(255.0f * g)) << 8 |
	       ((u32)(255.0f * b)) << 0);

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
      *pixels++ = color;
    }

    row += buffer->pitch;
  }
}


void draw_line_slow(Bitmap *buffer,
		    r32 real_x1, r32 real_y1, r32 real_x2, r32 real_y2,
		    r32 r, r32 g, r32 b)
{
  s32 x1 = round_real32_to_signed32(real_x1);
  s32 y1 = round_real32_to_signed32(real_y1);
  s32 x2 = round_real32_to_signed32(real_x2);
  s32 y2 = round_real32_to_signed32(real_y2);

  u32 color = (((u32)(255.0f * r)) << 16 |
	       ((u32)(255.0f * g)) << 8 |
	       ((u32)(255.0f * b)) << 0);
  
  s32 run = absolute_value_s32((x2 - x1));
  s32 rise = absolute_value_s32((y2 - y1));
  
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
      
      draw_pixel_slow(buffer, draw_x, draw_y, color);
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
      
      draw_pixel_slow(buffer, draw_x, draw_y, color);
    }
  }
}


internal
Window_State initialize_window_state()
{
  Window_State state = {};
  
  state.width = 800;
  state.height = 600;
  state.buffer_width = 800;
  state.buffer_height = 600;
  state.title = "VBX";

  return(state);
}

internal void
game_update_and_render(Bitmap *draw_buffer, Input *input)
{
  clear_screen(draw_buffer, 0.0f, 0.0f, 0.0f);
}
