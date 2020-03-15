#if !defined(VERTEXBYTE_H)

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
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

/*
  NOTE(vertexbyte):

    VERTEXBYTE_INTERNAL: 
      - 0 release build
      - 1 build that only runs on only our this machine

    VERTEXBYTE_SLOW: 
      - 0 no slow code
      - 1 slow code allowed
 */

#include "vertexbyte_intrinsics.cpp"
#include "vertexbyte_memory.cpp"
#include "vertexbyte_math.cpp"

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
  r32 target_fps;
};
  
struct Bitmap
{
  s32 width;
  s32 height;
  s32 pitch;
  void *memory;
};

// NOTE(vertexbyte): Routines to determin the button states
// NOTE(vertexbyte): Our input pointer ponits to two input structures
// because we have a new_input(index 0) and a old_input(index 1)
b32 button_down(Input *input, s32 index)
{
  b32 result = false;
  if(input[0].keyboard_buttons[index].is_down)
  {
    result = true;
  }
  return(result);
}

b32 button_released(Input *input, s32 index)
{
  b32 result = false;
  if(!input[0].keyboard_buttons[index].is_down &&
     input[1].keyboard_buttons[index].is_down)
  {
    result = true;
  }
  return(result);
}

b32 button_pressed(Input *input, s32 index)
{
  b32 result = false;
  if(input[0].keyboard_buttons[index].is_down &&
     !input[1].keyboard_buttons[index].is_down)
  {
    result = true;
  }
  return(result);
}


// NOTE(vertexbyte): game_update_and_render
#define GAME_UPDATE_AND_RENDER(name) void name(Bitmap *draw_buffer, Input *input, Game_Memory *memory, r32 delta_time)
typedef GAME_UPDATE_AND_RENDER(Game_Update_And_Render);

GAME_UPDATE_AND_RENDER(game_update_and_render_stub)
{
  return;
}

// NOTE(vertexbyte): initialize_window_state
#define INITIALIZE_WINDOW_STATE(name) Window_State name()
typedef INITIALIZE_WINDOW_STATE(Initialize_Window_State);

INITIALIZE_WINDOW_STATE(initialize_window_state_stub)
{
  Window_State state = {};
  return(state);
}

#define VERTEXBYTE_H
#endif
