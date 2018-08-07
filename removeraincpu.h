#ifndef REMOVERAINCPU_H
#define REMOVERAINCPU_H
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
#define MAX_FRAME_STORAGE                5
#define MAX_DIFF_VALUE_RAIN_OR_SNOW     50

class RemoveRainCpu
{
public:
    RemoveRainCpu(const Mat& frame);
    ~RemoveRainCpu();
    bool NewGetRemoveRainFrame(const cv::Mat& srcFrame, cv::Mat& dstFrame);

private:

    int frameCount;

    int frameIndex;

    int width, height;

    int maxFrameStorage;

    int halfMaxFrameStorage;

    uchar** srcPtrs;

    cv::Mat* frames;

    uchar*** datas;
    Mat frame;

    int channels;
    Mat avgMat, srcFrame, dstFrame;

    double* weight;
    uchar* diffValue;

private:
    void InitAvgMat();
    uchar GetAvgValue(uchar* ptr, int index);
    void SentFrameToData(const Mat& frame, const int k);

};

#endif // REMOVERAINCPU_H
