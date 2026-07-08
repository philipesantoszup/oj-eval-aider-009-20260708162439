#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <new>
#include <memory>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector
{
public:
	class const_iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = const T*;
		using reference = const T&;
		using iterator_category = std::random_access_iterator_tag;

	private:
		pointer ptr;

	public:
		const_iterator(pointer p = nullptr) : ptr(p) {}
		reference operator*() const { return *ptr; }
		pointer operator->() const { return ptr; }
		const_iterator& operator++() { ++ptr; return *this; }
		const_iterator operator++(int) { const_iterator tmp = *this; ++ptr; return tmp; }
		const_iterator& operator--() { --ptr; return *this; }
		const_iterator operator--(int) { const_iterator tmp = *this; --ptr; return tmp; }
		const_iterator operator+(difference_type n) const { return const_iterator(ptr + n); }
		const_iterator operator-(difference_type n) const { return const_iterator(ptr - n); }
		difference_type operator-(const const_iterator& rhs) const { return ptr - rhs.ptr; }
		const_iterator& operator+=(difference_type n) { ptr += n; return *this; }
		const_iterator& operator-=(difference_type n) { ptr -= n; return *this; }
		bool operator==(const const_iterator& rhs) const { return ptr == rhs.ptr; }
		bool operator!=(const const_iterator& rhs) const { return ptr != rhs.ptr; }
		bool operator<(const const_iterator& rhs) const { return ptr < rhs.ptr; }
		bool operator<=(const const_iterator& rhs) const { return ptr <= rhs.ptr; }
		bool operator>(const const_iterator& rhs) const { return ptr > rhs.ptr; }
		bool operator>=(const const_iterator& rhs) const { return ptr >= rhs.ptr; }
	};

	class iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::random_access_iterator_tag;

	private:
		pointer ptr;

	public:
		iterator(pointer p = nullptr) : ptr(p) {}
		reference operator*() const { return *ptr; }
		pointer operator->() const { return ptr; }
		iterator& operator++() { ++ptr; return *this; }
		iterator operator++(int) { iterator tmp = *this; ++ptr; return tmp; }
		iterator& operator--() { --ptr; return *this; }
		iterator operator--(int) { iterator tmp = *this; --ptr; return tmp; }
		iterator operator+(difference_type n) const { return iterator(ptr + n); }
		iterator operator-(difference_type n) const { return iterator(ptr - n); }
		difference_type operator-(const iterator& rhs) const { return ptr - rhs.ptr; }
		iterator& operator+=(difference_type n) { ptr += n; return *this; }
		iterator& operator-=(difference_type n) { ptr -= n; return *this; }
		bool operator==(const iterator& rhs) const { return ptr == rhs.ptr; }
		bool operator!=(const iterator& rhs) const { return ptr != rhs.ptr; }
		bool operator<(const iterator& rhs) const { return ptr < rhs.ptr; }
		bool operator<=(const iterator& rhs) const { return ptr <= rhs.ptr; }
		bool operator>(const iterator& rhs) const { return ptr > rhs.ptr; }
		bool operator>=(const iterator& rhs) const { return ptr >= rhs.ptr; }
		
		bool operator==(const const_iterator& rhs) const { return ptr == rhs.ptr; }
		bool operator!=(const const_iterator& rhs) const { return ptr != rhs.ptr; }
	};

	vector() : data(nullptr), size_val(0), capacity_val(0) {}

	vector(const vector& other) : size_val(other.size_val), capacity_val(other.capacity_val) {
		data = static_cast<T*>(::operator new(capacity_val * sizeof(T)));
		for (size_t i = 0; i < size_val; ++i) {
			new (data + i) T(other.data[i]);
		}
	}

	~vector() {
		clear();
		::operator delete(data);
	}

	vector& operator=(const vector& other) {
		if (this != &other) {
			if (other.size_val > capacity_val) {
				reserve(other.capacity_val);
			}
			clear();
			for (size_t i = 0; i < other.size_val; ++i) {
				new (data + i) T(other.data[i]);
			}
			size_val = other.size_val;
		}
		return *this;
	}

	T& at(const size_t& pos) {
		if (pos >= size_val) throw index_out_of_bound();
		return data[pos];
	}

	const T& at(const size_t& pos) const {
		if (pos >= size_val) throw index_out_of_bound();
		return data[pos];
	}

	T& operator[](const size_t& pos) {
		if (pos >= size_val) throw index_out_of_bound();
		return data[pos];
	}

	const T& operator[](const size_t& pos) const {
		if (pos >= size_val) throw index_out_of_bound();
		return data[pos];
	}

	const T& front() const {
		if (size_val == 0) throw container_is_empty();
		return data[0];
	}

	const T& back() const {
		if (size_val == 0) throw container_is_empty();
		return data[size_val - 1];
	}

	iterator begin() { return iterator(data); }
	const_iterator begin() const { return const_iterator(data); }
	const_iterator cbegin() const { return const_iterator(data); }

	iterator end() { return iterator(data + size_val); }
	const_iterator end() const { return const_iterator(data + size_val); }
	const_iterator cend() const { return const_iterator(data + size_val); }

	bool empty() const { return size_val == 0; }
	size_t size() const { return size_val; }

	void clear() {
		for (size_t i = 0; i < size_val; ++i) {
			data[i].~T();
		}
		size_val = 0;
	}

	iterator insert(iterator pos, const T& value) {
		size_t index = pos.ptr - data;
		if (index > size_val) throw index_out_of_bound();
		
		if (size_val == capacity_val) {
			reserve(capacity_val == 0 ? 1 : capacity_val * 2);
		}

		for (size_t i = size_val; i > index; --i) {
			new (data + i) T(std::move(data[i - 1]));
			data[i - 1].~T();
		}
		new (data + index) T(value);
		size_val++;
		return iterator(data + index);
	}

	iterator insert(const size_t& ind, const T& value) {
		if (ind > size_val) throw index_out_of_bound();
		return insert(iterator(data + ind), value);
	}

	iterator erase(iterator pos) {
		size_t index = pos.ptr - data;
		if (index >= size_val) throw index_out_of_bound();

		for (size_t i = index; i < size_val - 1; ++i) {
			new (data + i) T(std::move(data[i + 1]));
			data[i + 1].~T();
		}
		data[size_val - 1].~T();
		size_val--;
		return iterator(data + index);
	}

	iterator erase(const size_t& ind) {
		if (ind >= size_val) throw index_out_of_bound();
		return erase(iterator(data + ind));
	}

	void push_back(const T& value) {
		if (size_val == capacity_val) {
			reserve(capacity_val == 0 ? 1 : capacity_val * 2);
		}
		new (data + size_val) T(value);
		size_val++;
	}

	void pop_back() {
		if (size_val == 0) throw container_is_empty();
		data[size_val - 1].~T();
		size_val--;
	}

private:
	void reserve(size_t new_cap) {
		if (new_cap <= capacity_val) return;
		T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
		for (size_t i = 0; i < size_val; ++i) {
			new (new_data + i) T(std::move(data[i]));
			data[i].~T();
		}
		::operator delete(data);
		data = new_data;
		capacity_val = new_cap;
	}

	T* data;
	size_t size_val;
	size_t capacity_val;
};

}

#endif
