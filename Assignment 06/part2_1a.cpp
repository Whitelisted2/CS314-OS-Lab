#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<math.h>
#include<stdbool.h>
#include<chrono>
#include<thread>
#include<atomic>
using namespace std;
using namespace std::chrono;

atomic_flag molecule = ATOMIC_FLAG_INIT;
int position[2] = {0,0};

struct Pixel {
    unsigned char r,g,b;
};

typedef std::vector<std::vector<Pixel>> Image;

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

void GrayScale (Image &image) {
    //luminosity method for pixel to grayscale
    for (int y=0; y<image.size(); y++) {
        for(int x=0; x <image[y].size();x++) {
            while(atomic_flag_test_and_set(&molecule));
            int gray = 0.21*image[y][x].r + 0.72*image[y][x].g + 0.07*image[y][x].b;
            unsigned char graypix = static_cast<unsigned char>(gray);
            image[y][x].r=graypix;
            image[y][x].g=graypix;
            image[y][x].b=graypix;
            position[0] = x+1;
            position[1] = y+1;
            atomic_flag_clear(&molecule);
        }
    }
}

void BLUR(Image &image) {
  // Define the box filter kernel
  const std::vector<std::vector<double>> kernel = {
      {0.0625, 0.125, 0.0625},
      {0.125, 0.25, 0.125},
      {0.0625, 0.125, 0.0625}
  };

  // Create a temporary copy of the image to avoid modifying pixels multiple times
  std::vector<std::vector<Pixel>> temp_image = image;

  // Loop over each pixel in the image
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

// Calculate the gradient magnitude of the image using Sobel edge detection
// Calculate the gradient magnitude of the image using Sobel edge detection
void EDGE(Image& pixels) {
    // Create a temporary copy of the input pixels
    Image tempPixels = pixels;
    
    // Define the Sobel operator kernels
    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    bool PREVENT_ERROR = false;
    // Loop over each pixel in the input pixels
    for (int y = 1; y < pixels.size() - 1; y++) {
        for (int x = 1; x < pixels[y].size() - 1; x++) {
            while(atomic_flag_test_and_set(&molecule));
            // Calculate the gradient magnitude of the pixel using the Sobel kernels

            if(position[0]>=x+1||position[1]>=y+1) {
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
            PREVENT_ERROR = false;
            }
            else {
                PREVENT_ERROR = true;
            }
            atomic_flag_clear(&molecule);
            if(PREVENT_ERROR) x-=1;
        }
        if(PREVENT_ERROR) y-=1;
    }
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
    
    // Read the input PPM file
    Image pixels;
    int width, height;
    readPPM(argv[1], pixels, width, height);
    auto start = startTime();
    thread T1 (GrayScale, std::ref(pixels));
    thread T2 (EDGE, std::ref(pixels));
    // Apply the first transformation (e.g. grayscale)
    T1.join(); 
    
    // Apply the second transformation (e.g. edge detection)
    T2.join(); 
    auto stop = stopTime();
    // Write the output PPM file
    writePPM(argv[2], pixels, width, height);
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time Elapsed: " << duration.count() << " microseconds" << endl;
    // Exit the program
    return 0;
}

