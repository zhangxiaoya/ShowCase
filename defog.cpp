#include "Defog.h"
#include<time.h>

#define RESIZE_SIZE 4

Defog::Defog(const Mat& src)
{
    width = src.cols;
    height = src.rows;
    len = width * height;
    resizeWidth = width / RESIZE_SIZE;
    resizeHeight = height / RESIZE_SIZE;
    resizeLen = len / RESIZE_SIZE;
    darkImg = Mat(resizeHeight, resizeWidth, CV_8UC1);
    minDarkImg = Mat(resizeHeight, resizeWidth, CV_8UC1);
    resizeSrc = Mat(resizeHeight, resizeWidth, CV_8UC3);
    resizeTx = Mat(resizeHeight, resizeWidth, CV_32FC1);
    Tx = Mat(height, width, CV_32FC1);
    tempTx = Mat(height, width, CV_32FC1);
}


Defog::~Defog(void)
{

}

void Defog::NewGetDefogImg(const Mat &src, Mat &dst, int block, float w, int K, double eps)
{
    resize(src, resizeSrc, Size(resizeWidth, resizeHeight));
    NewDark(resizeSrc);
    NewMinDark(block);
    int A[3] = {0};
    NewFindA(A);
    NewFindTx(block, A, w);
    NewGuidedFilter(resizeSrc, resizeTx, block, eps);
    resize(resizeTx, Tx, Size(width, height));
    NewFinalMat(src, dst, A, K);

}

void Defog::GetDefogImg(const Mat& src, Mat& dst, int block, int K, double eps)
{
    time_t t1,t2,t3,t4,t5,t6,t7,t8;
    t1= clock();
    Mat tempSrc;
    resize(src,tempSrc,Size(src.cols/4,src.rows/4));
    auto dark = Dark(tempSrc);
    t2= clock();
    auto minDark = MinDark1(dark, block);
    t3 = clock();
    int A[3] = { 0 };
    FindA1(minDark, tempSrc, A);
    t4 = clock();
    auto tx = FindTx1(tempSrc, A);
    t5 = clock();
    tx = GuidedFilter(tempSrc, tx, block, eps);
    t6 = clock();
    Mat tempTx;
    resize(tx,tempTx,Size(src.cols,src.rows));
    t7 = clock();
    dst = FinalMat(src, tempTx, A, K);

    dst=Norm_0_255(dst);
    t8 = clock();

    std::cout<<t2-t1<<"  "<<t3-t2<<"  "<<t4-t3<<"  "<<t5-t4<<"  "<<t6-t5<<"  "<<t7-t6<<"  "<<t8-t7<<std::endl;
}

struct pixel
{
    int x;
    int y;
    int data;
};

inline int temp(pixel a,pixel b)
{
    if (a.data > b.data)
        return 1;
    return 0;
}

Mat Defog::Dark(const Mat& img)
{
    auto darkImg = Mat(img.rows, img.cols, CV_8UC1);
    uchar min;
    for (auto i = 0; i < img.rows; i++)
    {
        auto pdata = darkImg.ptr<uchar>(i);
        auto data = img.ptr<uchar>(i);
        for (auto j = 0; j < img.cols; j++)
        {
            min = *data;
            data++;
            min = MIN(min, *data);
            data++;
            min = MIN(min, *data);
            data++;
            *pdata++ = min;
        }
    }
    return darkImg;
}


Mat Defog::MinDark(Mat& darkImg, int r)
{
    Rect ROIrect;
    auto dark = darkImg.clone();
    Mat ROIimg;
    //ROIimg=Mat(block,block,CV_8UC1);
    double xmin = 0,xmax = 0;
    for (auto u = 0; u < darkImg.rows; u++)
    {
        auto data = dark.ptr<uchar>(u);
        for(auto v = 0; v < darkImg.cols; v++)
        {
            auto rMinRow = MAX(0, u - r);
            auto rMaxRow = MIN(darkImg.rows - 1, u + r);
            auto cMinCol = MAX(0, v - r);
            auto cMaxCol = MIN(darkImg.cols - 1, v + r);
            ROIrect.x = cMinCol;
            ROIrect.y = rMinRow;
            ROIrect.width = cMaxCol - cMinCol;
            ROIrect.height = rMaxRow - rMinRow;
            ROIimg = darkImg(ROIrect);
            minMaxIdx(ROIimg, &xmin, &xmax);
            data[v] = xmin;
        }
    }
    return dark;
}

Mat Defog::MinDark1(Mat& darkImg, int r)
{
    auto rs = darkImg.rows, cs = darkImg.cols;
    Mat dark(rs, cs, CV_8UC1);
    auto block = 2 * r + 1;
    for (auto i = 0; i < rs; i++)
    {
        uchar min = 255;
        auto outData = dark.ptr<uchar>(i);
        auto inData = darkImg.ptr<uchar>(i);
        for (auto j = 0; j < r; j++)
        {
            min = MIN(min, inData[j]);
        }
        *outData = min;
    }
    for (auto i = 0; i < rs; i++)
    {
        auto inData = darkImg.ptr<uchar>(i);
        auto outData = dark.ptr<uchar>(i);
        for (auto j = 1; j <= r; j++)
        {
            outData[j] = MIN(outData[j - 1], inData[j + r]);
        }
    }
    for (auto i = 0; i < rs; i++)
    {
        auto inData = darkImg.ptr<uchar>(i);
        auto outData = dark.ptr<uchar>(i);
        for (auto j = r + 1; j < cs - r; j++)
        {
            if (outData[j - 1] >= inData[j + r])
                outData[j] = inData[j + r];
            else
            {
                if (inData[j - r - 1] > outData[j - 1])
                    outData[j] = outData[j - 1];
                else
                {
                    auto min = 255;
                    for (auto k = 0; k < block; k++)
                    {
                        min = MIN(min, inData[j - r + k]);
                    }
                    outData[j] = min;
                }
            }
        }
    }
    for (auto i = 0; i < rs; i++)
    {
        auto outData = dark.ptr<uchar>(i);
        auto inData = darkImg.ptr<uchar>(i);
        for (auto j = cs - r; j < cs; j++)
        {
            if (outData[j - 1] < inData[j - 1 - r])
                outData[j] = outData[j - 1];
            else
            {
                auto min = 255;
                for (auto k = j - r; k < cs; k++)
                {
                    min = MIN(min, inData[k]);
                }
                outData[j] = min;
            }
        }
    }

    for (auto j = 0; j < cs; j++)
    {
        auto outData = darkImg.ptr<uchar>(0);
        auto inData = dark.ptr<uchar>(0);
        auto min = 255;
        for (auto i = 0; i < r; i++)
        {
            min = MIN(min, *(inData + j + cs * i));
        }
        outData[j]=min;
    }
    for (auto j = 0; j < cs; j++)
    {
        for (auto i = 1; i <= r; i++)
        {
            auto inData = dark.ptr<uchar>(i);
            auto outData = darkImg.ptr<uchar>(i);
            outData[j] = MIN(*(outData + j - cs), *(inData + j + r * cs));
        }
    }
    for (auto j = 0; j < cs; j++)
    {
        for (auto i = r + 1; i < rs - r; i++)
        {
            auto inData = dark.ptr<uchar>(i);
            auto outData = darkImg.ptr<uchar>(i);
            if (*(outData + j - cs) >= *(inData + j + r * cs))
                outData[j] = *(inData + j + r * cs);
            else
            {
                if (*(inData + j - cs * (r + 1)) > *(outData + j - cs))
                    outData[j] = *(outData + j - cs);
                else
                {
                    auto min = 255;
                    for (auto k = 0; k < block; k++)
                    {
                        min = MIN(min, *(inData + j + cs * (k - r)));
                    }
                    outData[j] = min;
                }
            }
        }
    }
    for (auto j = 0; j < cs; j++)
    {

        for (auto i = rs - r; i < rs; i++)
        {
            auto outData = darkImg.ptr<uchar>(i);
            auto inData = dark.ptr<uchar>(i);
            if (*(outData + j - cs) < *(inData + j - cs * (1 + r)))
                outData[j] = *(outData + j - cs);
            else
            {
                auto min=255;
                for (auto k=i-r;k<rs;k++)
                {
                    min = MIN(min, *(inData + j - (i - k) * cs));
                }
                outData[j] = min;
            }
        }
    }
    return darkImg;
}


void Defog::FindA(Mat& dark,Mat& img, int* p)
{
    pixel* allPixel;
    auto allData = dark.rows * dark.cols;
    auto maxData = static_cast<int>(allData) / 1000;
    allPixel = new pixel[allData];
    for(auto i=0;i<dark.rows;i++)
    {
        auto pData = dark.ptr<uchar>(i);
        for (auto j = 0; j < dark.cols; j++)
        {
            allPixel[i * dark.rows + j].data = *pData++;
            allPixel[i * dark.rows + j].x = i;
            allPixel[i * dark.rows + j].y = j;
        }
    }
    sort(allPixel, allPixel + allData, temp);
    for (auto i = 0; i < maxData; i++)
    {
        auto x = allPixel[i].x;
        auto y = allPixel[i].y;
        auto outData = img.ptr<uchar>(x);
        outData += 3 * y;
        //使用最大均值计算
        p[0] += *outData++;
        p[1] += *outData++;
        p[2] += *outData;
        //最亮点计算..................
        //val1=*outData++;
        //val2=*outData++;
        //val3=*outData;
        //avg=(val3+val2+val1)/3;
        //if (max<avg)
        //{
        //	max=avg;
        //	maxX=allPixel[i].x;
        //	maxY=allPixel[i].y;
        //}
        //............................
    }
    p[0] = p[0] / maxData;
    p[1] = p[1] / maxData;
    p[2] = p[2] / maxData;
    //...................................
    /*p[0]=img.at<Vec3b>(maxX,maxY)[0];
    p[1]=img.at<Vec3b>(maxX,maxY)[1];
    p[2]=img.at<Vec3b>(maxX,maxY)[2];*/
    //....................................
    p[0] = MIN(220, p[0]);
    p[1] = MIN(220, p[1]);
    p[2] = MIN(220, p[2]);
    delete[] allPixel;

}


void Defog::FindA1(Mat &dark, const Mat &img, int* pA)
{
    auto maxData = dark.rows * dark.cols / 80;

    int p[256] = { 0 };
    auto t = 0, mData = 0, sum = 0;
    for (auto i = 0; i < dark.rows; ++i)
    {
        auto inData = dark.ptr<uchar>(i);
        for (auto j = 0; j < dark.cols; ++j)
        {
            p[inData[j]]++;
        }
    }
    for (auto i = 255; i >= 0; --i)
    {
        sum += p[i];
        if (sum >= maxData)
        {
            mData = i;
            break;
        }
    }

    for(auto i = 0; i < img.rows; ++i)
    {
        auto imgData = img.ptr<uchar>(i);
        auto darkData = dark.ptr<uchar>(i);
        for (auto j = 0; j < img.cols; ++j)
        {
            if (darkData[j] > mData)
            {
                pA[0] += imgData[3 * j];
                pA[1] += imgData[3 * j + 1];
                pA[2] += imgData[3 * j + 2];
                t++;
            }
        }
    }

    pA[0] = pA[0] / t;
    pA[1] = pA[1] / t;
    pA[2] = pA[2] / t;
    pA[0] = MIN(220, pA[0]);
    pA[1] = MIN(220, pA[1]);
    pA[2] = MIN(220, pA[2]);

}


Mat Defog::FindTx(Mat& dark,int* val)
{
    int avg;
    avg = (val[0] + val[1] + val[2]) / 3;
    auto tx = Mat(dark.rows, dark.cols, CV_32FC1);
    float w = 0.95;
    for (auto i = 0;i <dark.rows ; i++)
    {
        auto pData = dark.ptr<uchar>(i);
        auto tx_data = tx.ptr<float>(i);
        for (auto j = 0; j < dark.cols; j++)
        {
            tx_data[j] = 1 - w * pData[j] / avg;
        }
    }
    return tx;
}


Mat Defog::FindTx1(const Mat& img, int* val)
{
    auto r = 30;
    auto tx = Mat(img.rows, img.cols, CV_32FC1);

    auto darkImg = Mat(img.rows, img.cols, CV_32FC1);

    float min;
    for (auto i = 0; i < img.rows; i++)
    {
        auto pdata = darkImg.ptr<float>(i);
        auto data = img.ptr<uchar>(i);
        for (auto j = 0; j < img.cols; j++)
        {
            min = static_cast<float>(*data) / val[0];
            data++;
            min = MIN(min, float(*data) / val[1]);
            data++;
            min = MIN(min, float(*data) / val[2]);
            data++;
            //pdata[j]=min;
            *pdata++ = min;
        }
    }

    auto rs = darkImg.rows, cs = darkImg.cols;
    Mat dark(rs, cs, CV_32FC1);
    auto block = 2 * r + 1;
    for (auto i = 0; i < rs; i++)
    {
        min = 255;
        auto outData = dark.ptr<float>(i);
        auto inData = darkImg.ptr<float>(i);
        for (auto j = 0; j < r; j++)
        {
            min = MIN(min, inData[j]);
        }
        *outData = min;
    }
    for (auto i = 0; i < rs; i++)
    {
        auto inData = darkImg.ptr<float>(i);
        auto outData = dark.ptr<float>(i);
        for (auto j = 1; j <= r; j++)
        {
            outData[j] = MIN(outData[j - 1], inData[j + r]);
        }
    }
    for (auto i = 0; i < rs; i++)
    {
        auto inData = darkImg.ptr<float>(i);
        auto outData = dark.ptr<float>(i);
        for (auto j = r + 1; j < cs - r; j++)
        {
            if (outData[j - 1] >= inData[j + r])
                outData[j] = inData[j + r];
            else
            {
                if (inData[j - r - 1] > outData[j - 1])
                    outData[j] = outData[j - 1];
                else
                {
                    min = 255;
                    for (auto k = 0; k < block; k++)
                    {
                        min = MIN(min, inData[j - r + k]);
                    }
                    outData[j] = min;
                }
            }
        }
    }
    for (auto i = 0; i < rs; i++)
    {
        auto outData = dark.ptr<float>(i);
        auto inData = darkImg.ptr<float>(i);
        for (auto j = cs - r; j < cs; j++)
        {
            if (outData[j - 1] < inData[j - 1 - r])
                outData[j] = outData[j - 1];
            else
            {
                min = 255;
                for (auto k = j - r; k < cs; k++)
                {
                    min = MIN(min, inData[k]);
                }
                outData[j] = min;
            }
        }
    }

    for (auto j = 0; j < cs; j++)
    {
        auto outData = darkImg.ptr<float>(0);
        auto inData = dark.ptr<float>(0);
        min = 255;
        for (auto i = 0; i < r; i++)
        {
            min = MIN(min, *(inData + j + cs * i));
        }
        outData[j] = min;
    }
    for (auto j = 0; j < cs; j++)
    {
        for (auto i = 1; i <= r; i++)
        {
            auto inData = dark.ptr<float>(i);
            auto outData = darkImg.ptr<float>(i);
            outData[j] = MIN(*(outData + j - cs), *(inData + j + r * cs));
        }
    }
    for (auto j = 0; j < cs; j++)
    {
        for (auto i = r + 1; i < rs - r; i++)
        {
            auto inData = dark.ptr<float>(i);
            auto outData = darkImg.ptr<float>(i);
            if (*(outData + j - cs) >= *(inData + j + r * cs))
                outData[j] = *(inData + j + r * cs);
            else
            {
                if (*(inData + j - cs*(r + 1)) > *(outData + j - cs))
                    outData[j] = *(outData + j - cs);
                else
                {
                    min = 255;
                    for (auto k = 0; k < block; k++)
                    {
                        min = MIN(min, *(inData + j + cs * (k - r)));
                    }
                    outData[j] = min;
                }
            }
        }
    }
    for (auto j = 0; j < cs; j++)
    {

        for (auto i = rs - r; i < rs; i++)
        {
            auto outData = darkImg.ptr<float>(i);
            auto inData = dark.ptr<float>(i);
            if (*(outData + j - cs) < *(inData + j - cs * (1 + r)))
                outData[j] = *(outData + j - cs);
            else
            {
                min = 255;
                for (auto k = i - r; k < rs; k++)
                {
                    min = MIN(min, *(inData + j - (i - k) * cs));
                }
                outData[j] = min;
            }
        }
    }


    float w = 0.95;
    for (auto i = 0; i < dark.rows; i++)
    {
        auto pData = dark.ptr<float>(i);
        auto tx_data = tx.ptr<float>(i);
        for (auto j = 0; j < dark.cols; j++)
        {
            tx_data[j] = 1 - w * pData[j];
        }
    }
    return tx;
}


Mat Defog::GuidedFilter(const Mat& img, Mat& tx, int r, double eps)
{
    Mat I, boxI, boxIT, boxT, II, boxII, IT, a, b, boxA, boxB;
    Size si(r * 6,r * 6);
    cvtColor(img, I, CV_BGR2GRAY);
    I.convertTo(I, CV_32FC1);
    IT = I.mul(tx);
    II = I.mul(I);
    boxFilter(II, boxII, CV_32FC1, si);
    boxFilter(I, boxI, CV_32FC1, si);
    boxFilter(tx, boxT, CV_32FC1, si);
    boxFilter(IT, boxIT, CV_32FC1, si);
    Mat cov_IT = boxIT - boxI.mul(boxT);
    a = cov_IT / (boxII - boxI.mul(boxI) + eps);
    b = boxT - a.mul(boxI);
    boxFilter(a, boxA, CV_32FC1, si);
    boxFilter(b, boxB, CV_32FC1, si);
    tx = boxA.mul(I) + boxB;

    return tx;
}

Mat Defog::FinalMat(const Mat& img,Mat& tx,int* p,int K)
{
    Mat finImg(tx.rows, tx.cols, CV_32FC3);


    float a;
    for (auto i = 0;i < tx.rows; i++)
    {
        auto outData = finImg.ptr<float>(i);
        auto inData = img.ptr<uchar>(i);
        auto txData = tx.ptr<float>(i);
        for (auto j = 0; j < tx.cols; j++)
        {
            a = MAX(abs(*inData - p[0]), abs(*(inData + 1) - p[1]));
            a = MAX(a, abs(*(inData + 2) - p[2]));
            a = K / a;
            a = MAX(a, 1);
            auto temp = MIN(a * MAX(0.1, txData[j]), 1);
            *outData++ = (*inData - p[0]) / temp + p[0];
            inData++;

            *outData++ = (*inData - p[1]) / temp + p[1];
            inData++;

            *outData++ = (*inData - p[2]) / temp + p[2];
            inData++;
        }
    }

    finImg.convertTo(finImg,CV_8UC3);
    return finImg;
}


Mat Defog::Norm_0_255(const Mat& src)
{
    // Create and return normalized image:
    Mat dst;
    switch (src.channels())
    {
    case 1:
        cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
        break;
    case 3:
        cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        break;
    default:
        src.copyTo(dst);
        break;
    }
    return dst;
}

void Defog::NewDark(const Mat &src)
{
    for(auto i = 0; i < resizeHeight; ++i)
    {
        auto darkPtr = darkImg.ptr<uchar>(i);
        auto ptr = src.ptr<uchar>(i);
        for(auto j = 0; j < resizeWidth; ++j)
        {
            auto min = *ptr++;
            min = __min(min, *ptr);
            ++ptr;
            *darkPtr++ = __min(min, *ptr);
            ++ptr;
        }
    }
}

void Defog::NewMinDark(int r)
{
    NewMin<uchar>(darkImg, minDarkImg, r);
}

void Defog::NewFindA(int *val)
{
    int hist[256] = { 0 };

    auto maxACount = resizeLen / 80;
    for(auto i = 0; i < resizeHeight; ++i)
    {
        auto ptr = minDarkImg.ptr<uchar>(i);
        for(auto j = 0; j < resizeWidth; ++j)
            hist[ptr[j]]++;
    }

    auto sum = 0;
    uchar indexA = 0;
    for (auto i = 255; i >= 0; --i)
    {
        sum += hist[i];
        if(sum >= maxACount)
        {
            indexA = i;
            break;
        }
    }
    auto count = 0;
    for (auto i = 0; i < resizeHeight; ++i)
    {
        auto ptr = resizeSrc.ptr<uchar>(i);
        auto darkPtr = minDarkImg.ptr<uchar>(i);
        for (auto j = 0; j < resizeWidth; ++j)
        {
            if (darkPtr[j] > indexA)
            {
                val[0] += *ptr;
                val[1] += *(ptr + 1);
                val[2] += *(ptr + 2);
                ++count;
            }
            ptr += 3;
        }
    }

    val[0] /= count;
    val[1] /= count;
    val[2] /= count;

    val[0] = __min(220, val[0]);
    val[1] = __min(220, val[1]);
    val[2] = __min(220, val[2]);
}

void Defog::NewFindTx(int r,int *val, float w)
{
    for (auto i = 0; i < resizeHeight; ++i)
    {
        auto srcPtr = resizeSrc.ptr<uchar>(i);
        auto tempTxPtr = tempTx.ptr<float>(i);
        for(auto j = 0; j < resizeWidth; ++j)
        {
            auto min = 1.0f * (*srcPtr) / val[0];
            ++srcPtr;
            min = __min(min, 1.0f * (*srcPtr) / val[1]);
            ++srcPtr;
            *tempTxPtr++ = __min(min, 1.0f * (*srcPtr) / val[2]);
            ++srcPtr;
        }
    }

    NewMin<float>(tempTx, resizeTx, r);

        for (auto i = 0; i < resizeHeight; ++i)
        {
            auto ptr = resizeTx.ptr<float>(i);
            for(auto j = 0; j < resizeWidth; ++j)
                ptr[j] = 1.0f - w * ptr[j];
        }
}

void Defog::NewGuidedFilter(const Mat &img, Mat &tx, int r, double eps)
{
    Mat I, boxI, boxIT, boxT, II, boxII, IT, a, b, boxA, boxB;
    Size si(r * 6,r * 6);
    cvtColor(img, I, CV_BGR2GRAY);
    I.convertTo(I, CV_32FC1);
    IT = I.mul(tx);
    II = I.mul(I);
    boxFilter(II, boxII, CV_32FC1, si);
    boxFilter(I, boxI, CV_32FC1, si);
    boxFilter(tx, boxT, CV_32FC1, si);
    boxFilter(IT, boxIT, CV_32FC1, si);
    Mat cov_IT = boxIT - boxI.mul(boxT);
    a = cov_IT / (boxII - boxI.mul(boxI) + eps);
    b = boxT - a.mul(boxI);
    boxFilter(a, boxA, CV_32FC1, si);
    boxFilter(b, boxB, CV_32FC1, si);
    tx = boxA.mul(I) + boxB;
}

void Defog::NewFinalMat(const Mat& src, Mat& dst, int *p, int K)
{
    float a = 0;
    for(auto i = 0; i < height; ++i)
    {
        auto srcPtr = src.ptr<uchar>(i);
        auto dstPtr =dst.ptr<uchar>(i);
        auto txPtr = Tx.ptr<uchar>(i);
        for (auto j = 0; j < width; ++j)
        {
            a = MAX(abs(*srcPtr - p[0]), abs(*(srcPtr + 1) - p[1]));
            a = MAX(a, abs(*(srcPtr + 2) - p[2]));
            a = K / a;
            a = MAX(a, 1);
            *dstPtr++ = (*srcPtr - p[0]) / MIN((a * MAX(0.1, txPtr[j])), 1) + p[0];
            srcPtr++;

            *dstPtr++ = (*srcPtr - p[1]) / MIN((a * MAX(0.1, txPtr[j])), 1) + p[1];
            srcPtr++;

            *dstPtr++ = (*srcPtr - p[2]) / MIN((a * MAX(0.1, txPtr[j])), 1) + p[2];
            srcPtr++;
        }
    }
}

template<typename T>
void Defog::NewMin(const Mat &src, Mat &dst, int r)
{

    auto tempMin = src.clone();
    auto block = 2 * r + 1;
    for (auto i = 0; i < resizeHeight; ++i)
    {
        auto outData = tempMin.ptr<T>(i);
        auto inData = darkImg.ptr<T>(i);
        auto min = inData[0];
        for (auto j = 1; j < r; ++j)
        {
            min = MIN(min, inData[j]);
        }
        *outData = min;
    }
    for (auto i = 0; i < resizeHeight; ++i)
    {
        auto inData = darkImg.ptr<T>(i);
        auto outData = tempMin.ptr<T>(i);
        for (auto j = 1; j <= r; ++j)
        {
            outData[j] = MIN(outData[j - 1], inData[j + r]);
        }
    }
    for (auto i = 0; i < resizeHeight; ++i)
    {
        auto inData = darkImg.ptr<T>(i);
        auto outData = tempMin.ptr<T>(i);
        for (auto j = r + 1; j < resizeWidth - r; ++j)
        {
            if (outData[j - 1] >= inData[j + r])
                outData[j] = inData[j + r];
            else
            {
                if (inData[j - r - 1] > outData[j - 1])
                    outData[j] = outData[j - 1];
                else
                {
                    auto min = inData[j - r];
                    for (auto k = 1; k < block; ++k)
                    {
                        min = MIN(min, inData[j - r + k]);
                    }
                    outData[j] = min;
                }
            }
        }
    }
    for (auto i = 0; i < resizeHeight; ++i)
    {
        auto outData = tempMin.ptr<T>(i);
        auto inData = darkImg.ptr<T>(i);
        for (auto j = resizeWidth - r; j < resizeWidth; ++j)
        {
            if (outData[j - 1] < inData[j - 1 - r])
                outData[j] = outData[j - 1];
            else
            {
                auto min = inData[j - r];
                for (auto k = j - r + 1; k < resizeWidth; k++)
                {
                    min = MIN(min, inData[k]);
                }
                outData[j] = min;
            }
        }
    }

    for (auto j = 0; j < resizeWidth; ++j)
    {
        auto outData = darkImg.ptr<T>(0);
        auto inData = tempMin.ptr<T>(0);
        auto min = *(inData + j);
        for (auto i = 0; i < r; ++i)
        {
            min = MIN(min, *(inData + j + resizeWidth * i));
        }
        outData[j]=min;
    }
    for (auto j = 0; j < resizeWidth; ++j)
    {
        for (auto i = 1; i <= r; ++i)
        {
            auto inData = tempMin.ptr<T>(i);
            auto outData = darkImg.ptr<T>(i);
            outData[j] = MIN(*(outData + j - resizeWidth), *(inData + j + r * resizeWidth));
        }
    }
    for (auto j = 0; j < resizeWidth; ++j)
    {
        for (auto i = r + 1; i < resizeHeight - r; ++i)
        {
            auto inData = tempMin.ptr<T>(i);
            auto outData = darkImg.ptr<T>(i);
            if (*(outData + j - resizeWidth) >= *(inData + j + r * resizeWidth))
                outData[j] = *(inData + j + r * resizeWidth);
            else
            {
                if (*(inData + j - resizeWidth * (r + 1)) > *(outData + j - resizeWidth))
                    outData[j] = *(outData + j - resizeWidth);
                else
                {
                    auto min = *(inData + j);
                    for (auto k = 1; k < block; ++k)
                    {
                        min = MIN(min, *(inData + j + resizeWidth * (k - r)));
                    }
                    outData[j] = min;
                }
            }
        }
    }
    for (auto j = 0; j < resizeWidth; ++j)
    {

        for (auto i = resizeHeight - r; i < resizeHeight; ++i)
        {
            auto outData = darkImg.ptr<T>(i);
            auto inData = tempMin.ptr<T>(i);
            if (*(outData + j - resizeWidth) < *(inData + j - resizeWidth * (1 + r)))
                outData[j] = *(outData + j - resizeWidth);
            else
            {
                auto min = *(inData + j + resizeWidth * (i - r));
                for (auto k = i - r + 1; k < resizeHeight; ++k)
                {
                    min = MIN(min, *(inData + j - (i - k) * resizeWidth));
                }
                outData[j] = min;
            }
        }
    }
}
