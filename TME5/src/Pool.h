#pragma once

#include "Queue.h"
#include "Job.h"
#include <vector>
#include <thread>

namespace pr {

	class Pool {
		Queue<Job> queue;
		std::vector<std::thread> threads;
	public:
		static void worker(Queue<Job> &q, int n);
		Pool(int qsize) ;
		void start (size_t nbthread);
		void submit (Job * job) ;
		void stop() ;
		~Pool() ;
	};

	Pool::Pool(int qsize):
		queue(qsize)
		{}

	void Pool::worker(Queue<Job> &q, int n)
	{
		//printf("created thread %d\n", n);
		while(true)
		{
			//printf("Process %d waiting for task\n", n);
			Job* job = q.pop();
			//printf("Process %d got task\n", n);
			if(job == nullptr) return;
			job->run();
			delete job;
		}
	}

	void Pool::start(size_t nbThread)
	{
		for(size_t i=0; i<nbThread; ++i)
		{
			threads.emplace_back(worker, std::ref(queue), i);
		}
	}

	void Pool::submit(Job* job)
	{
		queue.push(job);
	}

	void Pool::stop()
	{
		queue.setBlocking(false);
		for(std::thread& t:threads)
		{
			t.join();
		}
		threads.clear();
	}

	Pool::~Pool()
	{
		stop();
	}


}
