#pragma once

#include <deque>
#include <queue>
#include <condition_variable>


template<class TMessage> class CMultiThreadQueue {
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
	TimePoint Now() { return std::chrono::steady_clock::now(); }
	using TWorkerFunc = std::function<void(void)>;
	std::mutex  m_mtxQueue;
	std::condition_variable m_cvQueue;
	std::deque<TMessage> m_queue;
	using TElem = std::pair<TimePoint, TMessage>;
	std::priority_queue<TElem, std::vector<TElem>, decltype([](const auto& s1, const auto& s2) {return s1.first > s2.first; }) > m_delayed;
	bool m_fNeedExit = false;
public:
	std::pair<TMessage, bool> GetMessage() {
		std::unique_lock<std::mutex> lock(m_mtxQueue);
		while (true) {
			if (m_fNeedExit) {
				return { {},false };
			}
			if (!m_delayed.empty()) {
				auto now = Now();
				auto& it = m_delayed.top();
				auto delaedtoStart = it.first;
				if (now >= delaedtoStart) {
					auto res = std::pair(std::move(it.second), true);
					m_delayed.pop();
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
	size_t CountMsg() {
		std::unique_lock<std::mutex> lock(m_mtxQueue);
		return m_queue.size() + m_delayed.size();
	}
	void PostMsg(TMessage&& msg, int delay = 0) {
		std::unique_lock<std::mutex> lock(m_mtxQueue);
		if (delay == 0)
			m_queue.push_back(std::move(msg));
		else {
			m_delayed.emplace(Now() + std::chrono::milliseconds(delay), std::move(msg));
		}
		m_cvQueue.notify_all();
	}
	void Reinitialize() {
		std::unique_lock<std::mutex> lock(m_mtxQueue);
		m_fNeedExit = false;
		m_queue.clear();
		m_delayed = {};
	}
	void QuitRequest() {
		std::unique_lock<std::mutex> lock(m_mtxQueue);
		m_fNeedExit = true;
		m_cvQueue.notify_all();
	}

};
