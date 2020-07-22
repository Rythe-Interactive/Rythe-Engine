#pragma once
#include <atomic>

namespace args::core::async
{
	template <typename T>
	struct transferable_atomic
	{
	private:
		std::atomic<T> m_atomic;
	public:
		transferable_atomic() noexcept = default;

		constexpr transferable_atomic(T val) noexcept : m_atomic(val) {}

		transferable_atomic(const std::atomic<T>& other) : m_atomic(other.load(std::memory_order_acquire)) {}

		transferable_atomic(const transferable_atomic<T>& other) : m_atomic(other->load(std::memory_order_acquire)) {}

		transferable_atomic& operator=(const transferable_atomic<T>& other)
		{
			auto val = other->load(std::memory_order_acquire);
			try
			{
				m_atomic.store(val, std::memory_order_release);
			}
			catch (...)
			{
				std::cout << "whut" << std::endl;
			}
		}

		transferable_atomic& operator=(const std::atomic<T>& other)
		{
			m_atomic.store(other.load(std::memory_order_acquire), std::memory_order_release);
		}

		transferable_atomic& copy(const transferable_atomic<T>& other, std::memory_order loadOrder = std::memory_order_acquire, std::memory_order storeOrder = std::memory_order_release)
		{
			m_atomic.store(other->load(loadOrder), storeOrder);
		}

		transferable_atomic& move(const transferable_atomic<T>& other, std::memory_order loadOrder = std::memory_order_acquire, std::memory_order storeOrder = std::memory_order_release)
		{
			m_atomic.store(other->load(loadOrder), storeOrder);
			other->store(T(), storeOrder);
		}

		std::atomic<T>& get()
		{
			return m_atomic;
		}

		const std::atomic<T>& get() const
		{
			return m_atomic;
		}

		std::atomic<T>* operator->()
		{
			return &m_atomic;
		}

		const std::atomic<T>* operator->() const
		{
			return &m_atomic;
		}
	};
}