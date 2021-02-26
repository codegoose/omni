#include <spdlog/spdlog.h>
#include <thread>

namespace omni::file {

	std::atomic_bool bg_continue = true;
	std::thread bg_worker;

	void bg_work() {
		while (bg_continue) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			spdlog::info("Thread");
		}
	}

	bool initialize() {
		bg_worker = std::thread(bg_work);
		return true;
	}

	void shutdown() {
		bg_continue = false;
		bg_worker.join();
	}
}