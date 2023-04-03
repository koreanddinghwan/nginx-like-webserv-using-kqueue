//사용자의 요청에 대한 처리를 쓰레드풀에 추가.
//처리할 작업을 큐에 추가한다.
//stl의 queue는 thread-safe하지 않기때문에 mutex가 필요하다.

#include "./threadpool.hpp"

ThreadPool::ThreadPool(std::size_t num_threads): num_threads_(num_threads), stop_all(false) 
{
	worker_threads_.reserve(num_threads_);
	for (std::size_t i = 0; i < num_threads_; i++)
	{
		//각 쓰레드는 worker thread에 정의된 WorkerThread 함수를 수행, 
		//람다함수에서 멤버함수에 접근하기 위해선 this를 전달
		//람다함수(익명함수): [](매개변수) {함수동작} (호출시 인자)
		//[]: 캡쳐
		//(): 매개변수 선언부
		//{}: 함수동작부
		//():함수 호출시 인자
		//람다함수에는 this가 없어 멤버변수가 없으므로, this를 캡쳐에 넣으면 this를 bind한다.
		worker_threads_.emplace_back([this]() {this->WorkerThread();});
	}
}

void ThreadPool::WorkerThread() {
	while (true) {
		/*----- critical section-----*/
		//job queue lock
		//job queue에 작업을 main thread에서 넣기때문에 임계영역 처리.
		std::unique_lock<std::mutex> lock(m_job_q_);

		//condition variable에 notify가 될때까지, 즉 jobs_에 작업이 추가될때까지 wait
		//main thread에서 notify를 할거임
		cv_job_q_.wait(lock, [this]() { return !this->jobs_.empty() || stop_all;});
		if (stop_all && this->jobs_.empty()) {
			return;
		}


		//맨 앞 job pop
		std::function<void()> job = std::move(jobs_.front());
		jobs_.pop();

		//lock 해제.
		lock.unlock();

		/*----- critical section-----*/

		//job 수행
		job();
	}
}

/*
 * public
 * main thread에서 작업 할당시 사용할 함수.
 * */
void ThreadPool::EnqueueJob(std::function<void()> job) {
	if (stop_all) {
		throw std::runtime_error("ThreadPool end");
	}
	//lock_guard위한 블럭
	{
		/*-----critical section-----*/
		std::lock_guard<std::mutex> lock(m_job_q_);
		jobs_.push(std::move(job));
		/*-----critical section-----*/
	}	
	//condition variable에 notify해서 작업을 알림
	cv_job_q_.notify_one();
}

ThreadPool::~ThreadPool() {
	stop_all = true;
	cv_job_q_.notify_all();

	for (auto& t : worker_threads_) {
		t.join();
	}
}

