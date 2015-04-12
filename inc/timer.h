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
      start_time_.tv_usec = 0;
#else
      // unix solution
      gettimeofday(&start_time_, NULL);
#endif
    }

    // returns elapsed time in ms
    double elapsed_time() const {
      struct timeval end_time;
      long totalTime = 0;
#ifdef _MSC_VER
      // windows solution
      SYSTEMTIME st;
      GetSystemTime(&st);
      end_time.tv_sec = st.wSecond;
      end_time.tv_usec = 0;

      totalTime = (end_time.tv_sec - start_time_.tv_sec) * 1000000L;
#else
      // unix solution
      gettimeofday(&end_time, NULL);
      totalTime = (end_time.tv_sec - start_time_.tv_sec) * 1000000L;
      totalTime += (end_time.tv_usec - start_time_.tv_usec);
#endif
      return totalTime / 1000L;   // ms
    }

    void restart() {
#ifdef _MSC_VER
      // windows solution
      SYSTEMTIME st;
      GetSystemTime(&st);
      start_time_.tv_sec = st.wSecond;
      start_time_.tv_usec = 0;
#else
      // unix solution
      gettimeofday(&start_time_, NULL);
#endif
    }

private:
    struct timeval start_time_;
};