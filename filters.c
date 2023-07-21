#include <stdio.h>
#include "bmp.h"
#include <math.h>
#include <stdlib.h>

int str_cmp(char *str1, char *str2){
    for (int i = 0; str1[i] != '\0' || str2[i] != '\0'; ++i) {
        if (str1[i] > str2[i]) {return 1;}
        if (str1[i] < str2[i]) {return -1;}
    }
    return 0;
}

// Convert image to grayscale
void grayscale(int height, int width, RGB image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int x = round ((image[i][j].Red + image[i][j].Green + image[i][j].Blue) / 3.0);
            image[i][j].Red = x;
            image[i][j].Green = x;
            image[i][j].Blue = x;
        }
    }
    return;
}



// Blur image
void blur(int height, int width, RGB image[height][width], int intensity)
{
    RGB(*temp)[width] = calloc(height, width * sizeof(RGB));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float sumBlue = 0;
            float sumGreen = 0;
            float sumRed = 0;
            float counter = 0;

            for (int r = (-1)*intensity ; r < intensity+1 ; r++)
            {
                for (int c = (-1)*intensity ; c < intensity+1 ; c++)
                {
                    if (i + r < 0 || i + r > height - 1)
                    {
                        continue;
                    }

                    if (j + c < 0 || j + c > width - 1)
                    {
                        continue;
                    }

                    sumBlue += image[i + r][j + c].Blue;
                    sumGreen += image[i + r][j + c].Green;
                    sumRed += image[i + r][j + c].Red;
                    counter++;
                }
            }

            temp[i][j].Blue = round(sumBlue / counter);
            temp[i][j].Green = round(sumGreen / counter);
            temp[i][j].Red = round(sumRed / counter);
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].Blue = temp[i][j].Blue;
            image[i][j].Green = temp[i][j].Green;
            image[i][j].Red = temp[i][j].Red;
        }
    }
    free(temp);
    return;
}


// Convert image to sepia
int check (int x)
{
    if (x > 255 || x < 0)
    {
        x = 255;
        return x;
    }
    return x;
}

void sepia(int height, int width, RGB image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int R = round (0.393 * image[i][j].Red + 0.769 * image[i][j].Green + 0.189 * image[i][j].Blue);
            int G = round (0.349 * image[i][j].Red + 0.686 * image[i][j].Green + 0.168 * image[i][j].Blue);
            int B = round (0.272 * image[i][j].Red + 0.534 * image[i][j].Green + 0.131 * image[i][j].Green);

            image[i][j].Red = check (R);
            image[i][j].Green = check (G);
            image[i][j].Blue = check (B);
        }
    }

    return;
}



//Edge
long long int min(long long int a, long long int b) {
    return (a < b) ? a : b;
}

void artistic_filter(int height, int width, RGB image[height][width]) {
    RGB(*temp)[width] = calloc(height, width * sizeof(RGB));
    int filter_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int filter_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // Copy original image data to temporary array
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp[i][j] = image[i][j];
        }
    }

    // Apply Sobel operator to RGB values
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            int x_grad = 0, y_grad = 0;

            for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                    x_grad += temp[i + k][j + l].Red * filter_x[k + 1][l + 1];
                    y_grad += temp[i + k][j + l].Red * filter_y[k + 1][l + 1];
                }
            }

            int grad = abs(x_grad) + abs(y_grad);

            // Apply edge-enhancing filter to RGB values
            image[i][j].Red = min(255, grad + temp[i][j].Red);
            image[i][j].Green = min(255, grad + temp[i][j].Green);
            image[i][j].Blue = min(255, grad + temp[i][j].Blue);
        }
    }
    free(temp);
    return;
}

//Dark grain filter actually
void cartoon_like(int height, int width, RGB image[height][width]) {
    RGB(*temp)[width] = calloc(height, width * sizeof(RGB));
    int filter[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};

    // Copy original image data to temporary array
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp[i][j] = image[i][j];
        }
    }

    // Apply Laplacian filter to RGB values
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            int sum = 0;

            for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                    sum += temp[i + k][j + l].Red * filter[k + 1][l + 1];
                }
            }

            // Apply cartoon-like effect to RGB values
            if (sum > 255) {
                image[i][j].Red = 255;
            } else if (sum < 0) {
                image[i][j].Red = 0;
            } else {
                image[i][j].Red = sum;
            }

            if (sum > 255) {
                image[i][j].Green = 255;
            } else if (sum < 0) {
                image[i][j].Green = 0;
            } else {
                image[i][j].Green = sum;
            }

            if (sum > 255) {
                image[i][j].Blue = 255;
            } else if (sum < 0) {
                image[i][j].Blue = 0;
            } else {
                image[i][j].Blue = sum;
            }
        }
    }
    free(temp);
    return;
}

long long int max(long long int a,long long int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}
//Emboss filter
void emboss(int height, int width, RGB image[height][width]) {
    RGB(*temp)[width] = calloc(height, width * sizeof(RGB));
    int filter[3][3] = {{-2, -1, 0}, {-1, 1, 1}, {0, 1, 2}};

    // Copy original image data to temporary array
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp[i][j] = image[i][j];
        }
    }

    // Apply emboss filter to RGB values
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            int sumR = 0;
            int sumG = 0;
            int sumB = 0;

            for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                    sumR += temp[i + k][j + l].Red * filter[k + 1][l + 1];
                    sumG += temp[i + k][j + l].Green * filter[k + 1][l + 1];
                    sumB += temp[i + k][j + l].Blue * filter[k + 1][l + 1];
                }
            }

            // Clamp RGB values to range of 0 to 255
            image[i][j].Red = min(255, max(0, sumR + 128));
            image[i][j].Green = min(255, max(0, sumG + 128));
            image[i][j].Blue = min(255, max(0, sumB + 128));
        }
    }
    free(temp);
    return;
}


void posterize(int height, int width, RGB image[height][width], int levels) {
    // Calculate the color range for each color channel
    int range = 255 / (levels - 1);

    // Apply posterize filter to RGB values
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Calculate the posterized value for each color channel
            int posterizedR = (image[i][j].Red / range) * range;
            int posterizedG = (image[i][j].Green / range) * range;
            int posterizedB = (image[i][j].Blue / range) * range;

            // Set the posterized RGB values
            image[i][j].Red = posterizedR;
            image[i][j].Green = posterizedG;
            image[i][j].Blue = posterizedB;
        }
    }

    return;
}


void sponge(int height, int width, RGB image[height][width], int intensity)
{
    int i, j, r, g, b;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            // Generate a random number between -intensity and +intensity
            r = rand() % (2 * intensity + 1) - intensity;
            g = rand() % (2 * intensity + 1) - intensity;
            b = rand() % (2 * intensity + 1) - intensity;

            // Add the random values to the pixel's RGB values
            image[i][j].Red = max(0, min(255, image[i][j].Red + r));
            image[i][j].Green = max(0, min(255, image[i][j].Green + g));
            image[i][j].Blue = max(0, min(255, image[i][j].Blue + b));
        }
    }
}


void neon_glow(int height, int width, RGB image[height][width], int intensity, int threshold)
{
    int i, j, k, r, g, b;
    RGB(*temp)[width] = calloc(height, width * sizeof(RGB));

    // Copy the original image to a temporary image
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            temp[i][j] = image[i][j];
        }
    }

    // Apply a threshold to the original image
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (image[i][j].Red + image[i][j].Green + image[i][j].Blue < threshold * 3) {
                image[i][j].Red = 0;
                image[i][j].Green = 0;
                image[i][j].Blue = 0;
            }
        }
    }

    // Apply a Gaussian blur to the thresholded image
    blur(height, width, image, 14);

    // Add the blurred thresholded image to the original image, scaled by the intensity parameter
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            r = temp[i][j].Red + intensity * image[i][j].Red / 255;
            g = temp[i][j].Green + intensity * image[i][j].Green / 255;
            b = temp[i][j].Blue + intensity * image[i][j].Blue / 255;
            image[i][j].Red = max(0, min(255, r));
            image[i][j].Green = max(0, min(255, g));
            image[i][j].Blue = max(0, min(255, b));
        }
    }
}
/*The neon_glow function first applies a threshold to the image,
 * setting any pixels with a brightness below the threshold to black. Then, it applies a Gaussian blur to the
 * thresholded image to create a soft, glowing effect. Finally, it combines the blurred thresholded image with the
 * original image, scaled by the intensity parameter.

The threshold parameter controls the brightness threshold that is used to determine which pixels to include in
 the glow effect. The intensity parameter controls the strength of the glow effect.*/



void color_wash(int height, int width, RGB image[height][width], char c){
    if (c == 'R' || c == 'r'){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Green = 0;
                image[i][j].Blue = 0;
            }
        }
    }
    else if (c == 'B' || c == 'b'){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Green = 0;
                image[i][j].Red = 0;
            }
        }
    }
    else if (c == 'G' || c == 'g'){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Blue = 0;
                image[i][j].Red = 0;
            }
        }
    }
}


//yeki az value haye RGB ro mizarm jaye yeki dige az RGB bar asas entekhab karbar
void improvised(int height, int width, RGB image[height][width], char c){
    if (c == 'A' || c == 'a'){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Green = image[i][j].Red;
                //image[i][j].Blue = image[i][j].Red;
            }
        }
    }
    else if (c == 'B' || c == 'b'){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                //image[i][j].Green = image[i][j].Red;
                image[i][j].Blue = image[i][j].Red;
            }
        }
    }
    else if (c == 'C' || c == 'c'){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Green = image[i][j].Blue;
                //image[i][j].Red = image[i][j].Blue;
            }
        }
    }
    else if (c == 'D' || c == 'd'){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                //image[i][j].Green = image[i][j].Blue;
                image[i][j].Red = image[i][j].Blue;
            }
        }
    }
    else if (c == 'E' || c == 'e'){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Blue = image[i][j].Green;
                //image[i][j].Red = image[i][j].Green;
            }
        }
    }
    else if (c == 'F' || c == 'f'){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                //image[i][j].Blue = image[i][j].Green;
                image[i][j].Red = image[i][j].Green;
            }
        }
    }
}

//maximum beine bishtarin value az RGB ro sefr mizare
void improvised2(int height, int width, RGB image[height][width]){
    long long int R_count = 0, G_count = 0, B_count = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            R_count += image[i][j].Red;
            G_count += image[i][j].Green;
            B_count += image[i][j].Blue;
        }
    }
    long long int maximum = max(R_count, max(G_count, B_count));

    if (maximum == R_count){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Red = 0;
            }
        }
    }
    else if (maximum == G_count){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Green = 0;
            }
        }
    }
    else if (maximum == B_count){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Blue = 0;
            }
        }
    }
}

//minimumm beine bishtarin value az RGB ro sefr mizare
void improvised3(int height, int width, RGB image[height][width]){
    long long int R_count = 0, G_count = 0, B_count = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            R_count += image[i][j].Red;
            G_count += image[i][j].Green;
            B_count += image[i][j].Blue;
        }
    }
    long long int minimum = min(R_count, min(G_count, B_count));

    if (minimum == R_count){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Red = 0;
            }
        }
    }
    else if (minimum == G_count){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Green = 0;
            }
        }
    }
    else if (minimum == B_count){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                image[i][j].Blue = 0;
            }
        }
    }
}



//value R ya G ya B ke bishtare ya kamtar nesbat be baghie bar asas entekhab karbar ke ba max ya min kar kone replace mikone ba myangin do value dige
void improvised4(int height, int width, RGB image[height][width], char *s){
    long long int R_count = 0, G_count = 0, B_count = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            R_count += image[i][j].Red;
            G_count += image[i][j].Green;
            B_count += image[i][j].Blue;
        }
    }
    long long int maximum = max(R_count, max(G_count, B_count));
    long long int minimum = min(R_count, min(G_count, B_count));

    if (str_cmp(s, "Max") == 0 || str_cmp(s, "max") == 0){
        if (maximum == R_count){
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    image[i][j].Red = (image[i][j].Blue + image[i][j].Green) / 2;
                }
            }
        }
        else if (maximum == G_count){
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    image[i][j].Green = (image[i][j].Blue + image[i][j].Red) / 2;
                }
            }
        }
        else if (maximum == B_count){
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    image[i][j].Blue = (image[i][j].Red + image[i][j].Green) / 2;
                }
            }
        }
    }
    else if (str_cmp(s, "Min") == 0 || str_cmp(s, "min") == 0){
        if (minimum == R_count){
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    image[i][j].Red = (image[i][j].Blue + image[i][j].Green) / 2;
                }
            }
        }
        else if (minimum == G_count){
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    image[i][j].Green = (image[i][j].Blue + image[i][j].Red) / 2;
                }
            }
        }
        else if (minimum == B_count){
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    image[i][j].Blue = (image[i][j].Red + image[i][j].Green) / 2;
                }
            }
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGB image[height][width])
{
    RGB(*temp)[width] = calloc(height, width * sizeof(RGB));
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            temp[i][j] = image[i][j];
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int z = width -1, j = 0; z >= 0; z--, j++)
        {
            image[i][j] = temp[i][z];
        }
    }


    free(temp);
    return;
}

//reflect baraye png haye 4 channel
void reflectImage(unsigned char* image, int width, int height, int channels, RGB image_mat[height][width]) {
    //handels alpha values reflection
    const int stride = width * channels;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < stride / 2; x += channels) {
            int leftPixelIndex = y * stride + x;
            int rightPixelIndex = (y + 1) * stride - (x + channels);


            // Preserve the alpha channel
            unsigned char tempAlpha = image[leftPixelIndex + channels - 1];
            image[leftPixelIndex + channels - 1] = image[rightPixelIndex + channels - 1];
            image[rightPixelIndex + channels - 1] = tempAlpha;
        }
    }

    reflect(height, width, image_mat); //handels RGB reflection
}
