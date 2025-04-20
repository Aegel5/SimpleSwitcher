#pragma once

#include <deque>
#include <queue>
#include <condition_variable>
#include <chrono>

template<class TMessage> class MultiThreadQueue {
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
	TimePoint Now() { return std::chrono::steady_clock::now(); }
	std::mutex  m_mtxQueue;
	std::condition_variable m_cvQueue;
	std::deque<TMessage> m_queue;
	struct QueueElem {
		TimePoint time;
		mutable TMessage msg;
		bool operator< (const QueueElem& other) const { return time > other.time; }
	};
	std::priority_queue<QueueElem> m_delayed;
public:
	std::optional<TMessage> GetMessageNoWait() {

		std::unique_lock<std::mutex> lock(m_mtxQueue);

		if (!m_queue.empty()) {
			auto res = std::move(m_queue.front());
			m_queue.pop_front();
			return res;
		}
		if (!m_delayed.empty()) {
			auto now = Now();
			auto& it = m_delayed.top();
			auto delayedtoStart = it.time;
			if (now >= delayedtoStart) {
				auto msg = std::move(it.msg);
				m_delayed.pop();
				return msg;
			}
		}
		return std::nullopt;
	}
	TMessage GetMessageWait() {

		std::unique_lock<std::mutex> lock(m_mtxQueue);

		while (true) {

			if (!m_queue.empty()) {
				auto res = std::move(m_queue.front());
				m_queue.pop_front();
				return res;
			}

			if (!m_delayed.empty()) {
				auto now = Now();
				auto& it = m_delayed.top();
				auto delayedtoStart = it.time;
				if (now >= delayedtoStart) {
					auto msg = std::move(it.msg);
					m_delayed.pop();
					return msg;
				}
				m_cvQueue.wait_for(lock, delayedtoStart - now + std::chrono::microseconds(300));
			}
			else {
				m_cvQueue.wait(lock);
			}
		}
	}
	size_t Size() {
		std::unique_lock<std::mutex> lock(m_mtxQueue);
		return m_queue.size() + m_delayed.size();
	}
	void PostMsg(TMessage&& msg, uint64_t delay = 0) {
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
		m_queue.clear();
		m_delayed = {};
	}

};
