#include "image_processing.h"
#include "fractal_generation.h"
#include <stdio.h>

int main() {
    // Test image processing functions
    Image* originalImage = readPPM("sample_1280×853.ppm");
    if (!originalImage) {
        fprintf(stderr, "Error reading input image\n");
        return 1;
    }

    // Resize image
    resizeImage(originalImage, 400, 400);

    // Adjust brightness
    adjustBrightness(originalImage, 50);

    // Adjust contrast
    adjustContrast(originalImage, 1.5);

    // Adjust saturation
    adjustSaturation(originalImage, 1.5);

    // Apply filter
    applyFilter(originalImage);

    // Write the processed image to a new file
    writePPM("output_image.ppm", originalImage);

    // Free memory
    free(originalImage->data);
    free(originalImage);

    // Test fractal generation functions
    Image* mandelbrotImage = generateMandelbrot(800, 800, -2.0, 2.0, -2.0, 2.0, 100);
    writePPM("mandelbrot.ppm", mandelbrotImage);
    free(mandelbrotImage->data);
    free(mandelbrotImage);

    Complex juliaConstant = { -0.7, 0.27015 };
    Image* juliaImage = generateJulia(800, 800, -2.0, 2.0, -2.0, 2.0, 100, juliaConstant);
    writePPM("julia.ppm", juliaImage);
    free(juliaImage->data);
    free(juliaImage);

    // Test other fractals
    Image* kochCurveImage = generateKochCurve(10);
    writePPM("koch_curve.ppm", kochCurveImage);
    free(kochCurveImage->data);
    free(kochCurveImage);

    //Image* barnsleyFernImage = generateBarnsleyFern(100000);
    //writePPM("barnsley_fern.ppm", barnsleyFernImage);
    //free(barnsleyFernImage->data);
    //free(barnsleyFernImage);

    Image* sierpinskiTriangleImage = generateSierpinskiTriangle(5);
    writePPM("sierpinski_triangle.ppm", sierpinskiTriangleImage);
    free(sierpinskiTriangleImage->data);
    free(sierpinskiTriangleImage);

    return 0;
}
