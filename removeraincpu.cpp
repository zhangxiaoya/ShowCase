#include "removeraincpu.h"

RemoveRainCpu::RemoveRainCpu(const Mat& frame):
    frameCount(0), maxFrameStorage(MAX_FRAME_STORAGE)
{
    halfMaxFrameStorage = frameIndex = maxFrameStorage >> 1;
    width = frame.cols;
    height = frame.rows;
    channels = frame.channels();

    datas = new uchar**[height];
    for (auto i = 0; i < height; i++)
    {
        datas[i] = new uchar*[width];
        for (auto j = 0; j < width; ++j)
            datas[i][j] = new uchar[maxFrameStorage * channels];
    }

    SentFrameToData(frame, 0);

    avgMat = frame.clone();

    srcFrame = frame.clone();

    dstFrame = frame.clone();

    weight = new double[maxFrameStorage];

    diffValue = new uchar[maxFrameStorage];

    frameCount++;

//    for(auto i = 1; i < maxFrameStorage; ++i)
//    {
//        video >> frame;
//        cv::resize(frame, frame, cv::Size(1080, 720));
//        SentFrameToData(frame, i);
//        frameCount++;
//    }
}

bool RemoveRainCpu::NewGetRemoveRainFrame(const Mat &src, Mat &dst)
{
    if (src.empty())
        return false;
    if(frameCount < maxFrameStorage)
    {
        src.copyTo(dst);
        SentFrameToData(src, frameCount);
        frameCount++;
        return true;
    }

    auto temp = frameIndex - halfMaxFrameStorage;
    if (temp < 0)
        temp += maxFrameStorage;

    for (auto i = 0; i < height; ++i)
    {
//		auto avgPtr = avgMat.ptr<uchar>(i);
        auto framePtr = src.ptr<uchar>(i);
        auto dstPtr = dstFrame.ptr<uchar>(i);

        for (auto j = 0; j < width; ++j)
        {
            auto datasPtr = datas[i][j];

            for (auto k = 0; k < channels; ++k)
            {
                //calculate diffValue
                auto sumDiffValue = 0;
                auto avg = GetAvgValue(datasPtr, k);

                for (auto x = 0; x < maxFrameStorage; ++x)
                {
                    diffValue[x] = abs(datasPtr[channels * x + k] - avg);
                    sumDiffValue += diffValue[x];
                }

                if (sumDiffValue == 0)
                {
                    *dstPtr++ = datasPtr[channels * frameIndex + k];
                }

                else
                {
                    double sumWeight = 0;
                    double sumDst = 0;
                    for (auto x = 0; x < maxFrameStorage; ++x)
                    {
                        //calculate weight
                        weight[x] = (diffValue[x] - sumDiffValue) * (diffValue[x] - sumDiffValue);
                        //calculate dstFrame
                        sumDst += weight[x] * datasPtr[channels * x + k];
                        sumWeight += weight[x];
                    }
                    //normalize dstFrame
                    *dstPtr++ = static_cast<uchar>(sumDst / sumWeight);
                }

                //update datas
                datasPtr[temp * channels + k] = *framePtr++;

            }
        }
    }
    frameIndex = (frameIndex + 1) % maxFrameStorage;
    dst = dstFrame;
    return true;
}

void RemoveRainCpu::InitAvgMat()
{
    for (auto i = 0; i < height; ++i)
    {
        auto avgPtr = avgMat.ptr<uchar>(i);
        for (auto j = 0; j < width; ++j)
        {
            auto ptr = datas[i][j];
            for (auto k = 0; k < channels; ++k)
            {
                *avgPtr++ = GetAvgValue(ptr, k);
            }
        }
    }
}

uchar RemoveRainCpu::GetAvgValue(uchar *ptr, int index)
{
    auto avg = 0;
    for (auto i = index; i < channels * maxFrameStorage; i += channels)
    {
        avg += ptr[i];
    }
    return static_cast<uchar>(avg / maxFrameStorage);
}

void RemoveRainCpu::SentFrameToData(const Mat &frame, const int k)
{
    for (auto i = 0; i < height; ++i)
    {
        auto ptr = frame.ptr<uchar>(i);
        for (auto j = 0; j < width; ++j)
        {
            for (auto x = 0; x < channels; x++)
            {
                datas[i][j][k * channels + x] = *ptr++;
            }
        }
    }
}
