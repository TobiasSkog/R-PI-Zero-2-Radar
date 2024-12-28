#include "radar_display.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_SIZE 21
#define MAX_DISTANCE_CM 200

static char radar_grid[GRID_SIZE][GRID_SIZE];

void clear_console(void) {
  printf("\033[H\033[J");
  fflush(stdout);
}

void initialize_console_display(void) {
  memset(radar_grid, '.', sizeof(radar_grid));
}

//// ========================================================================== ////
///  Display: Angle, Distance, Pie-pieces every 30°, Green = Empty, Red = Found ////
///  180° range, x cm intervals                                                ////
//// ========================================================================== ////
void update_console_display(int angle, uint32_t distance) {
  memset(radar_grid, '.', sizeof(radar_grid)); // Clear the grid at each update

  int center = GRID_SIZE / 2;
  clear_console();
}


// for (int i = 0; i < GRID_SIZE; i++) {
//   for (int j = 0; j < GRID_SIZE; j++) {
//     radar_grid[i][j] = '.';
//   }
// }


//void update_console_display(int angle, uint32_t distance) {
//memset(radar_grid, '.', sizeof(radar_grid)); // Clear the grid at each update
//int center = GRID_SIZE / 2;
//clear_console();
//printf("\rangle:%d °, distance: %d cm", angle, distance);
// Draw the radar arm based on the current angle
// int arm_x = center + (int) (center * cos(angle * M_PI / 180.0));
// int arm_y = center + (int) (center * sin(angle * M_PI / 180.0));
// if (arm_x >= 0 && arm_x < GRID_SIZE && arm_y >= 0 && arm_y < GRID_SIZE) {
//   radar_grid[arm_y][arm_x] = '|';
// }

// Draw the object based on the current distance and angle
// if (distance > 0 && distance <= MAX_DISTANCE_CM) {
//   int obj_x = center + (int) (distance * cos(angle * M_PI / 180.0)); // Use 'distance' for object placement
//   int obj_y = center + (int) (distance * sin(angle * M_PI / 180.0));
//   if (obj_x >= 0 && obj_x < GRID_SIZE && obj_y >= 0 && obj_y < GRID_SIZE) {
//     radar_grid[obj_y][obj_x] = 'X'; // Place 'X' at the correct position
//   }
//   for (int theta = 0; theta < 360; theta++) {
//     int x = center + (int) (r * cos(theta * M_PI / 180.0));
//     int y = center + (int) (r * sin(theta * M_PI / 180.0));
//     if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
//       //radar_grid[y][x] = '*'; // Mark the grid with 'o' for scanning
//       //printf("[%d, %d]\n", x, y);
//       //printf("\033[H\033[J");
//       //fflush(stdout);
//     }
//   }
// }
//
// // Print the updated radar grid
// for (int i = 0; i < GRID_SIZE; i++) {
//   for (int j = 0; j < GRID_SIZE; j++) {
//     //printf("%c", radar_grid[i][j]);
//   }
//   //printf("\n");
// }
// for (int i = 0; i < GRID_SIZE; i++) {
//   //printf("\r");
// }
// }

// for (int r = 1; r < center; r++) {
//}

//clear_console();
//printf("\rangle:%d °, distance: %d cm", angle, distance);
//printf("angle:%d °, distance: %d cm\n", angle, distance);
