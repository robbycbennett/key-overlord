#pragma once


// A read-only reverse iterator
template<typename T>
class ReverseIterator
{
public:
	constexpr ReverseIterator(const T *i)
	{
		m_i = i;
	}

	constexpr operator const T *() const
	{
		return m_i;
	}

	constexpr void operator++()
	{
		m_i--;
	}

private:
	const T *m_i;
};
