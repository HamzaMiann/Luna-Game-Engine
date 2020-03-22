#pragma once

#include <vector>
#include <thread>
#include <functional>

class iJob
{
public:
	virtual ~iJob() {}
	virtual bool IsDone() = 0;
	virtual std::function<void()> GetDispatchedFunction() = 0;
};


class Thread
{
private:
	static std::vector<iJob*> jobs;
	static std::vector<std::function<void()>> dispatched;

public:

	Thread() = delete;

	static void PushJob(iJob* job);
	static void Dispatch(std::function<void()> func);

	static void Update(float dt);
};