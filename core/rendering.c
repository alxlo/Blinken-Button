/*
 * animation.c
 *
 *  http://interactive-matter.org/
 *
 *  This file is part of Blinken Button.
 *
 *  Blinken Button is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Blinken Button is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with Blinken Button.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  Created on: 26.01.2010
 *
 *  The animation routines control the
 *  TODO does it control the text mode too?
 */
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include "rendering.h"
#include "../animations.h"
#include "../sprites.h"
#include "../font.h"
#include "state.h"
#include "random.h"
#include "display.h"

/*
 * The defines the speed text scrolls through the display
 * Increase for slower scrolling, decrease for faster scrolling
 * TOSO this is a wait time - is there a way to rework this?
 */
#define TEXT_SCROLL_SPEED 2
/*
 * This defines the internal size of the animation buffer.
 * It contains several images of an animation in the main ram.
 * TODO and why? can we get rid of that?
 */
#define BUFFER_SIZE 8

/*
 * Our main display buffer to load sprites into. To make size comparisons easier
 * we allocate it one array bigger. Else we would have to use clumpsy
 * BUFFER_SIZE+1 calculation all through the code.
 * TODO get rid of this!
 */
uint8_t animations_buffer[BUFFER_SIZE + 1][8];

/*
 * State for displaying text & animations.
 * to coordinate the correct rendering states we must know if we are
 * - displaying animations
 * - displaying texts
 * - do we need to render some text
 * - is the text finished and we fill the remaining space
 * - do we need to load a new sequence from flash
 * - do we need to load a new sprite from flash
 */
//are we displaying text?
volatile uint8_t state_animation_displaying_text;
//are we displaying animations?
volatile uint8_t state_animation_displaying_animation;
//is some rendering needed?
volatile uint8_t state_animation_text_render_state;
//are we displaying text outro?
volatile uint8_t state_animation_display_text_outro;
//is a new sequence needed?
volatile uint8_t state_animation_next_sequence;
//do we need to load the next sprite?
volatile uint8_t state_animation_next_sprite;
//this state is used to play an simple test pattern at the beginning
volatile uint8_t state_animation_test_pattern;

/*
 * wait time to switch to the next sprite
 * */
volatile uint8_t animation_sprite_wait = 0;
/*
 * The display speed of the current sequence
 * This is set to 255. Each animation has it own animation speed later
 */
volatile uint8_t animation_sprite_speed = 255;
/*
 * How fast do we want to change to the next sequence.
 * This controls how fast we switch between the different animations and text.
 */
volatile uint8_t switch_sequence_interval;
/*
 * This variable holds the value how long we have waited to switch the
 * animation sequence. If that variable hits the sequence interval we switch
 * to the next sequence.
 */
volatile uint8_t switch_sequence_wait;
//begin index of the current sequence
volatile uint8_t animation_sequence_start;
//end index of the current sequence
volatile uint8_t animation_sequence_end;
//the index of the next sprite to be displayed
volatile uint8_t animation_sequence_next_sprite;
//buffer for the above values
//needed if we want to display text in between
uint8_t animation_buffer_sequence_start;
uint8_t animation_buffer_sequence_end;
uint8_t animation_buffer_sequence_speed;

/*
 * variables for displaying messages.
 * Basically
 *  msg_buffer holds the pointer to the actual message
 *  message_length is the length of the currently displayed message
 *  message_pointer points to the currently displayed character of the message
 *  message_char_pointer is the current position in the character, currently
 *   displayed (i.e. going from 0 to 3 for a 4 column character)
 *  message_char_length is the length of the currently displayed char
 *  message_active_char holds the numerical value of the current character to
 *   read it from flash memory
 */
char* msg_buffer;
uint8_t message_length;
uint8_t message_pointer;
uint8_t message_char_pointer;
uint8_t message_char_length;
uint8_t message_active_char;

/*
 * This are prototypes for functions we use in this file but we do not want to
 * make them accessible for others - since they are internal
 */
//start the timer for switching between animations
void
animation_start_animation_timer(void);
//start the timer for updating sequences or text
void
animation_start_update_timer(void);
//internal routing to set sequence
//with this routine you can overwrite the built in sprites
void
animation_set_sequence(int8_t start, int8_t end, uint8_t speed);
//clear the animation buffer
void
animation_clear_buffer(uint8_t buffer_number);

void
animation_show_char(void);
//finish displaying a message and go back to animation
void
animation_end_display_message(void);
//do everything to render the actual text
void
animation_text_render(void);
//load the next animation sequence fro flash
void
animation_load_next_sequence(void);
//load the next sprite from flash memory
void
animation_load_next_sprite(void);

/*
 * This functions set up everything for the animation routine. It needs to be
 * calles before the 'big' loop routine kicks in.
 */
void
animation_init(void)
{
  //get a new random seed
  randomize_seed();

  //register the states
  state_animation_text_render_state
      = state_register_task(animation_text_render);
  state_animation_next_sprite = state_register_task(animation_load_next_sprite);
  state_animation_next_sequence = state_register_task(
      animation_load_next_sequence);
  state_animation_displaying_text = state_register_state();
  state_animation_displaying_animation = state_register_state();
  state_animation_display_text_outro = state_register_state();
  state_animation_test_pattern = state_register_state();

  //before we do anything we switch on the test pattern
  state_activate(state_animation_test_pattern);

  //initialize the display
  display_init();
  //and now start the display
  //start the update timer for switching animations
  animation_start_update_timer();
  //start the timer to change sequences
  animation_start_animation_timer();
}

//routine to advance one sequence
void
animation_load_next_sequence(void)
{

  //select the next sequence randomly
  uint8_t animation_sequence_number = get_random(MAX_SEQUENCE);
  //buffer for loading a sequence (all information for the sequence)
  //like length and so on
  _sequence_struct curr_sequence;
  //copy the sequence from flash to the buffer in the previous line
  memcpy_P(&curr_sequence, &sequences[animation_sequence_number],
      sizeof(_sequence_struct));
  //how many sprites we got in the sequence
  uint8_t sequence_length = pgm_read_byte(curr_sequence.sprites);
  //load the sprites
  uint8_t i;
  for (i = 0; i < sequence_length; i++)
    {
      uint8_t index = pgm_read_byte(curr_sequence.sprites+i + 1);
      copy_to_buffer(predefined_sprites[index], animations_buffer[i]);
    }
  uint8_t speed = curr_sequence.display_speed;
  //now set the sequence a s currently displayed sequence
  animation_set_sequence(0, sequence_length - 1, speed);
  //set the sequence display length
  switch_sequence_interval = curr_sequence.display_length;
}
/*
 * This routine loads the next sprite from flash to load it into the display
 */
void
animation_load_next_sprite(void)
{
  //we load the next sprite to the display
  display_load_sprite(animations_buffer[animation_sequence_next_sprite]);
  //and switch to it
  display_advance_buffer();
}

/*
 * This routine sets the current sequence.
 *  start is in respect ot the internal buffer
 *  end dito
 *  speed is the wait time for the sprites - and by that controlling the speed
 *   of the animation
 */
void
animation_set_sequence(int8_t start, int8_t end, uint8_t speed)
{
  if ((start < BUFFER_SIZE) && (end < BUFFER_SIZE) && (start <= end))
    {
      animation_sequence_start = start;
      animation_sequence_end = end;
      animation_sequence_next_sprite = start;
      animation_sprite_speed = speed;
      state_activate(state_animation_displaying_animation);
      state_deactivate(state_animation_displaying_text);
    }
  else
    {
      //TODO: error handling
    }
}

/*
 * select a message from flash memory to load it into the internal message
 * buffer
 */
void
animation_load_message(void)
{
  uint8_t animation_message_number = get_random(MAX_MESSAGE);
  strcpy_P(message,
      (uint8_t*) pgm_read_word(&(messages[animation_message_number])));
}

/*
 * Display a certain message.
 * This is done by switching of the animation, switching to 'text mode'.
 * Saving the current animation to later come back to it after we have
 * finished displaying the text
 */
void
animation_display_message(char* message)
{
  //set status
  state_activate(state_animation_displaying_text);
  state_deactivate(state_animation_displaying_animation);

  //save the previous animation
  animation_buffer_sequence_start = animation_sequence_start;
  animation_buffer_sequence_end = animation_sequence_end;
  animation_buffer_sequence_speed = animation_sprite_speed;

  //TODO copy message to some internal buffer?
  msg_buffer = message;
  message_length = strlen(msg_buffer);
  message_pointer = 0;
  message_char_pointer = 0;
  message_char_length = 0;
  animation_sprite_speed = TEXT_SCROLL_SPEED;

  //to ensure a clean buffer we clear the text buffer
  animation_clear_buffer(BUFFER_SIZE);

  state_activate(state_animation_text_render_state);
}

/*
 * This routine finishes the displaying of message.
 * It basically restores the previous animation and sets the state variables
 * to display the animation again.
 */
void
animation_end_display_message(void)
{
  //restore the previous animation
  //the status is updated by set_Sequence
  animation_set_sequence(animation_buffer_sequence_start,
      animation_buffer_sequence_end, animation_buffer_sequence_speed);
  //the built in buffer was used for rendering the text
  //load_default_sequence();
  //set status
  state_deactivate(state_animation_displaying_text);
  state_activate(state_animation_displaying_animation);
}

/*
 * Displays the actual message.
 * Scrolls the screen to the left and draws new pixels for the current character
 * on the right.
 */
void
animation_show_char(void)
{
  uint8_t i;

  // shift the screen to the left
  for (i = 0; i < 8; i++)
    {
      animations_buffer[BUFFER_SIZE][i] >>= 1;
    }
  //if we reached the end of the previous char
  // advance a char if needed
  if (message_char_pointer == message_char_length)
    {
      //a unusual char length of 0
      //signals the first run
      if (message_char_length != 0)
        {
          message_pointer++;
        }
      if (message_pointer == message_length)
        {
          state_activate(state_animation_display_text_outro);
        }
      // which character is displayed
      message_active_char = msg_buffer[message_pointer] - CHAR_OFFSET;
      //how much columns got the current char
      message_char_length = pgm_read_byte(&font[message_active_char * 4 + 3]);
      //start at the beginning of the char
      message_char_pointer = 0;
    }
  else
    {
      //advance to the next char column
      if (!state_is_active(state_animation_display_text_outro))
        {
          uint8_t char_byte;
          // read pixels for current column of char
          char_byte
              = pgm_read_byte(font+(message_active_char * 4 + message_char_pointer));
          message_char_pointer++;
          // write pixels into screen memory
          for (i = 0; i < 6; i++)
            {
              if (char_byte & _BV(i))
                {
                  animations_buffer[BUFFER_SIZE][i + 2] |= _BV(7);
                }
            }
        }
      //if we are displaying the outro just add empty columns
      else
        {
          //if we have not reached 8 empty columns
          if (message_char_pointer < 8)
            {
              message_char_pointer++;
            }
          else
          //ok, we are really finished
            {
              state_deactivate(state_animation_display_text_outro);
              animation_end_display_message();
            }
        }
    }
  //load the rendered text part into the main display buffer
  display_load_sprite(animations_buffer[BUFFER_SIZE]);
  //we start at the first sprite
  animation_sequence_next_sprite = BUFFER_SIZE;
  //and now display this sprite
  animation_load_next_sprite();
}

/*
 * The animation_text_render manages all the high level animation stuff like
 * managing the text rendering or if no text is rendered to decide according to
 *
 */
void
animation_text_render(void)
{
  //if we are displaying text advance on char
  if (state_is_active(state_animation_displaying_text))
    {
      //we use alternating the first two sprites of the buffer
      animation_show_char();
    }
  else //if we are not displaying a text message
    {
      //according to a random value we decide if we want to display some text
      if (get_random(MESSAGE_PROBABILITY) == 1)
        {
          animation_load_message();
          animation_display_message(message);
        }
    }
}

/*
 * A small routine to write 8 zeroes to the buffer.
 */
void
animation_clear_buffer(uint8_t buffer_number)
{
  if (buffer_number < BUFFER_SIZE)
    {
      animations_buffer[buffer_number][0] = 0;
      animations_buffer[buffer_number][1] = 0;
      animations_buffer[buffer_number][2] = 0;
      animations_buffer[buffer_number][3] = 0;
      animations_buffer[buffer_number][4] = 0;
      animations_buffer[buffer_number][5] = 0;
      animations_buffer[buffer_number][6] = 0;
      animations_buffer[buffer_number][7] = 0;
    }
  else
    {
      //TODO error handling
    }
}

void
animation_start_update_timer(void)
{
  power_timer1_enable();
  TCCR1A = 0;
  TCCR1B = _BV(CS12);
  TCCR1C = 0;
  TIMSK1 = _BV(TOIE1);
}

volatile uint8_t test_row = 0;
volatile uint8_t test_column=0;

/*
 * This routine starts the update timer (Timer 1). it is used to switch between
 * animations. The timer runs at 0,4Hz (it is a 16 bit counter) to switch
 * smoothly between the animations.
 * TODO can't we unit those timers - they confuse me
 */
ISR (TIMER1_OVF_vect)
{
  //if the test state is active we first render the test pattern
  if (state_is_active(state_animation_test_pattern)) {
      return;
  }
  //if we are displaying animations
  if (state_is_active(state_animation_displaying_animation))
    {
      //wait until a update is needed
      switch_sequence_wait++;
      if (switch_sequence_wait > switch_sequence_interval)
        {
          //wait again
          switch_sequence_wait = 0;
          //but indicate that we want to load a new sequence
          state_activate(state_animation_next_sequence);
        }
    }
  //we render the text anyway
  //this function also decides if there should be some text displayed
  state_activate(state_animation_text_render_state);

}

/*
 * This is the 'animation timer' it switches between the different images of
 * the animation to produce the uhm - animation. It runs at 30Hz
 */
void
animation_start_animation_timer(void)
{
  power_timer2_enable();
  TCCR2A = 0;
  TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);
  TIMSK2 = _BV(TOIE2);
  ASSR = 0;
}

/*
 * This is the actual timer routine which controls switching from one sprite
 * to the other in a regular manner.
 */
ISR(TIMER2_OVF_vect)
{
  if (state_is_active(state_animation_test_pattern)) {
      //we simply let a dot go from top left to bottom right
      //so we first clear the buffer
      animation_clear_buffer(0);
      //then we enable the dot in the current column & row
      animations_buffer[0][test_row] = _BV(test_column);
      //load it immediately to the display
      display_load_sprite(animations_buffer[0]);
      //and switch the buffer
      display_advance_buffer();
      //on to thenext column
      test_column++;
      //if we reached the last column
      if (test_column>=8) {
          //we switch to the next row
          test_row++;
          //and start at the first column again
          test_column=0;
          //and if we even reached the last row - we can end the test pattern
          if (test_row>=8) {
              //We leave the buffer in a proper state
              animation_clear_buffer(0);
              //and deactivate our test state an go over to 'normal' operation
              state_deactivate(state_animation_test_pattern);
              //prepare to load the first sequence
              animation_load_next_sequence();
              //load the first sprite immediately
              animation_load_next_sprite();
          }
      }
      return;
  }
  //we should not wait any longer
  if (animation_sprite_wait == 0)
    {
      if (state_is_active(state_animation_displaying_animation))
        {
          //if we are not in the current sequence switch to start
          if (animation_sequence_next_sprite < animation_sequence_start)
            {
              animation_sequence_next_sprite = animation_sequence_start;
            }
          else
            {
              //advance one sprite
              animation_sequence_next_sprite++;
              //if we reached the end of the sequence
              if (animation_sequence_next_sprite > animation_sequence_end)
                {
                  //start again from the beginning
                  animation_sequence_next_sprite = animation_sequence_start;
                }
            }
          state_activate(state_animation_next_sprite);
        }
      //if we are displaying text initiate a new render cycle
      else if (state_is_active(state_animation_displaying_text))
        {
          state_activate(state_animation_text_render_state);
        }
      //set the wait variable to the current wait time
      animation_sprite_wait = animation_sprite_speed;
    }
  else
  //wait a bit less
    {
      animation_sprite_wait--;
    }
}
