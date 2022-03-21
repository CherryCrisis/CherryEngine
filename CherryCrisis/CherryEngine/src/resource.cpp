#include "pch.hpp"

#include "resource.hpp"
#include "threadpool.hpp"

void Resource::IsLoaded(std::shared_ptr<Resource> resource, ThreadPool* threadpool)
{
	auto eventFunction = CCFunction::BindFunctionUnsafe(&Resource::OnLoaded, resource.get(), resource);
	threadpool->CreateTask(eventFunction, EChannelTask::MAINTHREAD);
}