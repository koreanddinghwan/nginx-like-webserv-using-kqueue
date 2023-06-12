//사용자의 요청에 대한 처리를 쓰레드풀에 추가.
//처리할 작업을 큐에 추가한다.
//stl의 queue는 thread-safe하지 않기때문에 mutex가 필요하다.
#include <cstddef>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <queue>
#include <stdexcept>
#include <thread>

#ifndef THREAD_POOL_HPP
# define THREAD_POOL_HPP

class ThreadPool {

private:
	// num of total worker threads
	std::size_t num_threads_;

	// vector to store worker thread
	std::vector<std::thread> worker_threads_;

	//job queue
	std::queue<std::function< void() > > jobs_;

	//protect race condition
	std::condition_variable cv_job_q_;
	std::mutex m_job_q_;

	//stop worker threads
	bool stop_all;

public:
	ThreadPool(std::size_t num_threads);

	void WorkerThread();

	/*
	 * main thread에서 작업 할당시 사용할 함수.
	 * */
	void EnqueueJob(std::function<void()> job);


	~ThreadPool();
};

#endif
