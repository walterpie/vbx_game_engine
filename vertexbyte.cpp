#include "vertexbyte.h"
#include "vertexbyte_soft.cpp"

extern "C"
INITIALIZE_WINDOW_STATE(initialize_window_state)
{
  Window_State state = {};
  
  state.width = 900;
  state.height = 700;
  state.buffer_width = 800;
  state.buffer_height = 600;
  state.title = "VBX";
  state.target_fps = 60.0f;

  return(state);
}

struct Game_State
{
  b32 is_initialized;
};

  
extern "C"
GAME_UPDATE_AND_RENDER(game_update_and_render)
{
  Game_State *game_state = (Game_State *)memory->permanent_memory;
  if(!game_state->is_initialized)
  {
    game_state->is_initialized = true;
  }
  clear_screen(draw_buffer, 0.0f, 0.0f, 0.0f);
}
