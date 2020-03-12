#pragma once

#include <safe_promise.h>

class iAction
{
public:
	virtual ~iAction() {}
	virtual void execute() = 0;
};


class thread_manager
{
	struct thread_item
	{
		
	};

	thread_manager() {}

public:
	~thread_manager() {}

	static thread_manager& Instance()
	{
		static thread_manager instance;
		return instance;
	}

	template <typename T>
	void RunThreaded()
	{

	}

	void update();
};