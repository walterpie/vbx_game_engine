#include <windows.h>
#include <stdio.h>
#include "vertexbyte.h"

struct Win32_Game_Code
{
  HMODULE game_dll;
  Game_Update_And_Render *game_update_and_render;
  Initialize_Window_State *initialize_window_state;
};

struct Win32_Display_Buffer
{
  BITMAPINFO info;
  s32 width;
  s32 height;
  s32 pitch;
  void *memory;
};

global_variable b32 global_running;
global_variable Window_State global_window_state;
global_variable u64 counts_per_second;

internal void
load_game_code(Win32_Game_Code *game_code)
{
  CopyFile("vertexbyte.dll", "vertexbyte_load.dll", 0);
  
  game_code->game_dll = LoadLibraryA("vertexbyte_load.dll");
  
  if(game_code->game_dll)
  {
    // NOTE(vertexbyte): Loading the functions for the dll
    game_code->game_update_and_render =
      (Game_Update_And_Render *)GetProcAddress(game_code->game_dll, "game_update_and_render");
    
    game_code->initialize_window_state =
      (Initialize_Window_State *)GetProcAddress(game_code->game_dll, "initialize_window_state");
  }
}

internal void
unload_game_code(Win32_Game_Code *game_code)
{
  
  if(game_code->game_dll)
  {
    FreeLibrary(game_code->game_dll);
  }

  game_code->game_update_and_render = game_update_and_render_stub;
  game_code->initialize_window_state = initialize_window_state_stub;
}


internal void
win32_initialize_display_buffer(Win32_Display_Buffer *buffer,
				int width, int height)
{
  buffer->width = width;
  buffer->height = height;
  buffer->pitch = BYTES_PER_PIXEL*buffer->width;
  int buffer_size = buffer->pitch*buffer->height;
  
  buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
  buffer->info.bmiHeader.biWidth = width;
  buffer->info.bmiHeader.biHeight = -height;
  
  buffer->info.bmiHeader.biPlanes = 1;
  buffer->info.bmiHeader.biBitCount = 32;
  buffer->info.bmiHeader.biCompression = BI_RGB; // NOTE(vertexybte): Uncompressed

  buffer->memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE,
			       PAGE_READWRITE);
}

internal void
win32_copy_display_buffer_to_screen(Win32_Display_Buffer *buffer,
				    HDC device_context)
{
  StretchDIBits(device_context,
		0, 0, buffer->width, buffer->height,
		0, 0, buffer->width, buffer->height,
		buffer->memory, &buffer->info,
		DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
win32_main_window_procedure(HWND window,
			    UINT message,
			    WPARAM wparam,
			    LPARAM lparam)
{
  LRESULT result = 0;
  switch(message)
  {
    case WM_CLOSE:
    {
      global_running = false;
    }break;
    case WM_PAINT:
    {
      PAINTSTRUCT ps = {};
      HDC device_context = BeginPaint(window, &ps);
      int x = ps.rcPaint.left;
      int y = ps.rcPaint.top;
      int width = ps.rcPaint.right;
      int height = ps.rcPaint.bottom;
      PatBlt(device_context, x, y, width, height, WHITENESS);
      EndPaint(window, &ps);
    }break;
    default:
    {
      result = DefWindowProc(window, message, wparam, lparam);
    }break;
  }

  return(result);
}

// TODO(vertexbyte): Mouse buttons need to get handeled!!!
void win32_process_peek_messages(Input *new_input, Input *old_input,
				 HWND window)
{
  MSG msg = {};
  while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
  {
    switch(msg.message)
    {
      case WM_KEYDOWN:
      case WM_KEYUP:
      {
	u32 vk_code = (u32)msg.wParam;
	b32 is_down = ((msg.lParam >> 31) == 0);
	b32 was_down = ((msg.lParam >> 30) == 1);

#define process_keyboard_button(vk, index)\
case vk:\
{\
  new_input->keyboard_buttons[index].is_down = is_down;\
  new_input->keyboard_buttons[index].transition_count += ((is_down & old_input->keyboard_buttons[index].is_down) ? 0 : 1); \
}break;

	switch(vk_code)
	{
	  process_keyboard_button(VK_UP, BUTTON_UP);
	  process_keyboard_button(VK_DOWN, BUTTON_DOWN);
	  process_keyboard_button(VK_LEFT, BUTTON_LEFT);
	  process_keyboard_button(VK_RIGHT, BUTTON_RIGHT);

	  process_keyboard_button('W', BUTTON_W);
	  process_keyboard_button('A', BUTTON_A);
	  process_keyboard_button('S', BUTTON_S);
	  process_keyboard_button('D', BUTTON_D);

	  // TODO(vertexbyte): We will add more buttons later...
	}
      }break;
      case WM_MOUSEMOVE:
      {
	new_input->mouse_x = (msg.lParam & 0xFFFF);
	new_input->mouse_y = ((msg.lParam >> 16) & 0xFFFF);
      }break;
      default:
      {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
      }break;
    }
  }
}

// NOTE(vertexbyte): This returns the counts of the process
u64 win32_get_wall_clock()
{
  u64 result = 0;
  LARGE_INTEGER large_integer = {};
  QueryPerformanceCounter(&large_integer);
  result = large_integer.QuadPart;
  
  return(result);
}

r32 win32_wall_clock_ms(u64 start, u64 end)
{
  r32 result = 0.0f;
  u64 elapsed = end - start;

  result = (r32)elapsed/(r32)counts_per_second;

  return(result);
}

INT WinMain(HINSTANCE instance,
	    HINSTANCE prev_instance,
	    PSTR command_line,
	    INT command_show)
{
  // NOTE(vertexbyte): Get the counts per second(fixed at system boot time)
  LARGE_INTEGER pref_freq = {};
  QueryPerformanceFrequency(&pref_freq);
  counts_per_second = pref_freq.QuadPart;

  b32 time_is_granular = true;
  
  if(timeBeginPeriod(1) == TIMERR_NOCANDO)
  {
    time_is_granular = false;
  }
  
  WNDCLASSA window_class = {};
  window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc = win32_main_window_procedure;
  window_class.lpszClassName = "vbx_class";

  if(RegisterClassA(&window_class))
  {
    Win32_Game_Code game_code = {};
    load_game_code(&game_code);

    global_window_state = game_code.initialize_window_state();
    
    HWND window = CreateWindowEx(0, window_class.lpszClassName,
				 global_window_state.title,
				 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				 CW_USEDEFAULT, CW_USEDEFAULT,
				 global_window_state.width,
				 global_window_state.height,
				 0, 0, 0, 0);
    if(window)
    {
      global_running = true;
      HDC device_context = GetDC(window);
      
      Win32_Display_Buffer display_buffer = {};
      Bitmap draw_buffer = {};
      
      win32_initialize_display_buffer(&display_buffer,
				      global_window_state.buffer_width,
				      global_window_state.buffer_height);


      draw_buffer.width = display_buffer.width;
      draw_buffer.height = display_buffer.height;
      draw_buffer.pitch = display_buffer.pitch;
      draw_buffer.memory = display_buffer.memory;
	
      Input input[2] = {};
      
      Input *new_input = &input[0];
      Input *old_input = &input[1];

      u64 start_counter = win32_get_wall_clock();
      u64 start_cycles = __rdtsc();

      r32 target_delta_time = 1.0f/global_window_state.target_fps;
      r32 delta_time = target_delta_time;
      r32 win32_passed_time = 0.0f;

      WIN32_FIND_DATA starting_file_time;
      WIN32_FIND_DATA changed_file_time;

      FindFirstFileA("vertexbyte.dll", &starting_file_time);
      
      while(global_running)
      {
	FindFirstFileA("vertexbyte.dll", &changed_file_time);

	FILETIME starting_time = starting_file_time.ftLastWriteTime;
	FILETIME changed_time = changed_file_time.ftLastWriteTime;
	
	if(CompareFileTime(&starting_time, &changed_time) != 0)
	{
	  unload_game_code(&game_code);
	  load_game_code(&game_code);
	  starting_file_time = changed_file_time;
	}
		
	win32_process_peek_messages(new_input, old_input, window);
	game_code.game_update_and_render(&draw_buffer, input, delta_time);
	
	// NOTE(vertexbyte): GUR stands for game update and render
	u64 end_counter = win32_get_wall_clock();
	r32 gur_ms = win32_wall_clock_ms(start_counter,
					 end_counter);

	if(gur_ms < target_delta_time)
	{
	  while(gur_ms < target_delta_time)
	  {
	    if(time_is_granular)
	    {
	      s32 sleep_time = (s32)(target_delta_time*1000) - (s32)(gur_ms*1000);
	      Sleep(sleep_time);
	    }

	    end_counter = win32_get_wall_clock();
	    gur_ms = win32_wall_clock_ms(start_counter,
					 end_counter);
	  }
	}
	
	win32_copy_display_buffer_to_screen(&display_buffer,
					    device_context);

	//
	// Outputing frame time
	//

	u64 counts_per_frame = end_counter - start_counter;
	u64 end_cycles = __rdtsc();

	u64 cycles_elapsed = end_cycles - start_cycles;

	r32 ms_per_frame = (r32)counts_per_frame/(r32)counts_per_second;
	delta_time = ms_per_frame;
	r32 fps = 1.0f/ms_per_frame;
	u32 mc_per_frame = cycles_elapsed / (1000*1000);

	win32_passed_time += ms_per_frame;
	
	if(win32_passed_time >= 1.0f)
	{
	  char buffer[256];
	  sprintf(buffer, "MS: %.4f FPS: %f MPF: %d\n", ms_per_frame,
		  fps, mc_per_frame);
	  
	  OutputDebugStringA(buffer);
	  win32_passed_time = 0.0f;
	}
	
	start_counter = end_counter;
	start_cycles = end_cycles;

	*old_input = *new_input;
      }
    }
    else
    {
      // TODO(vertexbyte): Logging???
    }
  }
  else
  {
    // TODO(vertexbyte): Logging???
  }
 
  return(0);
}
