#ifndef RINGBUFFER_H
#define RINGBUFFER_H
#include<condition_variable>

struct ReadRingBuffer
{
    ReadRingBuffer( int len, int count = 10):
        readIndex(0),writeIndex(0),isFinishRead(false),
        imageLen(len),buffCount(count)
    {
        data = new unsigned char[imageLen * buffCount];
    }
    ~ReadRingBuffer()
    {delete[]data;}

    unsigned char* data;
    int readIndex;
    int writeIndex;
    bool isFinishRead;
    std::condition_variable is_ready_read;
    std::condition_variable is_ready_write;
    std::mutex buffMutex;
private:
    int imageLen,buffCount;
};

struct ShowRingBuffer
{
    ShowRingBuffer( int len, int count = 10):
        readIndex(0),writeIndex(0),isFinishShow(false),
        imageLen(len),buffCount(count)
    {
        data = new unsigned char[imageLen * buffCount];
    }
    ~ShowRingBuffer()
    {delete[]data;}

    unsigned char* data;
    int readIndex;
    int writeIndex;
    bool isFinishShow;
    std::condition_variable is_ready_read;
    std::condition_variable is_ready_write;
    std::mutex buffMutex;
private:
    int imageLen,buffCount;
};

#endif // RINGBUFFER_H
