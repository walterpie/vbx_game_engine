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
  clear_screen(draw_buffer, make_color(0.0f, 0.0f, 0.0f));

  draw_circle_slow(draw_buffer, 300, 300, 50, make_color(0.0f, 1.0f, 0.0f));
  
  fill_triangle(draw_buffer, make_vec3r(100, 300, 0),
		make_vec3r(50, 400, 0), make_vec3r(140, 400, 0),
		make_color(1.0f, 0.0f, 0.0f));
  
  fill_triangle(draw_buffer, make_vec3r(400, 200, 0),
		make_vec3r(500, 200, 0), make_vec3r(450, 400, 0),
		make_color(1.0f, 0.0f, 0.0f));

  fill_triangle(draw_buffer, make_vec3r(320, 200, 0),
		make_vec3r(400, 300, 0), make_vec3r(300, 400, 0),
		make_color(1.0f, 1.0f, 1.0f));
  
}
