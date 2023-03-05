#include <bits/stdc++.h>
#include <chrono>
#include <cmath>
#include <unistd.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <vector>
#include <semaphore.h>
#include <sys/types.h>
#include <thread>
#include <sys/wait.h>

using namespace std;
using namespace std::chrono;
sem_t sema;
int position[2] = {0,0};

struct Pixel {
    unsigned char r,g,b;
};

typedef std::vector<std::vector<Pixel>> Image;
Image globalImage;
void readPPM(char *filename,key_t key, int width, int height) {
  std::ifstream inFile(filename, std::ios::binary);
struct Pixel * sharedImage;
  if (inFile.is_open()) {
    std::string line;
    std::getline(inFile, line);

    if (line != "P6") {
      std::cerr << "Error: Invalid PPM file format\n";
      return;
    }

    std::getline(inFile, line);
    int a, b;
    while (line[0] == '#') {
      std::getline(inFile, line);
    }

    std::stringstream ss(line);
    ss >> a >> b;
    //attach memory 
    int shmid = shmget(key, sizeof(struct Pixel)*(height)*(width),0666|IPC_CREAT);

    sharedImage = (struct Pixel*)shmat(shmid,NULL,0);

    std::getline(inFile, line);

    int maxVal = std::stoi(line);

    if (maxVal != 255) {
      std::cerr << "Error: Invalid PPM file format\n";
      return;
    }

    globalImage.resize(height, std::vector<Pixel>(width));

    char pixel[3];
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        inFile.read(pixel, 3);
        globalImage[i][j].r = pixel[0];
        globalImage[i][j].g = pixel[1];
        globalImage[i][j].b = pixel[2];
      }
    }

    inFile.close();
  } else {
    std::cerr << "Error: Could not open input file\n";
    return;
  }
    for(int i = height - 1; i > 0; i--){
        for(int j = 1; j <= width - 1; j++){
            sharedImage[i * width + j] = globalImage[i][j];
        }
    }
}

void T1 (key_t positionKey, key_t sharedDataKey, int pid, int height, int width){
    //kill function call if parent process 
    if(pid > 0) return;
    //luminosity method for pixel to grayscale
    //shared data initializer for both Image Data and Position Data (conflict resolution)
    int shmid = shmget(sharedDataKey, sizeof(struct Pixel) * width *height,0666|IPC_CREAT);
    struct Pixel *sharedImage;
    sharedImage = (struct Pixel *)shmat(shmid,NULL,0);
    //Semaphore open
    sem_t *sharedSemaphore = sem_open("/sharedSemaphore",O_RDWR);
    int *position;
    int positionShm = shmget(positionKey, sizeof(int) *2, 0666 | IPC_CREAT);
    position = (int *)shmat(positionShm,NULL,0);
    //Temporary struct for storing 
    struct Pixel temp;
    for (int y=0; y<height ; y++) {
        for(int x=0; x <width;x++) {
            sem_wait(sharedSemaphore);
            temp = sharedImage[y*width + x];
            int gray = 0.21*temp.r + 0.72*temp.g + 0.07*temp.b;
            unsigned char graypix = static_cast<unsigned char>(gray);
            temp.r=graypix;
            temp.g=graypix;
            temp.b=graypix;
            //push it back
            sharedImage[y*width + x] = temp;
            position[0] = x+1;
            position[1] = y+1;
            sem_post(sharedSemaphore);
        }
    }
    if(pid == 0 ) exit(0);
}

void T2(key_t positionKey, key_t sharedDataKey, int pid, int height, int width) {
  //kill function call if parent 
  if(pid > 0)
    return;
  //shared data initializer for both Image Data and Position Data
  int shmid = shmget(sharedDataKey, sizeof(struct Pixel) * width * height, 0666|IPC_CREAT);
  struct Pixel *sharedImage;
  sharedImage = (struct Pixel *) shmat(shmid, NULL, 0);
  //Semaphore time
  sem_t *sharedSemaphore = sem_open("/sharedSemaphore",O_RDWR);
  int *position;
  int positionShm = shmget(positionKey, sizeof(int) * 2, 0666|IPC_CREAT);
  position = (int *) shmat(positionShm, NULL, 0);
  struct Pixel temp;
  // Define the box filter kernel
  const std::vector<std::vector<double>> kernel = {
      {0.0625, 0.125, 0.0625},
      {0.125, 0.25, 0.125},
      {0.0625, 0.125, 0.0625}
  };
  struct Pixel *tempShared;
  tempShared = (struct Pixel*)malloc(sizeof(struct Pixel) * width * height);
  for(int y = 0; y < height ; y++) {
    for (int x = 0; x < width ; x++) {
        struct Pixel tempPixel = sharedImage[y * width + x];
        tempShared[y * width + x] = tempPixel;
  }}
  // Create a temporary copy of the image to avoid modifying pixels multiple times (not needed here)
//   std::vector<std::vector<Pixel>> temp_image = image;
  
  // Loop over each pixel in the image
  for (int y = 1; y < height - 1; y++) {
    for (int x = 1; x < width - 1; x++) {
    
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
          struct Pixel tempInner = sharedImage[ny * width + nx];       
          // Multiply the color of the neighboring pixel by the corresponding kernel value
          sum_r += kernel[ky + 1][kx + 1] * tempInner.r;
          sum_g += kernel[ky + 1][kx + 1] * tempInner.g;
          sum_b += kernel[ky + 1][kx + 1] * tempInner.b;
        }
      }
    struct Pixel temp;
      // Set the color of the current pixel to the weighted average of its neighbors
      temp.r = static_cast<unsigned char>(sum_r);
      temp.g = static_cast<unsigned char>(sum_g);
      temp.b = static_cast<unsigned char>(sum_b);
      tempShared[y*width + x ] = temp;
    }
  }

  // Copy the modified pixels back to the original image
  for (int y=0 ; y < height ; y++){
    for (int x=0 ; x < width ; x++){
        struct Pixel tempPixel;
        tempPixel = tempShared[y*width + x];
        sharedImage[y*width + x] = tempPixel;
  }
}}

// Calculate the gradient magnitude of the image using Sobel edge detection
void T3(key_t positionKey, key_t sharedDataKey, int pid, int height, int width) {
    //kill function call if parent 
    if(pid > 0) 
        return;
    // Create a temporary copy of the input pixels
    // Image tempPixels = globalImage;
    //shared data access 
    int shmid = shmget(sharedDataKey, sizeof(struct Pixel) * width * height, 0666|IPC_CREAT);
    struct Pixel *sharedData;
    sharedData = (struct Pixel *)shmat(shmid,NULL,0);
    //access semaphore 
    sem_t *sharedSemaphore = sem_open("/sharedSemaphore",O_RDWR);
    int * position;
    int positionShm = shmget(positionKey, sizeof(int) * 2, 0666|IPC_CREAT);
    position = (int *)shmat(positionShm,NULL,0);
    // Define the Sobel operator kernels
    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    bool PREVENT_ERROR = false;
    // Loop over each pixel in the input pixels
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            sem_wait(sharedSemaphore);
            // Calculate the gradient magnitude of the pixel using the Sobel kernels

            if(position[0]>=x+1||position[1]>=y+1) {
            int gx_sum = 0, gy_sum = 0;
            for (int j = -1; j <= 1; j++) {
                for (int i = -1; i <= 1; i++) {
                    gx_sum += gx[j + 1][i + 1] * globalImage[y + j][x + i].r;
                    gy_sum += gy[j + 1][i + 1] * globalImage[y + j][x + i].r;
                }
            }
            int mag = sqrt(gx_sum * gx_sum + gy_sum * gy_sum);
            
            // Set the RGB values of the pixel to the gradient magnitude
            struct Pixel tempPixel;
            tempPixel.r = mag;
            tempPixel.g = mag;
            tempPixel.b = mag;
            sharedData[y*width + x] = tempPixel;
            PREVENT_ERROR = false;
            }
            else {
                PREVENT_ERROR = true;
            }
            sem_post(sharedSemaphore);
            if(PREVENT_ERROR) x-=1;
        }
        if(PREVENT_ERROR) y-=1;
    }
    if(pid == 0 )
        exit(0);
}

// void writePPM(string filename, Image & pixels, int width, int height) {
//     // Open the output file
//     ofstream fout(filename, ios::out | ios::binary);
    
//     // Write the PPM header
//     fout << "P6\n" << width << " " << height << "\n" << 255 << "\n";
    
//     // Write the pixel data
//     for (int i = 0; i < height; i++) {
//         for (int j = 0; j < width; j++) {
//             fout << pixels[i][j].r << pixels[i][j].g << pixels[i][j].b;
//         }
//     }
    
//     // Close the output file
//     fout.close();
// }
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
    char PPV[10];
    int width, height,max;
    key_t key = 0x1234;
    key_t positionKey = 0x1235;
    FILE *inputImage = fopen(argv[1], "r");
    fscanf(inputImage, "%s%d%d%d", PPV, &width, &height, &max);
    fclose(inputImage);
    

    // Read the input PPM file
    struct Pixel *sharedImage;
    int shmid = shmget(key, sizeof(struct Pixel) * (height) * width, 0666 | IPC_CREAT);
    sharedImage = (struct Pixel *)shmat(shmid, NULL, 0);
    readPPM(argv[1], key, width, height);

    // Apply the first transformation (e.g. grayscale)
    
    sem_t *binarySemaphore = sem_open("/sharedSemaphore", O_CREAT | O_EXCL, (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP), 1);
    auto start = startTime();
    T1(positionKey, key, fork(), height, width);
    T3(positionKey, key, fork(), height, width);
    wait(NULL);
    wait(NULL);
    auto stop = stopTime();
    // wait(NULL);
    // // Apply the second transformation (e.g. edge detection)
    // T3(pixels);
    
    // Write the output PPM file
    // writePPM(argv[2], pixels, width, height);
    ofstream fout(argv[2], ios::out | ios::binary);
    fout << "P6\n" << width << " " << height << "\n" << 255 << "\n";
    for(int y=0; y < height; y++){
        for(int x=0; x < width; x++) {
            struct Pixel tempPixel = sharedImage[y*width + x];
            fout << tempPixel.r << tempPixel.g << tempPixel.b;
        }
    }
        auto duration = duration_cast<microseconds>(stop - start);
            cout << "Time Elapsed: " << duration.count() << " microseconds" << endl;
    // Exit the program
    return 0;
}

