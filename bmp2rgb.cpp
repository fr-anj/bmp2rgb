#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>

using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::string;

#pragma pack(1)

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

int main (int argc,char* argv[]) {
    unsigned char* datBuff[2] = {NULL,NULL};//Header buffers
    unsigned char* pixels = NULL;

    //unsigned char* red = NULL;
    //unsigned char* green = NULL;
    //unsigned char* blue = NULL;

    int padding = 0;

    PBITMAPFILEHEADER bmpheader = NULL; //header
    PBITMAPINFOHEADER bmpInfo = NULL; //info

    const char* location = argv[1]; 
    string strlocation(location);
    string fr, fg, fb;
    strlocation.erase(strlocation.length()-4,4);
    cout << "new location: " << strlocation << endl;
    fr = strlocation + "_r.dat";
    fg = strlocation + "_g.dat";
    fb = strlocation + "_b.dat";

    //open file
    ifstream file(location, std::ios::binary);
    if(!file){
        cout << "Failure to open bitmap file.\n";
        return 1;
    }

    datBuff[0] = new unsigned char[sizeof(BITMAPFILEHEADER)];
    datBuff[1] = new unsigned char[sizeof(BITMAPINFOHEADER)];

    file.read((char*)datBuff[0], sizeof(BITMAPFILEHEADER));
    file.read((char*)datBuff[1], sizeof(BITMAPINFOHEADER));

    bmpheader = (PBITMAPFILEHEADER) datBuff[0];
    bmpInfo = (PBITMAPINFOHEADER) datBuff[1];

    //check if file is BMP
    if(bmpheader->bfType != 0x4D42) {
        cout << "File \"" << location << "\" is not a bitmap file\n";
        return 2;
    }

    pixels = new unsigned char[bmpInfo->biSizeImage];

    file.seekg(bmpheader->bfOffBits);
    file.read((char*)pixels, bmpInfo->biSizeImage);

    //red = new unsigned char[bmpInfo->biWidth];
    //green = new unsigned char[bmpInfo->biWidth];
    //blue = new unsigned char[bmpInfo->biWidth];

    padding = bmpInfo->biWidth % 4;

    int ctr = 0;
    //int newctr = 0;
    cout << "size: " << bmpInfo->biSizeImage << endl;
    cout << "width: " << bmpInfo->biWidth << endl;
    cout << "height: " << bmpInfo->biHeight << endl;
    cout << "padding: " << padding << endl;

    //--------------------------------------------RED
    const char* cfr = fr.c_str();
    ofstream red(cfr);
    if (!red) {
        cout << "Failed to create file " << fr << endl;
        return 1;
    }
    cout << "creating file " << fr << endl;

    //--------------------------------------------GREEN
    const char* cfg = fg.c_str();
    ofstream green(cfg);
    if (!green) {
        cout << "Failed to create file " << fg << endl;
        return 1;
    }
    cout << "creating file " << fg << endl;

    //--------------------------------------------BLUE
    const char* cfb = fb.c_str();
    ofstream blue(cfb);
    if (!blue) {
        cout << "Failed to create file " << fb << endl;
        return 1;
    }
    cout << "creating file " << fb << endl;

    for (unsigned long i = 0; i < bmpInfo->biSizeImage; i+= 3) {
        //cout << ctr << " ";
        if (ctr == bmpInfo->biWidth) {
            i+= padding;
            //cout << endl;
            ctr = 0;
        }
        ctr++;
        red << std::showbase << std::hex << (unsigned int)pixels[i+2] << endl;
        green << std::showbase << std::hex << (unsigned int)pixels[i+1] << endl;
        blue << std::showbase << std::hex << (unsigned int)pixels[i] << endl;
    }

    //close files
    red.close();
    green.close();
    blue.close();
    return 1;
}

