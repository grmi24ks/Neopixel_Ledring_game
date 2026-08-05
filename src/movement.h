#ifndef MOVEMENT_H
#define MOVEMENT_H

void update_circular_position(
    int *current_position,
    int pixel_count
);

void update_pendulum_position(
    int *current_position,
    int *direction,
    int pixel_count
);

#endif