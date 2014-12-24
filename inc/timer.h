#pragma once

#include <sys/time.h>

class Timer {
public:
    Timer() {
        gettimeofday(&start_time_, NULL);
    }

    // returns elapsed time in ms
    double elapsed_time() const {
        struct timeval end_time;
        gettimeofday(&end_time, NULL);
        long totalTime =  (end_time.tv_sec - start_time_.tv_sec) * 1000000L;
        totalTime += (end_time.tv_usec - start_time_.tv_usec);
        return totalTime / 1000L;   // ms
    }

    void restart() {
        gettimeofday(&start_time_, NULL);
    }

private:
    struct timeval start_time_;
};