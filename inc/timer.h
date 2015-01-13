#pragma once

#ifdef _MSC_VER
#include <Windows.h>
#include <time.h>
#else
#include <sys/time.h>
#endif


class Timer {
public:
    Timer() {
#ifdef _MSC_VER
      // windows solution
      SYSTEMTIME st;
      GetSystemTime(&st);
      start_time_.tv_sec = st.wSecond;
      start_time_.tv_usec = st.wMilliseconds;
#else
      // unix solution
      gettimeofday(&start_time_, NULL);
#endif
    }

    // returns elapsed time in ms
    double elapsed_time() const {
      struct timeval end_time;

#ifdef _MSC_VER
      // windows solution
      SYSTEMTIME st;
      GetSystemTime(&st);
      end_time.tv_sec = st.wSecond;
      end_time.tv_usec = st.wMilliseconds;
#else
      // unix solution
      gettimeofday(&end_time, NULL);
#endif
      long totalTime = (end_time.tv_sec - start_time_.tv_sec) * 1000000L;
      totalTime += (end_time.tv_usec - start_time_.tv_usec);
      return totalTime / 1000L;   // ms
    }

    void restart() {
#ifdef _MSC_VER
      // windows solution
      SYSTEMTIME st;
      GetSystemTime(&st);
#else
      // unix solution
      gettimeofday(&start_time_, NULL);
#endif
    }

private:
    struct timeval start_time_;
};