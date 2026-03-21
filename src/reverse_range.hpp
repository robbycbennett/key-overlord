#pragma once


#include "reverse_iterator.hpp"


// A read-only reverse begin iterator and reverse end iterator
template<typename T>
class ReverseRange
{
public:
	constexpr ReverseRange(const T *begin, const T *end)
	{
		m_begin = begin;
		m_end = end;
	}

	constexpr ReverseIterator<T> begin()
	{
		return m_begin;
	}

	constexpr ReverseIterator<T> end()
	{
		return m_end;
	}

private:
	const T *m_begin;
	const T *m_end;
};
