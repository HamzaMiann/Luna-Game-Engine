#include "threading.h"

std::vector<iJob*> Thread::jobs = std::vector<iJob*>();
std::vector<std::function<void()>> Thread::dispatched = std::vector<std::function<void()>>();

void Thread::Update(float dt)
{
	std::vector<iJob*> toRemove;
	for (iJob* job : Thread::jobs)
	{
		if (job->IsDone())
		{
			std::function<void()> func = job->GetDispatchedFunction();
			Dispatch(func);
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

void Thread::PushJob(iJob* job)
{
	jobs.push_back(job);
}

void Thread::Dispatch(std::function<void()> func)
{
	dispatched.push_back(func);
}
