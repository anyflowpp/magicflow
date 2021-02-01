#include<stdlib.h>
#include<string.h>
#include<gtest/gtest.h>
#include<spdlog_wrap.h>
#include<context_manager.h>
#include<core/thread_pool.h>

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	setLogLevel(spdlog::level::debug);
	auto& gpoll = flow_thread_pool::GetInstance();
	RUN_ALL_TESTS();
	auto g_context = ContextManager::GetInstance().lock();
	g_context->Shutdown();
	return 0;
}