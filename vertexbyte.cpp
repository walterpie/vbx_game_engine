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


struct Particle
{
  V2 position;
  V2 velocity;
  s32 life;
};

struct Particle_Family
{
  Particle *particles;
  Color color;
  s32 count; 
  s32 active_count;
};


#define GRAVITY make_v2(0.0f, 0.5f)
#define MAX_PARTICLE_SPEED 12
#define FIREWORK_FRICTION 0.95f
#define FIREWORK_RADIUS 2

Color color_table[] =
{
  make_color(1.0f, 0.0f, 0.0f),
  make_color(0.0f, 1.0f, 0.0f),
  make_color(0.0f, 0.0f, 1.0f),
  make_color(1.0f, 1.0f, 0.0f),
  make_color(0.0f, 1.0f, 1.0f),
  make_color(1.0f, 1.0f, 1.0f),
  make_color(0.5f, 1.0f, 0.5f),
  make_color(1.0f, 0.2f, 0.0f),
  make_color(0.2f, 1.0f, 1.0f)
};

void make_firework_particles(Particle_Family *family, s32 count,
			     V2 position)
{
  family->count = count;
  family->active_count = count;
  
  // NOTE(vertexbyte): Still dont have a proper random number generator.
  family->color = color_table[rand() % 9];

  // NOTE(vertexbyte): Our frame rate is 60 fps so a life of 60 is one
  // second 
  s32 min_life = 30;
  s32 max_life = 70;

  family->particles = (Particle *)malloc(sizeof(Particle)*count);

  for(s32 index = 0;
      index < count;
      ++index)
  {
    family->particles[index].position = position;
    family->particles[index].velocity = make_v2(0.0f, 0.0f);
    family->particles[index].life = min_life + (rand()%(max_life - min_life));
    
    r32 speed = rand() % MAX_PARTICLE_SPEED;
    set_length_v2(&family->particles[index].velocity, speed);

    r32 random_angle_deg = rand() % 360;
    // TODO(vertexbyte): Make radians_to_degrees and degrees_to_radians
    r32 angle = random_angle_deg*PI32/180;
    
    set_angle_v2(&family->particles[index].velocity, angle);
  }
}

void update_and_render_firework(Bitmap *buffer, Particle_Family *family)
{
  Particle *particles = family->particles;

  for(s32 index = family->count - family->active_count;
      index < family->count;
      ++index)
  {
    if(particles[index].life > 0)
    {
      --particles[index].life;
      fill_circle_slow(buffer, particles[index].position.x,
		       particles[index].position.y,
		       FIREWORK_RADIUS, family->color);
      
      particles[index].velocity = particles[index].velocity * FIREWORK_FRICTION;
      particles[index].velocity = particles[index].velocity + GRAVITY;
      particles[index].position = particles[index].position + particles[index].velocity;
    }
    else
    {
      // NOTE(vertexbyte): We dont need to draw or update dead particles
      --family->active_count;
    }
  }
}



struct Rocket
{
  V2 position;
  Color color;
  r32 speed;
  r32 explode_y;
  r32 tail_lenght;
  b32 exploded;
};

struct Game_State
{
  Particle_Family fireworks[5];
  Rocket rockets[5];
  s32 next_firework_index;
  s32 next_rocket_index;
  r32 rocket_fire_delay;
  r32 time_passed;
  b32 is_initialized;
};


void fire_rocket(Rocket *rocket, Bitmap *draw_buffer)
{
  r32 random_x = 20 + (rand() % (draw_buffer->width - 20));
  
  r32 random_explode_y = 30 + (rand() % draw_buffer->height/3);
  
  rocket->position = make_v2(random_x, draw_buffer->height);
  rocket->explode_y = random_explode_y;
  rocket->tail_lenght = 100 + (rand() % 50);
  rocket->speed = 5 + (rand() % 7);
  rocket->color = color_table[rand() % 9];
  rocket->exploded = false;
}

void update_and_render_rocket(Bitmap *buffer, Rocket *rocket,
			      Game_State *game_state)
{
  if(rocket->position.y >= rocket->explode_y)
  {
    rocket->position.y -= rocket->speed;
    
    fill_circle_slow(buffer, rocket->position.x, rocket->position.y,
		   20, rocket->color);

    r32 tail_segment = rocket->tail_lenght;
    if((rocket->position.y - rocket->explode_y) < rocket->tail_lenght)
    {
      tail_segment -= (rocket->tail_lenght - (rocket->position.y - rocket->explode_y));
    }
    draw_line(buffer, rocket->position.x, rocket->position.y,
	      rocket->position.x, rocket->position.y + tail_segment,
	      rocket->color);

  }
  else
  {
    if(!rocket->exploded)
    {
      make_firework_particles(&game_state->fireworks[game_state->next_firework_index],
			      100, rocket->position);
      
      game_state->next_firework_index = ++game_state->next_firework_index % 5;
    }
    
    rocket->exploded = true;
  }
}


extern "C"
GAME_UPDATE_AND_RENDER(game_update_and_render)
{
  Game_State *game_state = (Game_State *)memory->permanent_memory;
  if(!game_state->is_initialized)
  {
    srand(time(0));

    game_state->rocket_fire_delay = 0.5f;
    game_state->is_initialized = true;
  }
  
  clear_screen(draw_buffer, make_color(0.0f, 0.0f, 0.0f));
  Particle_Family *fireworks = game_state->fireworks;
  Rocket *rockets = game_state->rockets;
  
  for(s32 index = 0;
      index < 5;
      ++index)
  {
    update_and_render_firework(draw_buffer, &fireworks[index]);
    update_and_render_rocket(draw_buffer, &rockets[index], game_state);
  }

  game_state->time_passed += delta_time;
  if(game_state->time_passed >= game_state->rocket_fire_delay)
  {
    fire_rocket(&rockets[game_state->next_rocket_index], draw_buffer);
    game_state->next_rocket_index = ++game_state->next_rocket_index % 5;
    game_state->time_passed = 0.0f;
  }
}
