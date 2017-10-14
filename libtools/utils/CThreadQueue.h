#pragma once

#include <deque>
#include <list>
#include <condition_variable>
#include <thread>

namespace ThreadQueue
{

	template<class TMessage>
	class CThreadQueue
	{
	private:
		using TWorkerFunc = std::function<void(void)>;

		std::thread m_thread;
		std::mutex  m_mtxQueue;
		std::condition_variable m_cvQueue;

		std::deque<TMessage> m_queue;

		bool m_fNeedExit = false;
	public:
		bool GetMessage(TMessage& msg)
		{
			std::unique_lock<std::mutex> lock(m_mtxQueue);
			while (true)
			{
				if (m_fNeedExit)
				{
					return false;
				}
				if (!m_queue.empty())
				{
					break;
				}
				m_cvQueue.wait(lock);
			}

			msg = m_queue.front();
			m_queue.pop_front();

			return true;
		}
		CThreadQueue()
		{

		}
		~CThreadQueue()
		{
			StopAndWait();
			StopAndWait();
		}
		size_t CountMsg()
		{
			std::unique_lock<std::mutex> lock(m_mtxQueue);
			return m_queue.size();
		}
		void PostMsg(TMessage& msg)
		{
			std::unique_lock<std::mutex> lock(m_mtxQueue);
			m_queue.push_back(msg);
			m_cvQueue.notify_all();
		}

		TStatus StartWorker(TWorkerFunc func)
		{
			//m_func = func;
			m_thread = std::thread(func);

			RETURN_SUCCESS;
		}
		TStatus StopAndWait()
		{
			{
				std::unique_lock<std::mutex> lock(m_mtxQueue);
				m_fNeedExit = true;
				m_cvQueue.notify_all();
			}

			if(m_thread.joinable())
				m_thread.join();

			RETURN_SUCCESS;
		}
	};
}