#ifndef RADAR_DISPLAY_H
#define RADAR_DISPLAY_H
#include <stdint.h>

void initialize_console_display(void);
void update_console_display(int angle, uint32_t distance);
void clear_console(void);

#endif //RADAR_DISPLAY_H
