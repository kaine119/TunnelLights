#ifndef KINETIC_RAIN_H
#define KINETIC_RAIN_H
#endif

/**
 * Color struct encoding 3 colour channels at 8-bit depth,
 * as supported by the WS2812 LED module.
 */
typedef struct {
  char h;
  char s;
  char v;
} Color;

/**
 * Struct holding the data to be received from the control machine.
 * Encodes the following:
 *     char positions[25]: bytes representing the desired height 
 *                          height values of each servo-attached
 *                          ball, from 0-255
 *     Color colors[25]: Color structs representing the desired
 *                          color of each LED ball.
 */
typedef struct {
  Color colors[25];
  char positions[25];
} RecvData;