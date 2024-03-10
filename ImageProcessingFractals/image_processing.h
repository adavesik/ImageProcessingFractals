#pragma once

#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

typedef struct {
    unsigned char red, green, blue;
} Pixel;

typedef struct {
    int width, height;
    Pixel* data;
} Image;

Image* readPPM(const char* filename);
void writePPM(const char* filename, Image* img);
void resizeImage(Image* img, int newWidth, int newHeight);
void applyFilter(Image* img);
void adjustBrightness(Image* img, int delta);
void adjustContrast(Image* img, double factor);
void adjustSaturation(Image* img, double factor);

Image* createImage(int width, int height);
void drawLine(Image* img, int x1, int y1, int x2, int y2);
void fillTriangle(Image* img, int x1, int y1, int x2, int y2, int x3, int y3);

#endif // IMAGE_PROCESSING_H
