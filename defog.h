#ifndef _DEFOG_H
#define _DEFOG_H
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
using namespace cv;
using namespace std;

class Defog
{
public:
    Defog(const Mat& src);
    ~Defog(void);

    void NewGetDefogImg(const Mat& src, Mat& dst, int block, float w, int K, double eps);

    static void GetDefogImg(const Mat& src, Mat& dst, int block, int K, double eps);

private:
    static Mat Dark(const Mat& img);
    static Mat MinDark(Mat& darkImg,int r);
    static Mat MinDark1(Mat& darkImg,int r);
    static void FindA(Mat& dark, Mat& img, int* A);
    static void FindA1(Mat& dark, const Mat& img, int* A);
    static Mat FindTx(Mat& dark, int* val);
    static Mat FindTx1(const Mat& img, int* val);
    static Mat GuidedFilter(const Mat& img, Mat& tx, int r, double eps);
    static Mat FinalMat(const Mat& img, Mat& tx, int* p, int K);
    static Mat Norm_0_255(const Mat& src);

    void NewDark(const Mat& src);
    void NewMinDark(int r);
    void NewFindA(int* val);
    void NewFindTx(int r, int* val, float w);
    void NewGuidedFilter(const Mat& img, Mat& tx, int r, double eps);
    void NewFinalMat(const Mat& src, Mat& dst, int* p, int K);

    template <typename T>
    void NewMin(const Mat& src, Mat& dst, int r);


private:
    int width,height,len;
    int resizeWidth,resizeHeight,resizeLen;
    Mat darkImg, minDarkImg;
    Mat resizeTx,Tx,resizeSrc, tempTx;
};
#endif
