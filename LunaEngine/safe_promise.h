#pragma once

#include <future>


template <class T>
class safe_promise
{
	std::promise<T> promise;
	std::future<T>* future;

public:

	safe_promise()
	{
		future = &promise.get_future();
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
		return future->wait_for(std::chrono::duration<float>(time));
	}

	inline bool is_ready()
	{
		return future->wait_for(std::chrono::duration<float>(0.f)) == std::future_status::ready;
	}

	inline T get()
	{
		return future->get();
	}
};