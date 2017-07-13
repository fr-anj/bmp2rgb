#include <iostream>
#include <unistd.h>
#include <fstream>

using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;

#pragma pack(1)
#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

unsigned char** reds;
unsigned char** greens;
unsigned char** blues;
unsigned char** temp_reds;
unsigned char** temp_greens;
unsigned char** temp_blues;
int rows;
int cols;

//void ColorTest() {
//    // Makes Red Rectangle in top left corner. Rectangle stretches to right alot
//    for (int i = rows / 10; i < 3 * rows / 10; i++)
//        for (int j = cols / 10; j < 7 * cols / 10; j++)
//            reds[i][j] = 0xff;
//
//    // Makes small green box in bottom right
//    for (int i = 8 * rows / 10; i < rows; i++)
//        for (int j = 8 * cols / 10; j < cols; j++)
//            greens[i][j] = 0xff;
//
//    // Makes White box in the middle of the screeene    
//    for (int i = rows * 4 / 10; i < rows * 6 / 10; i++)
//        for (int j = cols * 4 / 10; j < cols * 6 / 10; j++) {
//            greens[i][j] = 0xff;
//            reds[i][j] = 0xff;
//            blues[i][j] = 0xff;
//        }
//
//    // Blue verticle rectangle bottom left
//    for (int i = rows * 6 / 10; i < rows; i++)
//        for (int j = cols * 0; j < cols * 1 / 10; j++)
//            blues[i][j] = 0xff;
//}

void RGB_Allocate(unsigned char**& dude) {
    dude = new unsigned char*[rows];
    for (int i = 0; i < rows; i++)
        dude[i] = new unsigned char[cols];
}

bool FillAndAllocate(char*& buffer, const char* Picture, int& rows, int& cols, int& BufferSize) { //Returns 1 if executed sucessfully, 0 if not sucessfull
    std::ifstream file(Picture);

    if (file) {
        file.seekg(0, std::ios::end);
        std::streampos length = file.tellg();
        file.seekg(0, std::ios::beg);

        buffer = new char[length];
        file.read(&buffer[0], length);

        PBITMAPFILEHEADER file_header;
        PBITMAPINFOHEADER info_header;

        file_header = (PBITMAPFILEHEADER) (&buffer[0]);
        info_header = (PBITMAPINFOHEADER) (&buffer[0] + sizeof(BITMAPFILEHEADER));
        rows = info_header->biHeight;
        cols = info_header->biWidth;
        BufferSize = file_header->bfSize;
        return 1;
    }
    else {
        cout << "File" << Picture << " don't Exist!" << endl;
        return 0;
    }
}

void GetPixlesFromBMP24(unsigned char** reds, unsigned char** greens, unsigned char** blues, int end, int rows, int cols, char* FileReadBuffer) { // end is BufferSize (total size of file)
    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
    for (int i = 0; i < rows; i++){
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
            for (int k = 0; k < 3; k++) {
                    switch (k) {
                    case 0:
                        reds[i][j] = FileReadBuffer[end - count++];
                        break;
                    case 1:
                        greens[i][j] = FileReadBuffer[end - count++];
                        break;
                    case 2:
                        blues[i][j] = FileReadBuffer[end - count++];
                        break;
                    }
            }
    }
}

void WriteOutBmp24(char* FileBuffer, const char* NameOfFileToCreate, int BufferSize) {
    std::ofstream write(NameOfFileToCreate);
    if (!write) {
        cout << "Failed to write " << NameOfFileToCreate << endl;
        return;
    }
    int count = 1;
    int extra = cols % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
    for (int i = 0; i < rows; i++){
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0: //reds
                    FileBuffer[BufferSize - count] = temp_reds[i][j];
                    break;
                case 1: //green
                    FileBuffer[BufferSize - count] = temp_greens[i][j];
                    break;
                case 2: //blue
                    FileBuffer[BufferSize - count] = temp_blues[i][j];
                    break;
                }
                count++;
            }
    }
    write.write(FileBuffer, BufferSize);
}

//-------------------------------------------------------------------------------

char filter (int hfilsel, unsigned char ar[]) {
    int coef[4][5] = { 
        {0,0,8,0,0},
        {0,1,6,1,0},
        {0,2,4,2,0},
        {1,2,2,2,1}
    };
    unsigned char filout;
    unsigned int temp; 
                        
    for (int j = 0; j < 5; j++) 
        temp =  temp + (coef[hfilsel][j] * (int)ar[j]);
        //filout =  filout + (coef[hfilsel][j] * (int)ar[j]);

    temp = temp / 8;

    return filout = temp;
    //return filout/8;
}

//horizontal antialiasing filter
void haafil (int hfilsel) {
    unsigned char win_r[] = {0,0,0,0,0};
    unsigned char win_g[] = {0,0,0,0,0};
    unsigned char win_b[] = {0,0,0,0,0};

    for (int i = 0; i < rows; i++) {
        //clear window
        cout << "line:" << i << endl;
        for (int k = 0; k < 5; k++) {
            win_r[k] = 0;
            win_g[k] = 0;
            win_b[k] = 0;
        }
        for (int j = cols-1; j >= 0; j--) {
            if (j < 2 || j >= cols-2) {
                temp_reds[i][j] = reds[i][j];
                temp_greens[i][j] = greens[i][j];
                temp_blues[i][j] = blues[i][j];
            }
        }
#ifdef MAMO 
        for (int j = cols+1; j >= 0; j--) {
            if (j >= 2) {//&&j < cols-2) {
                //store to window
                for (int k = 0; k < 5; k++) {
                    win_r[k] = reds[i][j-2+k];
                    win_g[k] = greens[i][j-2+k];
                    win_b[k] = blues[i][j-2+k];
                }

                //apply filter
                temp_reds[i][j-2] = filter(hfilsel,win_r);
                temp_greens[i][j-2] = filter(hfilsel,win_g);
                temp_blues[i][j-2] = filter(hfilsel,win_b);
            } else {
                temp_reds[i][j-2] = reds[i][j-2];
                temp_greens[i][j-2] = greens[i][j-2];
                temp_blues[i][j-2] = blues[i][j-2];
            }
#ifdef DEBUG
            for (int k = 0; k < 5; k++) {
                cout << (int)win_r[k] << " ";
                //cout << (int)reds[i][j-2+k] << " ";
                //cout << (int)win_g[k] << " ";
                //cout << (int)win_b[k] << " ";
            }
            cout << endl;
#endif
        }
#endif
    }
}

    
//-------------------------------------------------------------------------------

int main(int args, char** cat) {
    char* FileBuffer; int BufferSize;

    #define Picture "test3.bmp"
    if (!FillAndAllocate(FileBuffer, Picture, rows, cols, BufferSize)){cout << "File read error" << endl; return 0;}
    cout << "Rows: " << rows << " Cols: " << cols << endl;

    RGB_Allocate(reds);
    RGB_Allocate(greens);
    RGB_Allocate(blues);

    RGB_Allocate(temp_reds);
    RGB_Allocate(temp_greens);
    RGB_Allocate(temp_blues);

    GetPixlesFromBMP24( reds,  greens, blues,BufferSize, rows, cols, FileBuffer);
    //ColorTest();
#ifdef IMGOUT
    haafil(0);
    WriteOutBmp24(FileBuffer, "hfilsel_0.bmp",BufferSize);
    haafil(1);
    WriteOutBmp24(FileBuffer, "hfilsel_1.bmp",BufferSize);
    haafil(2);
    WriteOutBmp24(FileBuffer, "hfilsel_2.bmp",BufferSize);
    haafil(3);
    WriteOutBmp24(FileBuffer, "hfilsel_3.bmp",BufferSize);
    haafil(4);
    WriteOutBmp24(FileBuffer, "hfilsel_4.bmp",BufferSize);
#endif
    return 1;
}
