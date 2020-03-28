#include "threading.h"

std::vector<iJob*> Thread::jobs = std::vector<iJob*>();
std::vector<std::function<void()>> Thread::dispatched = std::vector<std::function<void()>>();
std::mutex Thread::mtx;

void Thread::Update(float dt)
{
	auto_lock lock(mtx);

	std::vector<iJob*> toRemove;
	for (iJob* job : Thread::jobs)
	{
		if (job->IsDone())
		{
			std::function<void()> func = job->GetDispatchedFunction();
			_dispatch(func);
			toRemove.push_back(job);
		}
	}

	for (iJob* job : toRemove)
	{
		Thread::jobs.erase(std::find(Thread::jobs.begin(), Thread::jobs.end(), job));
	}

	for (std::function<void()> func : Thread::dispatched)
	{
		func();
	}

	Thread::dispatched.clear();
}

void Thread::_dispatch(std::function<void()> func)
{
	dispatched.push_back(func);
}

void Thread::PushJob(iJob* job)
{
	auto_lock lock(mtx);
	jobs.push_back(job);
}

void Thread::Dispatch(std::function<void()> func)
{
	auto_lock lock(mtx);
	dispatched.push_back(func);
}
