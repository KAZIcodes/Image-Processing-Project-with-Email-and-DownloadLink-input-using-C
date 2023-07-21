#include "bmp.h"
// Filter functions:

// Convert image to grayscale
void grayscale(int height, int width, RGB image[height][width]);
// Reflect image horizontally
void reflect(int height, int width, RGB image[height][width]);
//reflect baraye png haye 4 channel
void reflectImage(unsigned char* image, int width, int height, int channels, RGB image_mat[height][width]);
// Blur image
void blur(int height, int width, RGB image[height][width], int intensity);
// Convert image to sepia
void sepia(int height, int width, RGB image[height][width]);
// Edge_artistic
void artistic_filter(int height, int width, RGB image[height][width]);
// Cartoon_like
void cartoon_like(int height, int width, RGB image[height][width]);
// Emboos
void emboss(int height, int width, RGB image[height][width]);
// Posterize
void posterize(int height, int width, RGB image[height][width], int levels);
//Sponge(parazit tor)
void sponge(int height, int width, RGB image[height][width], int intensity);
//Neon glow
void neon_glow(int height, int width, RGB image[height][width], int intensity, int threshold);
//washes two of RGB values
void color_wash(int height, int width, RGB image[height][width], char c);
//
void improvised(int height, int width, RGB image[height][width], char c);
//
void improvised2(int height, int width, RGB image[height][width]);
//
void improvised3(int height, int width, RGB image[height][width]);
//
void improvised4(int height, int width, RGB image[height][width], char *s);

