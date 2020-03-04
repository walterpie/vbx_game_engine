#include <windows.h>
#include "vertexbyte.cpp"

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

INT WinMain(HINSTANCE instance,
	    HINSTANCE prev_instance,
	    PSTR command_line,
	    INT command_show)
{
  WNDCLASSA window_class = {};
  window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc = win32_main_window_procedure;
  window_class.lpszClassName = "vbx_class";

  
  if(RegisterClassA(&window_class))
  {
    global_window_state = initialize_window_state();
      
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
      
      while(global_running)
      {
	win32_process_peek_messages(new_input, old_input, window);
	game_update_and_render(&draw_buffer, input);
	*old_input = *new_input;

	win32_copy_display_buffer_to_screen(&display_buffer,
					    device_context);
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
