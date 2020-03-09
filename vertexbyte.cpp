#include "vertexbyte.h"
#include "vertexbyte_soft.cpp"

extern "C"
INITIALIZE_WINDOW_STATE(initialize_window_state)
{
  Window_State state = {};
  
  state.width = 800;
  state.height = 600;
  state.buffer_width = 800;
  state.buffer_height = 600;
  state.title = "VBX";
  state.target_fps = 60.0f;

  return(state);
}

extern "C"
GAME_UPDATE_AND_RENDER(game_update_and_render)
{
  clear_screen(draw_buffer, 0.0f, 0.0f, 0.0f);

  if(button_down(input, BUTTON_W))
  {
    y -= 400.0f*delta_time;
  }
  if(button_down(input, BUTTON_A))
  {
    x -= 400.0f*delta_time;
  }
  if(button_down(input, BUTTON_S))
  {
    y += 400.0f*delta_time;
  }
  if(button_down(input, BUTTON_D))
  {
    x += 400.0f*delta_time;
  }
}
