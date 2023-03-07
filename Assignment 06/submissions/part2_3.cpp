#include <bits/stdc++.h>
#include <cmath>
#include <vector>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>
#include <atomic>

using namespace std;
using namespace std::chrono;
struct Pixel {
    unsigned char r,g,b;
};

struct PPMHEADER {
    int height;
    int width;
    int maxcolor;
    char PPMC[100];
};

typedef vector<vector<Pixel>> Image;

void readPPM(char *filename,Image & image, int& width, int& height) {
  std::ifstream inFile(filename, std::ios::binary);

  if (inFile.is_open()) {
    std::string line;
    std::getline(inFile, line);

    if (line != "P6") {
      std::cerr << "Error: Invalid PPM file format\n";
      return;
    }

    std::getline(inFile, line);

    while (line[0] == '#') {
      std::getline(inFile, line);
    }

    std::stringstream ss(line);
    ss >> width >> height;

    std::getline(inFile, line);

    int maxVal = std::stoi(line);

    if (maxVal != 255) {
      std::cerr << "Error: Invalid PPM file format\n";
      return;
    }

    image.resize(height, std::vector<Pixel>(width));

    char pixel[3];
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        inFile.read(pixel, 3);
        image[i][j].r = pixel[0];
        image[i][j].g = pixel[1];
        image[i][j].b = pixel[2];
      }
    }

    inFile.close();
  } else {
    std::cerr << "Error: Could not open input file\n";
    return;
  }
}

void T1 (char *inputFile, int pid, int pipefds[2], int pipefds2[2]) {
    if(pid>0){
        return;
    }
    int height, width, colormax;
    int r, g, b;
    FILE *inputImage = fopen(inputFile, "r");

    struct PPMHEADER storeHeader[1];

    fscanf(inputImage, "%s%d%d%d", storeHeader[0].PPMC, &width, &height, &colormax);
    storeHeader[0].height = height;
    storeHeader[0].width = width;
    storeHeader[0].maxcolor = colormax;
    write(pipefds2[1], storeHeader, sizeof(struct PPMHEADER));

    vector<vector<Pixel>> image;

    readPPM(inputFile, image, width, height);
    //luminosity method for pixel to grayscale
    for (int y=0; y<image.size(); y++) {
        for(int x=0; x <image[y].size();x++) {
            int gray = 0.21*image[y][x].r + 0.72*image[y][x].g + 0.07*image[y][x].b;
            unsigned char graypix = static_cast<unsigned char>(gray);
            image[y][x].r=graypix;
            image[y][x].g=graypix;
            image[y][x].b=graypix;
        }
    }
    // struct Pixel pixelMatrix[9];
    // for (int i = 0; i <= height - 3; i += 3){
    // for (int j = 0; j <= width - 3; j += 3){
            
    //         pixelMatrix[0] = arrayOfData[i][j];
    //         pixelMatrix[1] = arrayOfData[i][j + 1];
    //         pixelMatrix[2] = arrayOfData[i][j + 2];

    //         pixelMatrix[3] = arrayOfData[i + 1][j];
    //         pixelMatrix[4] = arrayOfData[i + 1][j + 1];
    //         pixelMatrix[5] = arrayOfData[i + 1][j + 2];

    //         pixelMatrix[6] = arrayOfData[i + 2][j];
    //         pixelMatrix[7] = arrayOfData[i + 2][j + 1];
    //         pixelMatrix[8] = arrayOfData[i + 2][j + 2];

    //         write(pipefds[1], pixelMatrix, sizeof(pixelMatrix));
    //     }
    // }
    for(int y = 0; y<image.size(); y++) {
        for(int x = 0; x<image[y].size(); x++) {
            struct Pixel tempPix[1];
            tempPix[0] = image[y][x];
            write(pipefds[1], tempPix, sizeof(tempPix));
    }}
    // ofstream fout("testing.ppm", ios::out | ios::binary);
    
    // // Write the PPM header
    // fout << "P6\n" << width << " " << height << "\n" << 255 << "\n";
    
    // // Write the pixel data
    // for (int i = 0; i < height; i++) {
    //     for (int j = 0; j < width; j++) {
    //         fout << image[i][j].r << image[i][j].g << image[i][j].b;
    //     }
    // }
    
    // // Close the output file
    // fout.close();
    exit(EXIT_SUCCESS);
}

void T2(Image &image) {
  // Define the box filter kernel
  const std::vector<std::vector<double>> kernel = {
      // {1.0 / 20, 1.0 / 10, 1.0 / 20},
      // {1.0 / 10, 1.0 / 10, 1.0 / 10},
      // {1.0 / 20, 1.0 / 10, 1.0 / 20}
      {0.0625, 0.125, 0.0625},
      {0.125, 0.25, 0.125},
      {0.0625, 0.125, 0.0625}
  };

  // Create a temporary copy of the image to avoid modifying pixels multiple times
  std::vector<std::vector<Pixel>> temp_image = image;

  // Loop over each pixel in the image
  int num_iter = 50;
  for(int i = 0; i < num_iter; i++){
    for (int y = 1; y < image.size() - 1; y++) {
      for (int x = 1; x < image[y].size() - 1; x++) {
        // Initialize the sum of colors for the surrounding pixels to zero
        double sum_r = 0;
        double sum_g = 0;
        double sum_b = 0;

        // Loop over each pixel in the kernel
        for (int ky = -1; ky <= 1; ky++) {
          for (int kx = -1; kx <= 1; kx++) {
            // Compute the index of the neighboring pixel
            int ny = y + ky;
            int nx = x + kx;

            // Multiply the color of the neighboring pixel by the corresponding kernel value
            sum_r += kernel[ky + 1][kx + 1] * image[ny][nx].r;
            sum_g += kernel[ky + 1][kx + 1] * image[ny][nx].g;
            sum_b += kernel[ky + 1][kx + 1] * image[ny][nx].b;
          }
        }

        // Set the color of the current pixel to the weighted average of its neighbors
        temp_image[y][x].r = static_cast<unsigned char>(sum_r);
        temp_image[y][x].g = static_cast<unsigned char>(sum_g);
        temp_image[y][x].b = static_cast<unsigned char>(sum_b);
      }
    }

    // Copy the modified pixels back to the original image
    image = temp_image;
  }
}

// Calculate the gradient magnitude of the image using Sobel edge detection
// Ref: https://www.cs.auckland.ac.nz/compsci373s1c/PatricesLectures/Edge%20detection-Sobel_2up.pdf 
void T3(char *outputFile, int pid, int pipefds[2], int pipefds2[2]) {
    if(pid > 0){
        return;
    }
    // Create a temporary copy of the input pixels
    struct Pixel pixelMatrix[9];
    struct PPMHEADER header[1];
    read(pipefds2[0], header, sizeof(header));
    int width = header[0].width;
    int height = header[0].height;
    int colormax = header[0].maxcolor;
    vector<vector<Pixel>> pixels(height, vector<Pixel>(width));
    for(int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            struct Pixel tempPixel[1];
            read(pipefds[0], tempPixel, sizeof(tempPixel));
            pixels[y][x] = tempPixel[0];
        }
    }
    vector<vector<Pixel>> tempPixels = pixels;
    // Define the Sobel operator kernels
    int gx[3][3] = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    // Loop over each pixel in the input pixels
    for (int y = 1; y < pixels.size() - 1; y++) {
        for (int x = 1; x < pixels[y].size() - 1; x++) {
            // Calculate the gradient magnitude of the pixel using the Sobel kernels
            int gx_sum = 0, gy_sum = 0;
            for (int j = -1; j <= 1; j++) {
                for (int i = -1; i <= 1; i++) {
                    gx_sum += gx[j + 1][i + 1] * tempPixels[y + j][x + i].r;
                    gy_sum += gy[j + 1][i + 1] * tempPixels[y + j][x + i].r;
                }
            }
            int mag = sqrt(gx_sum * gx_sum + gy_sum * gy_sum);
            
            // Set the RGB values of the pixel to the gradient magnitude
            pixels[y][x].r = mag;
            pixels[y][x].g = mag;
            pixels[y][x].b = mag;
        }
    }
    ofstream fout(outputFile, ios::out | ios::binary);
    
    // Write the PPM header
    fout << "P6\n" << width << " " << height << "\n" << 255 << "\n";
    
    // Write the pixel data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fout << pixels[i][j].r << pixels[i][j].g << pixels[i][j].b;
        }
    }
    
    // Close the output file
    fout.close();
    exit(EXIT_SUCCESS);
}

void writePPM(string filename, Image & pixels, int width, int height) {
    // Open the output file
    ofstream fout(filename, ios::out | ios::binary);
    
    // Write the PPM header
    fout << "P6\n" << width << " " << height << "\n" << 255 << "\n";
    
    // Write the pixel data
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fout << pixels[i][j].r << pixels[i][j].g << pixels[i][j].b;
        }
    }
    
    // Close the output file
    fout.close();
}
auto startTime(){
    auto start = chrono::high_resolution_clock::now();
    return start;
}

auto stopTime(){
    auto stop = chrono::high_resolution_clock::now();
    return stop;
}

int main(int argc, char *argv[]) {
    // Check that the input arguments are valid
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <path-to-original-image> <path-to-transformed-image>" << endl;
        return 1;
    }
    int pipefds[2];
    if(pipe(pipefds) == -1) {
        perror("pipe");
    }
    int pipefds2[2];
    if(pipe(pipefds2) == -1) {
        perror("pipe");
    }
    // Read the input PPM file
    Image pixels;
    int width, height;
    readPPM(argv[1], pixels, width, height);
    auto start = startTime();
    // Apply the first transformation (e.g. grayscale)
    T1(argv[1],fork(),pipefds,pipefds2);

    // Apply the second transformation (e.g. image blur)
    // T2(pixels);
    
    // Apply the third transformation (e.g. edge detection)
    T3(argv[2],fork(),pipefds,pipefds2);
    wait(NULL);
    wait(NULL);
    auto stop = stopTime();
    // Write the output PPM file
    // writePPM(argv[2], pixels, width, height);
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time Elapsed: " << duration.count() << " microseconds" << endl;
    // Exit the program
    return 0;
}

