#include "movement.h"

void update_circular_position(
    int *current_position,
    int pixel_count
)
{
    (*current_position)++;

    if (*current_position >= pixel_count)
    {
        *current_position = 0;
    }
}

void update_pendulum_position(
    int *current_position,
    int *direction,
    int pixel_count
)
{
    *current_position += *direction;

    if (*current_position >= pixel_count)
    {
        *current_position = pixel_count -2;
        *direction = -1;
    }
    else if (*current_position < 0)
    {
        *current_position = 1;
        *direction = 1;
    }
}