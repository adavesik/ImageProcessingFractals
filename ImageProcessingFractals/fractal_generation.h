#pragma once

#include "image_processing.h"

#ifndef FRACTAL_GENERATION_H
#define FRACTAL_GENERATION_H

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif


typedef struct {
    double real, imag;
} Complex;

Image* generateMandelbrot(int width, int height, double xmin, double xmax, double ymin, double ymax, int maxIterations);
Image* generateJulia(int width, int height, double xmin, double xmax, double ymin, double ymax, int maxIterations, Complex c);
void generateKochCurveRec(Image* img, int iterations, int x1, int y1, int x2, int y2);
Image* generateKochCurve(int iterations);
Image* generateBarnsleyFern(int iterations);
Image* generateSierpinskiTriangle(int iterations);

#endif // FRACTAL_GENERATION_H
