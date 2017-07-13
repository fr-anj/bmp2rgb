#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

unsigned char hfil (short hfilsel, unsigned char win[]) {
    unsigned int temp = 0;
    //coef
    unsigned int hcoef [][5] = {
        {0,0,8,0,0},
        {0,1,6,1,0},
        {0,2,4,2,0},
        {1,2,2,2,1}
    };
    //filter
    #ifdef DEBUG
    cout << "(" << hfilsel << ") ";
    #endif
    for (int n = 0; n < 5; n++) {
        temp += (hcoef[hfilsel][n] * (unsigned int)win[n]);
        #ifdef DEBUG
        cout << (hcoef[hfilsel][n] * (unsigned int)win[n]) << " ";
        #endif
    }
    #ifdef DEBUG
    cout << "= " << temp << " ";
    #endif
    temp = temp >> 3;
    return (unsigned char)temp;
}

unsigned char vfil (short vfilsel, unsigned char win[]) {
    unsigned int temp = 0;
    //coef
    unsigned int vcoef [][3] = {
        {0,8,0},
        {1,6,1},
        {2,4,2}
    };
    //filter
    #ifdef DEBUG
    cout << "(" << vfilsel << ") ";
    #endif
    for (int n = 0; n < 3; n++) {
        temp += (vcoef[vfilsel][n] * (unsigned int)win[n]);
        #ifdef DEBUG
        cout << (vcoef[vfilsel][n] * (unsigned int)win[n]) << " ";
        #endif
    }
    #ifdef DEBUG
    cout << "= " << temp << " ";
    #endif
    temp = temp >> 3;
    return (unsigned char)temp;
}

unsigned char interpolate (unsigned char cur, unsigned char prev, int cnt, int scale) {
    unsigned int temp = 0;
    temp = (prev - cur) * cnt/(scale + 1);
    return (unsigned char)temp;
}

int size_out (int size_in, int scale) {
    #ifdef DEBUG
    cout << "size= " << size_in << endl;
    cout << "out= " << floor (1 + ((size_in - 1) * (scale + 1)) / 12);
    #endif 
    return (int)floor (1 + ((size_in - 1) * (scale + 1)) / 12);
}

vector<unsigned char> hdscale (vector<unsigned char> line, short hscale, int hsize) {
    int InPixCnt = 0;
    unsigned char CurPix = 0;
    unsigned char PrevPix = 0;
    unsigned char Output_a = 0;
    vector<unsigned char> Output;
    bool skip_flag = 0;
    for (int hcnt = 0; hcnt <= hsize +2; hcnt++) {
        CurPix = line[hcnt];
        if (hcnt > 2) InPixCnt += hscale + 1; //calc InPixCnt
        if (InPixCnt < 12) skip_flag = 1; //skip
        else { 
            skip_flag = 0;
            InPixCnt -= 12;
        }
        //interpolated data
        Output_a = interpolate(CurPix,PrevPix,InPixCnt,hscale);
        if (hcnt < 2) {
            continue;
        }else if (hcnt == 2){
            Output.push_back(CurPix);
        }else if (skip_flag) {
            continue;
        }else if (InPixCnt == 0) {
            Output.push_back(CurPix);
        }else {
            Output.push_back(Output_a);
        }
        PrevPix = CurPix;
    }
    return Output;
}

vector<unsigned char> vdscale ( vector< vector<unsigned char> > image, short vscale, int hsize, int vsize) {
    int InLineCnt = 0;
    int last_line_flag = 0;
    int vcnt_out = 0;
    int vsize_out_tmp = 0;
    int vsize_out = 0;
    bool skip_flag = 0;
    bool out_en = 0;
    vector <unsigned char> CurLine;
    vector <unsigned char> PrevLine;
    vector <unsigned char> OutLine_a;
    vector <unsigned char> OutLine;

    for (int vcnt_in = 0; vcnt_in < vsize; vcnt_in++) {
        if (vcnt_in == 0) continue;
        else InLineCnt += vsize + 1;

        vsize_out_tmp = size_out (vsize,vscale); 

        if (InLineCnt < 12) skip_flag = 1; //skip
        else {
            skip_flag = 0;
            InLineCnt -= 12;
            if (vsize_out_tmp <= vcnt_out + 1) last_line_flag = 1;
        }
        for (int hcnt = 0; hcnt < hsize; hcnt++) {
            CurLine = image[vcnt_in];
            //interpolated data
            //TODO:loop to perform addition to all (-)CurLine[x] & PrevLine[x]
            //TODO: overload interpolate to accept Line
            if (vcnt_in == 0) {
                OutLine = CurLine;
                vsize_out = vcnt_out + 1;
                out_en = 1;
            } else if (skip_flag == 0) {
                OutLine = OutLine_a;
                vsize_out = vsize_out_tmp;
                out_en = 1;
            } else if (last_line_flag == 0 && vsize <= vcnt_in + 1) {
                OutLine = CurLine;
                vsize_out = vcnt_out + 1;
                out_en = 1;
            } else {
                out_en = 0;
            } 
        }
        vcnt_in++;
        if (out_en) {
            vcnt_out++;
        }
    }
}

//display funcs
#ifdef DEBUG
void disp_filt (const short coeflim, unsigned char win[]) {
    int val = 0;
    for (int n = 0; n < coeflim; n++) {
        switch (coeflim) {
            case 3:
                val = (int)vfil (n,win);
                cout << "-> " << val << endl;
                break;
            case 4:
                val = (int)hfil (n,win);
                cout << "-> " << val << endl;
                break;
            default: 
                cout << "no such thing exists.." << endl;
        }
    }
}

void disp_win (const short winlim, unsigned char win[]) {
    if (winlim==5||winlim==3) {
        for (int n = 0; n < winlim; n++) 
            cout << (int)win[n] << " ";
        cout << endl;
    } else {
        cout << "no such thing exists.." << endl;
    }
}
#endif

int main (int arg,char **ops) {
    #ifdef FILTEST
    //test
    unsigned char win5[] = {0,0,255,255,255};
    unsigned char win3[] = {0,255,255};
    cout << "hfil example:" << endl;
    disp_win (5,win5);
    disp_filt (4,win5);
    cout << "vfil example:" << endl;
    disp_win (3,win3);
    disp_filt (3,win3);
    #endif
    return 1;
}
