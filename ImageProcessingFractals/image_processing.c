#include "image_processing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Image* readPPM(const char* filename) {
    FILE* file;
    fopen_s(&file, filename, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    char magic[3];
    int width, height, maxVal;

    if (fscanf_s(file, "%2s %d %d %d", magic, sizeof(magic), &width, &height, &maxVal) != 4 || strcmp(magic, "P6") != 0) {
        fprintf(stderr, "Invalid PPM file format\n");
        fclose(file);
        return NULL;
    }

    if (maxVal != 255) {
        fprintf(stderr, "Unsupported max value in PPM file (only 8-bit images are supported)\n");
        fclose(file);
        return NULL;
    }

    // Consume the newline character following the max value
    fgetc(file);

    Image* img = (Image*)malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->data = (Pixel*)malloc(width * height * sizeof(Pixel));

    fread(img->data, sizeof(Pixel), width * height, file);

    fclose(file);
    return img;
}


void writePPM(const char* filename, Image* img) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    fprintf(file, "P6\n%d %d\n255\n", img->width, img->height);
    fwrite(img->data, sizeof(Pixel), img->width * img->height, file);

    fclose(file);
}

void resizeImage(Image* img, int newWidth, int newHeight) {
    Pixel* newData = (Pixel*)malloc(newWidth * newHeight * sizeof(Pixel));

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int origX = x * img->width / newWidth;
            int origY = y * img->height / newHeight;
            newData[y * newWidth + x] = img->data[origY * img->width + origX];
        }
    }

    free(img->data);
    img->data = newData;
    img->width = newWidth;
    img->height = newHeight;
}

void applyFilter(Image* img) {
    // Edge detection kernel
    int kernel[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    Image* result = (Image*)malloc(sizeof(Image));
    result->width = img->width;
    result->height = img->height;
    result->data = (Pixel*)malloc(img->width * img->height * sizeof(Pixel));

    for (int y = 1; y < img->height - 1; ++y) {
        for (int x = 1; x < img->width - 1; ++x) {
            int sumRed = 0, sumGreen = 0, sumBlue = 0;

            // Apply the kernel
            for (int ky = 0; ky < 3; ++ky) {
                for (int kx = 0; kx < 3; ++kx) {
                    int pixelX = x - 1 + kx;
                    int pixelY = y - 1 + ky;

                    sumRed += img->data[pixelY * img->width + pixelX].red * kernel[ky][kx];
                    sumGreen += img->data[pixelY * img->width + pixelX].green * kernel[ky][kx];
                    sumBlue += img->data[pixelY * img->width + pixelX].blue * kernel[ky][kx];
                }
            }

            // Clamp values to the range [0, 255]
            result->data[y * img->width + x].red = (unsigned char)fmin(255, fmax(0, sumRed));
            result->data[y * img->width + x].green = (unsigned char)fmin(255, fmax(0, sumGreen));
            result->data[y * img->width + x].blue = (unsigned char)fmin(255, fmax(0, sumBlue));
        }
    }

    // Copy the result back to the original image
    memcpy(img->data, result->data, img->width * img->height * sizeof(Pixel));

    // Free memory
    free(result->data);
    free(result);
}


void adjustBrightness(Image* img, int delta) {
    for (int i = 0; i < img->width * img->height; ++i) {
        img->data[i].red = (unsigned char)fmin(255, fmax(0, img->data[i].red + delta));
        img->data[i].green = (unsigned char)fmin(255, fmax(0, img->data[i].green + delta));
        img->data[i].blue = (unsigned char)fmin(255, fmax(0, img->data[i].blue + delta));
    }
}

void adjustContrast(Image* img, double factor) {
    for (int i = 0; i < img->width * img->height; ++i) {
        img->data[i].red = (unsigned char)fmin(255, fmax(0, (img->data[i].red - 128) * factor + 128));
        img->data[i].green = (unsigned char)fmin(255, fmax(0, (img->data[i].green - 128) * factor + 128));
        img->data[i].blue = (unsigned char)fmin(255, fmax(0, (img->data[i].blue - 128) * factor + 128));
    }
}

void adjustSaturation(Image* img, double factor) {
    for (int i = 0; i < img->width * img->height; ++i) {
        // Convert RGB to HSV
        double r = img->data[i].red / 255.0;
        double g = img->data[i].green / 255.0;
        double b = img->data[i].blue / 255.0;

        double max = fmax(r, fmax(g, b));
        double min = fmin(r, fmin(g, b));

        double h, s, v;
        v = max;
        if (max == 0) {
            s = 0;
        }
        else {
            s = (max - min) / max;
        }

        if (s == 0) {
            h = 0;
        }
        else {
            if (max == r) {
                h = (g - b) / (max - min) + (g < b ? 6 : 0);
            }
            else if (max == g) {
                h = (b - r) / (max - min) + 2;
            }
            else {
                h = (r - g) / (max - min) + 4;
            }
            h /= 6;
        }

        // Adjust saturation
        s *= factor;

        // Convert back to RGB
        int iMax = (int)(h * 6);
        double f = h * 6 - iMax;

        double p = v * (1 - s);
        double q = v * (1 - f * s);
        double t = v * (1 - (1 - f) * s);

        switch (iMax % 6) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
            r = v;
            g = p;
            b = q;
            break;
        }

        img->data[i].red = (unsigned char)(r * 255);
        img->data[i].green = (unsigned char)(g * 255);
        img->data[i].blue = (unsigned char)(b * 255);
    }
}

// Function to create an image
Image* createImage(int width, int height) {
    Image* img = (Image*)malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->data = (Pixel*)calloc(width * height, sizeof(Pixel)); // Initialize to black
    return img;
}

// Function to draw a line using Bresenham's algorithm
void drawLine(Image* img, int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;

    while (1) {
        // Draw the pixel at (x1, y1)
        if (x1 >= 0 && x1 < img->width && y1 >= 0 && y1 < img->height) {
            img->data[y1 * img->width + x1].red = 255;
            img->data[y1 * img->width + x1].green = 255;
            img->data[y1 * img->width + x1].blue = 255;
        }

        if (x1 == x2 && y1 == y2) {
            break;
        }

        int e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Function to fill a triangle using barycentric coordinates (as called - area coordinates)
void fillTriangle(Image* img, int x1, int y1, int x2, int y2, int x3, int y3) {
    for (int y = fmin(y1, fmin(y2, y3)); y <= fmax(y1, fmax(y2, y3)); ++y) {
        for (int x = fmin(x1, fmin(x2, x3)); x <= fmax(x1, fmax(x2, x3)); ++x) {
            double alpha = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) /
                ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
            double beta = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) /
                ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
            double gamma = 1.0 - alpha - beta;

            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                // Point (x, y) is inside the triangle
                if (x >= 0 && x < img->width && y >= 0 && y < img->height) {
                    img->data[y * img->width + x].red = 255;
                    img->data[y * img->width + x].green = 255;
                    img->data[y * img->width + x].blue = 255;
                }
            }
        }
    }
}

