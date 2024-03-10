#include "fractal_generation.h"
#include <stdlib.h>
#include <math.h>

// Function to calculate mid-point between two points
Complex midpoint(Complex p1, Complex p2) {
    Complex mid;
    mid.real = (p1.real + p2.real) / 2.0;
    mid.imag = (p1.imag + p2.imag) / 2.0;
    return mid;
}

Image* generateMandelbrot(int width, int height, double xmin, double xmax, double ymin, double ymax, int maxIterations) {
    Image* img = (Image*)malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->data = (Pixel*)malloc(width * height * sizeof(Pixel));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double zx, zy, cx, cy;
            zx = x * (xmax - xmin) / (width - 1) + xmin;
            zy = y * (ymax - ymin) / (height - 1) + ymin;
            cx = zx;
            cy = zy;
            int iteration = 0;

            while (iteration < maxIterations) {
                double xtemp = zx * zx - zy * zy + cx;
                zy = 2 * zx * zy + cy;
                zx = xtemp;

                if ((zx * zx + zy * zy) > 4.0) {
                    break;
                }

                iteration++;
            }

            // Color the pixel based on the number of iterations
            double colorRatio = (double)iteration / maxIterations;
            img->data[y * width + x].red = (unsigned char)(255 * colorRatio);
            img->data[y * width + x].green = (unsigned char)(255 * colorRatio);
            img->data[y * width + x].blue = (unsigned char)(255 * colorRatio);
        }
    }

    return img;
}

Image* generateJulia(int width, int height, double xmin, double xmax, double ymin, double ymax, int maxIterations, Complex c) {
    Image* img = (Image*)malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->data = (Pixel*)malloc(width * height * sizeof(Pixel));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double zx, zy, cx, cy;
            zx = x * (xmax - xmin) / (width - 1) + xmin;
            zy = y * (ymax - ymin) / (height - 1) + ymin;
            cx = c.real;
            cy = c.imag;
            int iteration = 0;

            while (iteration < maxIterations) {
                double xtemp = zx * zx - zy * zy + cx;
                zy = 2 * zx * zy + cy;
                zx = xtemp;

                if ((zx * zx + zy * zy) > 4.0) {
                    break;
                }

                iteration++;
            }

            // Color the pixel based on the number of iterations
            double colorRatio = (double)iteration / maxIterations;
            img->data[y * width + x].red = (unsigned char)(255 * colorRatio);
            img->data[y * width + x].green = (unsigned char)(255 * colorRatio);
            img->data[y * width + x].blue = (unsigned char)(255 * colorRatio);
        }
    }

    return img;
}

void generateKochCurveRec(Image* img, int iterations, int x1, int y1, int x2, int y2) {
    if (iterations == 0) {
        // Base case: draw a line
        drawLine(img, x1, y1, x2, y2);
    }
    else {
        // Calculate intermediate points
        int x3 = x1 + (x2 - x1) / 3;
        int y3 = y1 + (y2 - y1) / 3;
        int x4 = x1 + 2 * (x2 - x1) / 3;
        int y4 = y1 + 2 * (y2 - y1) / 3;
        int x5 = (x1 + x4) / 2 + (y4 - y3) * sqrt(3) / 2;
        int y5 = (y1 + y4) / 2 - (x4 - x3) * sqrt(3) / 2;

        // Recursively draw segments
        generateKochCurveRec(img, iterations - 1, x1, y1, x3, y3);
        generateKochCurveRec(img, iterations - 1, x3, y3, x5, y5);
        generateKochCurveRec(img, iterations - 1, x5, y5, x4, y4);
        generateKochCurveRec(img, iterations - 1, x4, y4, x2, y2);
    }
}

Image* generateKochCurve(int iterations) {
    Image* img = createImage(800, 800);

    // Set the initial points of the curve
    int x1 = 50, y1 = 400;
    int x2 = 750, y2 = 400;

    // Generate the Koch curve recursively
    generateKochCurveRec(img, iterations, x1, y1, x2, y2);

    return img;
}

Image* generateBarnsleyFern(int iterations) {
    int numPoints = iterations * 1000; // Adjust the multiplier based on the desired density
    Complex* points = (Complex*)malloc(numPoints * sizeof(Complex));

    // Initialize the first point
    points[0].real = 0.0;
    points[0].imag = 0.0;

    // Apply Barnsley Fern transformation
    for (int i = 1; i < numPoints; ++i) {
        double random = (double)rand() / RAND_MAX;

        if (random < 0.01) {
            // Transformation 1
            points[i].real = 0.0;
            points[i].imag = 0.16 * points[i - 1].imag;
        }
        else if (random < 0.86) {
            // Transformation 2
            points[i].real = 0.85 * points[i - 1].real + 0.04 * points[i - 1].imag;
            points[i].imag = -0.04 * points[i - 1].real + 0.85 * points[i - 1].imag + 1.6;
        }
        else if (random < 0.93) {
            // Transformation 3
            points[i].real = 0.2 * points[i - 1].real - 0.26 * points[i - 1].imag;
            points[i].imag = 0.23 * points[i - 1].real + 0.22 * points[i - 1].imag + 1.6;
        }
        else {
            // Transformation 4
            points[i].real = -0.15 * points[i - 1].real + 0.28 * points[i - 1].imag;
            points[i].imag = 0.26 * points[i - 1].real + 0.24 * points[i - 1].imag + 0.44;
        }
    }

    // Create an image and plot points
    Image* img = (Image*)malloc(sizeof(Image));
    img->width = 800;
    img->height = 800;
    img->data = (Pixel*)calloc(img->width * img->height, sizeof(Pixel)); // Initialize to black

    // Scale and plot points on the image
    for (int i = 0; i < numPoints; ++i) {
        int x = (int)(img->width / 2 + points[i].real * img->width / 11);
        int y = (int)(img->height - points[i].imag * img->height / 12);
        if (x >= 0 && x < img->width && y >= 0 && y < img->height) {
            img->data[y * img->width + x].red = 255;
            img->data[y * img->width + x].green = 255;
            img->data[y * img->width + x].blue = 255;
        }
    }

    free(points);
    return img;
}


Image* generateSierpinskiTriangle(int iterations) {
    int numPoints = pow(3, iterations) + 1;
    Complex* points = (Complex*)malloc(numPoints * sizeof(Complex));

    // Initialize the points
    points[0].real = 0.0;
    points[0].imag = 0.0;

    for (int i = 1; i < numPoints; ++i) {
        double angle = i * 2.0 * M_PI / 3.0;
        points[i].real = cos(angle);
        points[i].imag = sin(angle);
    }

    // TODO: Implement logic

    Image* img = (Image*)malloc(sizeof(Image));
    img->width = 800;
    img->height = 800;
    img->data = (Pixel*)malloc(img->width * img->height * sizeof(Pixel));

    // Clear the image to white
    for (int i = 0; i < img->width * img->height; ++i) {
        img->data[i].red = 255;
        img->data[i].green = 255;
        img->data[i].blue = 255;
    }

    free(points);
    return img;
}
