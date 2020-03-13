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

#define FROGGER_SIZE 20
#define GRID_SIZE 30
#define OBSTICLE_SIZE 25


// NOTE(vertexbyte): In this case because fogger is such a small game
// these global variables are acceptable
// NOTE(vertexbyte): Our resolution is always 800x600.
#define STARTING_X (800/2)
#define STARTING_Y  (600 - (GRID_SIZE-5))

Vec2r frogger = make_vec2r(STARTING_X, STARTING_Y);
Color frogger_color = make_color(0.3f, 1.0f, 0.2f);
s32 frogger_lane_index = (600/GRID_SIZE) - 1;
r32 frogger_x_velocity = 0.0f;

enum
{
  LANE_GRASS,
  LANE_CAR,
  LANE_WATTER
};

struct Lane
{
  r32 speed;
  Color color;
  s32 type;
};


#define GRASS_LANE {0.0f, make_color(0.4f, 0.6f, 0.4f), LANE_GRASS}
#define CAR_LANE_SLOW {70.0f, make_color(0.8f, 0.8f, 0.8f), LANE_CAR}
#define CAR_LANE_MEDIUM {120.0f, make_color(0.8f, 0.8f, 0.8f), LANE_CAR}
#define CAR_LANE_FAST {200.0f, make_color(0.8f, 0.8f, 0.8f), LANE_CAR}
#define WATTER_LANE_SLOW {70.0f, make_color(0.2f, 0.1f, 0.8f), LANE_WATTER}
#define WATTER_LANE_FAST {180.0f, make_color(0.2f, 0.1f, 0.8f), LANE_WATTER}

// NOTE(vertexbyte): Logs are also considered obsticles
struct Obsticle
{
  Vec2r pos;
  Vec2r dim;
  Color color;
  s32 lane_index;
};


Lane lanes[600/GRID_SIZE] =
{
  GRASS_LANE, WATTER_LANE_SLOW,
  WATTER_LANE_FAST, WATTER_LANE_SLOW,
  WATTER_LANE_FAST,

  GRASS_LANE, CAR_LANE_MEDIUM,
  CAR_LANE_SLOW, CAR_LANE_FAST,
  CAR_LANE_MEDIUM,

  GRASS_LANE,
  WATTER_LANE_SLOW, WATTER_LANE_FAST,
  GRASS_LANE,
  
  CAR_LANE_SLOW,
  CAR_LANE_MEDIUM,
  CAR_LANE_FAST,
  CAR_LANE_SLOW,
  
  GRASS_LANE, GRASS_LANE
};

#define MAX_OBSTICLES 128
Obsticle obsticles[MAX_OBSTICLES];
s32 obsticle_count = 0;

// NOTE(vertexbyte): The dimensions of the vehicle to be more preciese
// its just the width and the height of the vehicle
Vec2r car_dims[2] = 
{
  {1.0f, 1.0f}, // regular car
  {3.0f, 1.0f}  // truck
};


Color car_colors[3] =
{
  {1.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 1.0f},
  {0.0f, 0.0f, 0.0f}
};

Vec2r log_dims[3] = 
{
  {3.0f, 1.0f}, 
  {4.0f, 1.0f},
  {5.0f, 1.0f}
};

Color log_colors[3] =
{
  {1.0f, 0.4f, 0.2f},
  {1.0f, 0.4f, 0.2f},
  {1.0f, 0.4f, 0.2f}
};

// NOTE(vertexbyte): 
void add_obsticle_to_lane(s32 lane_index, s32 count)
{

  // NOTE(vertexbyte): The colors pointer well be our color array
  // for the current lane type so if the lane type is watter then the
  // obsticle will have a orange color if its a car lane it will have
  // one of the 3 car colors
  Vec2r dim;
  Color *colors;
  if(lanes[lane_index].type == LANE_CAR)
  {
    dim = car_dims[rand() % 2];
    colors = car_colors;
  }
  if(lanes[lane_index].type == LANE_WATTER)
  {
    dim = log_dims[rand() % 3];
    colors = log_colors;
  }
    
  r32 starting_x = rand() % (GRID_SIZE*5);

  r32 align_y = (GRID_SIZE - OBSTICLE_SIZE)/2;
  r32 starting_y = lane_index*GRID_SIZE + align_y;

  s32 spacing;

  if(count < 3)
  {
    spacing = 200; // @hardcoded
  }
  else
  {
    spacing = (800/count) - (rand() % (s32)dim.x*OBSTICLE_SIZE);
  }

  for(s32 obsticles_created = 0;
      obsticles_created < count;
      ++obsticles_created)
  {
    obsticles[obsticle_count].pos.x = starting_x + spacing*obsticles_created;
    obsticles[obsticle_count].pos.y = starting_y;
    obsticles[obsticle_count].dim = dim;
    obsticles[obsticle_count].lane_index = lane_index;
    obsticles[obsticle_count].color = colors[rand() % 3];
    
    ++obsticle_count;
  }
}

void draw_and_update_obsticles(Bitmap *draw_buffer, r32 delta_time)
{
  for(s32 obsticle_index = 0;
      obsticle_index < obsticle_count;
      ++obsticle_index)
    {
      Vec2r *pos = &obsticles[obsticle_index].pos;
      
      Vec2r dim = obsticles[obsticle_index].dim;
      Color c = obsticles[obsticle_index].color;
      r32 speed = lanes[obsticles[obsticle_index].lane_index].speed;
      
      draw_rectangle_wrap(draw_buffer, pos->x, pos->y, dim.x*OBSTICLE_SIZE,
			  dim.y*OBSTICLE_SIZE,
			  c.r, c.g, c.b);

      // Moving the obsticle
      obsticles[obsticle_index].pos.x -= speed*delta_time;
            
      // NOTE(vertexbyte): Wraping the x when off screen
      wrap_coordinates(pos->x, pos->y, &pos->x, &pos->y,
		       draw_buffer->width, draw_buffer->height);
  }
}

void draw_lanes(Bitmap *draw_buffer)
{
  for(s32 i = 0;
      i < 600/GRID_SIZE;
      ++i)
  {    
    draw_rectangle(draw_buffer, 0, 0+(GRID_SIZE*i), 800, GRID_SIZE,
		   lanes[i].color.r, lanes[i].color.g, lanes[i].color.b);
  }
}


void check_collisions()
{
  b32 on_log = false;
  
  for(s32 obsticle_index = 0;
      obsticle_index < obsticle_count;
      ++obsticle_index)
  {
    // In the car lane when frogger colides with a car he dies, but
    // when he is on the water lane and did not colide with the log or
    // in other words he is not on the log then he dies
    if(obsticles[obsticle_index].lane_index == frogger_lane_index)
    {
      Vec2r pos = obsticles[obsticle_index].pos;
      Vec2r dim = obsticles[obsticle_index].dim;
	
      if((lanes[frogger_lane_index].type == LANE_CAR))
      {
	// NOTE(vertexbyte): AABB
	// NOTE(vertexbyte): We only use forggers centered
	/// position for collision checking this is to give
	// the player some wiggle roomw
	
	if((frogger.x + FROGGER_SIZE/2 > pos.x) &&
	   (frogger.x + FROGGER_SIZE/2 < pos.x + dim.x*OBSTICLE_SIZE) &&
	   (frogger.y + FROGGER_SIZE/2 > pos.y) &&
	   (frogger.y + FROGGER_SIZE/2 < pos.y + dim.y*OBSTICLE_SIZE))
	{
	  frogger.x = STARTING_X;
	  frogger.y = STARTING_Y;
	  frogger_lane_index = (600/GRID_SIZE) - 1;
	}
      }
      else if(lanes[frogger_lane_index].type == LANE_WATTER)
      {
	if((frogger.x + FROGGER_SIZE > pos.x) &&
	   (frogger.x < pos.x + dim.x*OBSTICLE_SIZE) &&
	   (frogger.y + FROGGER_SIZE > pos.y) &&
	   (frogger.y < pos.y + dim.y*OBSTICLE_SIZE))
	{
	  on_log = true;
	  break;
	}
      }
    }
  }

  
  // NOTE(vertexbyte): Its negative because we are going left
  // we can also use a bool because if its true the value will be the speed
  // and if its false it will be zero
  frogger_x_velocity = -lanes[frogger_lane_index].speed*on_log;
  
  if(!on_log && lanes[frogger_lane_index].type == LANE_WATTER)
  {
    frogger.x = STARTING_X;
    frogger.y = STARTING_Y;
    frogger_lane_index = (600/GRID_SIZE) - 1;
    frogger_x_velocity = 0.0f;
  }
}

b32 game_initialized = false;

extern "C"
GAME_UPDATE_AND_RENDER(game_update_and_render)
{
  if(!game_initialized)
  {
    srand(time(0));

    // NOTE(vertexbyte): What is good about this is the ability to change
    // the map obeticles(vehicles) dynamicly meaning that if the player
    // reaches higer scores we want to make it harder for him, and
    // we can already change the speed of the lanes and the vehicle
    // counts of the lanes
    add_obsticle_to_lane(14, 3);
    add_obsticle_to_lane(15, 2);
    add_obsticle_to_lane(16, 3);
    add_obsticle_to_lane(17, 2);

    add_obsticle_to_lane(12, 3);
    add_obsticle_to_lane(11, 2);

    add_obsticle_to_lane(9, 2);
    add_obsticle_to_lane(8, 4);
    add_obsticle_to_lane(7, 2);
    add_obsticle_to_lane(6, 2);

    add_obsticle_to_lane(4, 3);
    add_obsticle_to_lane(3, 4);
    add_obsticle_to_lane(2, 2);

    game_initialized = true;
  }
  
  clear_screen(draw_buffer, 0.0f, 0.0f, 0.0f);

  draw_lanes(draw_buffer);
  draw_and_update_obsticles(draw_buffer, delta_time);
  check_collisions();
  
  if(button_pressed(input, BUTTON_W))
  {
    frogger.y -= GRID_SIZE;
    --frogger_lane_index;
  }
  if(button_pressed(input, BUTTON_A))
  {
    frogger.x -= GRID_SIZE;
  }
  if(button_pressed(input, BUTTON_S))
  {
    frogger.y += GRID_SIZE;
    ++frogger_lane_index;
  }
  if(button_pressed(input, BUTTON_D))
  {
    frogger.x += GRID_SIZE;
  }

  frogger.x += frogger_x_velocity*delta_time;

  // TODO(vertexbyte): Write a overloaded draw_rectangle that takes in
  // vectors and colors
  draw_rectangle(draw_buffer, frogger.x, frogger.y, FROGGER_SIZE,
		 FROGGER_SIZE, frogger_color.r, frogger_color.g, frogger_color.b);
}
