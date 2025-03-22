#pragma once

#include <deque>
#include <list>
#include <condition_variable>
#include <thread>

namespace ThreadQueue {

	template<class TMessage>
	class CThreadQueue {
	private:
		using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
		TimePoint Now() {
			return std::chrono::steady_clock::now();
		}

		using TWorkerFunc = std::function<void(void)>;
		std::thread m_thread;
		std::mutex  m_mtxQueue;
		std::condition_variable m_cvQueue;
		std::deque<TMessage> m_queue;
		std::multimap<TimePoint, TMessage> delaed_msg;

		bool m_fNeedExit = false;
	public:
		std::pair<TMessage, bool> GetMessage() {
			std::unique_lock<std::mutex> lock(m_mtxQueue);
			while (true) {
				if (m_fNeedExit) {
					return { {},false };
				}
				if (!delaed_msg.empty()) {
					auto now = Now();
					auto it = delaed_msg.begin();
					auto delaedtoStart = it->first;
					if (now >= delaedtoStart) {
						auto res = std::pair(std::move(it->second), true);
						delaed_msg.erase(it);
						return res;
					}
					if (m_queue.empty()) {
						m_cvQueue.wait_for(lock, delaedtoStart - now);
						continue;
					}
				}

				if (!m_queue.empty()) {
					auto res = std::pair(std::move(m_queue.front()), true);
					m_queue.pop_front();
					return res;
				}
				m_cvQueue.wait(lock);
			}
		}
		~CThreadQueue() {
			StopAndWait();
			StopAndWait();
		}
		size_t CountMsg() {
			std::unique_lock<std::mutex> lock(m_mtxQueue);
			return m_queue.size();
		}
		void PostMsg(TMessage&& msg, int delay = 0) {
			std::unique_lock<std::mutex> lock(m_mtxQueue);
			if (delay == 0)
				m_queue.push_back(std::move(msg));
			else {
				TimePoint to_start = Now() + std::chrono::milliseconds(delay);
				delaed_msg.emplace(to_start, std::move(msg));
			}
			m_cvQueue.notify_all();
		}


		void ReStartWorker(TWorkerFunc func) {

			StopAndWait();

			{
				std::unique_lock<std::mutex> lock(m_mtxQueue);
				m_fNeedExit = false;
				m_queue.clear();
				delaed_msg.clear();
			}

			m_thread = std::thread(func);
		}
		void StopAndWait() {

			{
				std::unique_lock<std::mutex> lock(m_mtxQueue);
				m_fNeedExit = true;
				m_cvQueue.notify_all();
			}

			if (m_thread.joinable())
				m_thread.join();
		}
	};
}