#pragma once

#include <future>
#include <threading.h>


template <typename T>
class safe_promise : public iJob
{
	std::promise<T> promise;
	std::future<T> future;
	std::function<void()> func;

public:

	safe_promise():
		future(promise.get_future()),
		func([]() {})
	{
	}

	safe_promise(std::function<void()> function) :
		future(promise.get_future()),
		func(function)
	{
	}

	~safe_promise()
	{
	}

	inline void set_value(T value)
	{
		promise.set_value(value);
	}

	inline std::future_status wait_for(float time)
	{
		return future.wait_for(std::chrono::duration<float>(time));
	}

	inline bool is_ready()
	{
		return future.wait_for(std::chrono::duration<float>(0.f)) == std::future_status::ready;
	}

	virtual inline bool IsDone() override
	{
		return is_ready();
	}

	virtual inline std::function<void()> GetDispatchedFunction() override
	{
		return func;
	}

	inline T get()
	{
		return future.get();
	}
};