#if !defined(VERTEXBYTE_H)

#include <stdint.h>
#include <math.h>

// Signed values
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

// Unsigned values
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Real numbers
typedef float r32;
typedef double r64;

// Bool
typedef s32 b32;

#define BYTES_PER_PIXEL 4

#define internal static
#define local_persist static
#define global_variable static


// TODO(faruk): Move to intrinsics!!!!
r32 round_real32_to_signed32(r32 value)
{
  s32 result = (s32)(value + 0.5f);
  return(result);
}

s32 absolute_value_s32(s32 value)
{
  s32 result = value;
  if(value < 0)
  {
    result = -value;
  }
  return(value);
}


enum
{
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_W,
  BUTTON_A,
  BUTTON_S,
  BUTTON_D,
  BUTTON_E,
  BUTTON_Q,
  BUTTON_R,
  BUTTON_COUNT
};

struct Keyboard_Button
{
  b32 is_down;
  s32 transition_count;
};

struct Input
{
  // Mouse
  s32 mouse_x;
  s32 mouse_y;

  // TODO(vertexbyte): Handle mouse buttons
  s32 mbl_down;
  s32 mbr_down;
  
  // Keyboard
  Keyboard_Button keyboard_buttons[BUTTON_COUNT];
};

struct Window_State
{
  s32 width;
  s32 height;
  s32 buffer_width;
  s32 buffer_height;
  char *title;
};
  
struct Bitmap
{
  s32 width;
  s32 height;
  s32 pitch;
  void *memory;
};

internal
Window_State initialize_window_state();

internal
void draw_pixel_slow(Bitmap *buffer, s32 x, s32 y, u32 color);

internal
void clear_screen(Bitmap *buffer, r32 r, r32 g, r32 b);


internal
void draw_rectangle(Bitmap *buffer, r32 real_min_x, r32 real_min_y,
		    r32 real_max_x, r32 real_max_y,
		    r32 r, r32 g, r32 b);

internal
void draw_line_slow(Bitmap *buffer, r32 real_x1, r32 real_y1,
		    r32 real_x2, r32 real_y2,
		    r32 r, r32 g, r32 b);

internal
void game_update_and_render(Bitmap *draw_buffer, Input *input);
  

#define VERTEXBYTE_H
#endif
