#ifndef STATS_H
#define STATS_H

#include <ctime>
#include <cstdint>
#include <mutex>

// TODO - make methods/variables static?

static std::mutex stats_mutex;

class Stats {
private:
	time_t time_start_{}, time_end_{};

public:
	bool render_complete;
	uint_fast64_t ray_int_test_count;

	Stats() : render_complete(false), ray_int_test_count(0) {}

	void start_timer() { time(&time_start_); }
	int get_time() { time(&time_end_); return int(difftime(time_end_, time_start_)); } // in seconds
	void inc_ray_int_test_count() { stats_mutex.lock(); ray_int_test_count++; stats_mutex.unlock(); }
};

#endif
