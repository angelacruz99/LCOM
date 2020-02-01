#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t status;
  uint8_t lsb,msb;
  uint8_t control_word_4lsbits;
  uint8_t control_word;
  uint16_t divisor = (uint16_t)(TIMER_FREQ / freq);
  uint32_t control_word_32b;

  //get status and mantain counting base and operating mode
  if (timer_get_conf(timer, &status) == 0) {
    control_word_4lsbits = status & 0X0F;

    //build new Control Word
    if (timer == 0) {
      control_word = (TIMER_SEL0 | TIMER_LSB_MSB | control_word_4lsbits);
      return 0;
    }
    else if (timer == 1) {
      control_word = (TIMER_SEL0 | TIMER_LSB_MSB | control_word_4lsbits);
      return 0;
    }
    else if (timer == 2) {
      control_word = (TIMER_SEL0 | TIMER_LSB_MSB | control_word_4lsbits);
      return 0;
    }
    else
      return 1;

    control_word_32b = (uint32_t) control_word;

    uint8_t counter;

    if (sys_outb(TIMER_CTRL, control_word_32b) == OK) {
      if (timer == 0) {
        counter = (uint8_t *) TIMER_0;
      }
      else if (timer == 1) {
        counter = (uint8_t *) TIMER_1;
      }
      else
        counter = (uint8_t *) TIMER_2;

      //write lsb to counter of the timer
      util_get_LSB(divisor, &lsb);

      //write msb to counter of the timer
      util_get_MSB(divisor, &msb);
    }
  }
  return 1;

}

int(timer_subscribe_int)(uint8_t *bit_no) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void(timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {

  //setting the RB Command in a uint8_t: D7=1, D6=1 - RB command; D5=1 - don't latch count; D4=0 - latch status;
  uint8_t rb_command = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer)); //REMEMBER: for count=0 we have count_=1 and for status= 1 we have status_=0

  //converting uint8_t RB command to a uint32_t
  uint32_t rb_command_32b = (uint32_t) rb_command;

  //checking if the output the rb command regenerates when inputed to the controller is valid - using a uint32_t

  if (sys_outb(TIMER_CTRL, rb_command_32b) == OK) {
    //Do the input and return the staus byte in the variable rb_command
    if (timer == 0) {
      if (util_sys_inb(TIMER_0, &rb_command) == 0) {
        *st = rb_command;
        return 0;
      }
      return 1;
    }
    else if (timer == 1) {
      if (util_sys_inb(TIMER_1, &rb_command) == 0) {
        *st = rb_command;
        return 0;
      }
      return 1;
    }
    else if (timer == 2) {
      if (util_sys_inb(TIMER_2, &rb_command) == 0) {
        *st = rb_command;
        return 0;
      }
      return 1;
    }
    return 1;
  }
  return 1;
}

int(timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  //uint8_t field_value;
  uint8_t temp;

  //for each type of field, check it's value for the timer "timer" and output it's meaning

  union timer_status_field_val field_value;

  switch (field) {

    case tsf_base:
      field_value.bcd = TIMER_SB_BCD & st;
      /*field_value = TIMER_SB_BCD & st;
      printf("Counting Base: ");
      if(field_value == 0)
        printf("Binary (16 bits)\n\n");
      else
        printf("BCD (4 decades)\n\n");*/
      break;

    case tsf_mode:
      //printf("Operating Mode: ");
      temp = ((TIMER_SB_MODE & st) >> 1);
      if (temp == 0 || temp == 1 || temp == 4 || temp == 5)
        field_value.count_mode = temp;
      else if (temp == 2 || temp == 6)
        field_value.count_mode = 2;
      else
        field_value.count_mode = 3;
      break;

    case tsf_initial:
      temp = ((TIMER_SB_INIT & st) >> 4);
      //printf("Initialization Mode: ");
      if (temp == 1)
        field_value.in_mode = LSB_only;
      //printf("LSB\n\\n");
      else if (temp == 2)
        field_value.in_mode = MSB_only;
      //printf("MSB\n\n");
      else if (temp == 3)
        field_value.in_mode = MSB_after_LSB;
      //printf("LSB followed by MSB\n\n");*/
      else
        field_value.in_mode = INVAL_val;
      break;

    case tsf_all:
      field_value.byte = st;
      break;

    default:
      printf("Invalid Field!\n\n");
      return 1;
  }

  return timer_print_config(timer, field, field_value);
}
