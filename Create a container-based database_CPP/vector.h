#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <type_traits>
#include <stdexcept>

namespace cw {

/**
 * @brief vector is a sequence container that encapsulates dynamic size arrays.
 */
template<typename T, typename Alloc = std::allocator<T> >
class vector {
public:
	template<typename ValueT>
	class iterator_
	{
	public:
		typedef iterator_ self_type;
		typedef ValueT value_type;
		typedef ValueT& reference;
		typedef ValueT* pointer;
		//typedef std::forward_iterator_tag iterator_category;
		typedef int difference_type;

		iterator_(pointer ptr) : ptr_(ptr) { }
		iterator_(iterator_ const& ref) = default;

		self_type operator++()
		{
			++ptr_;
			return *this;
		}
		self_type operator++(int postfix)
		{
			self_type prev = *this;
			++ptr_;
			return prev;
		}
		self_type operator--()
		{
			--ptr_;
			return *this;
		}
		self_type operator--(int postfix)
		{
			self_type prev = *this;
			--ptr_;
			return prev;
		}
		reference operator*() { return *ptr_; }
		pointer operator->() { return ptr_; }
		bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
		bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
	private:
		pointer ptr_;
	};

	typedef iterator_<T> iterator;
	typedef iterator_<const T> const_iterator;

	iterator begin()
	{
		return iterator(start);
	}
	iterator end()
	{
		return iterator(finish);
	}

	const_iterator begin() const
	{
		return const_iterator(start);
	}
	const_iterator end() const
	{
		return const_iterator(finish);
	}

public:
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T*		pointer;
	typedef const T*	const_pointer;
	typedef T&		reference;
	typedef const T&	const_reference;
	typedef T		value_type;
	typedef Alloc		allocator_type;
private:
	enum : size_type {
		initial_capacity = 10,
		capacity_factor = 2,
		capacity_sanitize_factor = capacity_factor * 2
	};

	pointer start;
	pointer finish;
	pointer end_of_storage;
	allocator_type a;

	inline pointer allocate(size_type n)
	{
		return n != 0 ? a.allocate(n) : pointer();
	}

	inline void deallocate()
	{
		if(start != nullptr) {
			for(pointer p = start; p != finish; ++p)
				p->~value_type();
			a.deallocate(start, capacity());
		}
	}

	void create_empty()
	{
		start = finish = end_of_storage = nullptr;
	}

	void create_storage(size_type capacity = initial_capacity)
	{
		start = allocate(capacity);
		finish = start;
		end_of_storage = start + capacity;
	}

	void assign_content(size_type a_size, pointer to, const_reference value)
	{
		finish = to + a_size;
		for (pointer p = to; p != finish; ++p)
			new(p) value_type(value);
	}

	void assign_content_to_other(size_type size, const_pointer from, pointer to)
	{
		for (size_type it = 0; it != size; it++)
			new(to + it) value_type(from[it]);
	}

	void assign_content(std::initializer_list<value_type> const& from)
	{
		finish = start + from.size();
		pointer p = start;
		for(auto it = from.begin(); it != from.end(); ++it) {
			new(p) value_type(*it);
			p++;
		}
	}

	void assign_content(size_type a_size, const_reference value)
	{
		assign_content(a_size, start, value);
	}

	void assign_content(vector const& from)
	{
		assign_content(from.size(), from.start);
	}

	void move_content(vector&& vec)
	{
		start = vec.start;
		finish = vec.finish;
		end_of_storage = vec.end_of_storage;
		vec.create_empty();
	}

	inline void sanitize()
	{
		if (capacity() > size() * capacity_sanitize_factor)
			reserve(size() * capacity_factor);
	}
public:
	/**
	 * @brief Returns the number of elements in the container
	 */
	inline size_type size() const noexcept { return finish - start; }

	/**
	 * @brief:	Return size of allocated storage
	 * @return:	Returns the size of the storage space currently
	 * allocated, expressed in terms of bytes.
	 * This capacity is not necessarily equal to the string length.
	 * It can be equal or greater, with the extra space allowing the object
	 * to optimize its operations when new characters are added to
	 * the string.
	 */
	inline size_type capacity() const noexcept
	{
		return end_of_storage - start;
	}

	/**
	 * Returns true if the %vector is empty.
	 * Thus begin would equal to end
	 */
	bool empty() const noexcept { return start == finish; }

	/**
	 *  @brief  Creates a %vector with no elements.
	 */
	vector()
	noexcept(std::is_nothrow_default_constructible<allocator_type>::value)
	{
		create_storage(initial_capacity);
	}

	/**
	 *  @brief  Creates a %vector with default constructed elements.
	 *  @param  n The number of elements to initially create.
	 *
	 *  This constructor fills the %vector with n default
	 *  constructed elements.
	 */
	explicit vector(size_type n)
	{
		create_storage(n * capacity_factor);
		assign_content(n, value_type());
	}

	/**
	 *  @brief  Creates a %vector with copies of an exemplar element.
	 *  @param  n  The number of elements to initially create.
	 *  @param  value  An element to copy.
	 *
	 *  This constructor fills the %vector with @a __n copies of @a __value.
	 */
	explicit vector(size_type n, const_reference value)
	{
		create_storage(n * capacity_factor);
		assign_content(n, value);
	}

	/**
	 *  @brief  Creates a %vector with the content of raw pointer.
	 *  @param  n  The number of elements to initially create.
	 *  @param  p The pointer to copy from.
	 */
	explicit vector(size_type n, const_pointer p) noexcept(false)
	{
		if(!p || !n)
			throw std::invalid_argument("invalid parameters");
		create_storage(n * capacity_factor);
		assign_content(n, p);
	}

	/**
	 *  @brief  %Vector copy constructor.
	 *  @param  v  A %vector of identical element and allocator types.
	 *
	 *  All the elements of x are copied, but any unused capacity in
	 *  v will not be copied
	 *
	 */
	explicit vector(const vector& vec)
	noexcept(std::is_nothrow_constructible<allocator_type>::value)
	{
		create_storage(vec.size() * capacity_factor);
		assign_content(vec.size(), vec.start);
	}

	/**
	 *  @brief  %Vector move constructor.
	 *  @param  v  A %vector of identical element and allocator types.
	 *
	 *  The newly-created %vector contains the exact contents of x.
	 *  The contents of x are a valid empty %vector.
	 */
	explicit vector(vector&& other) noexcept
	{
		create_storage(other.size() * capacity_factor);
		move_content(std::move(other));
	}

	/**
	 *  @brief  Builds a %vector from an initializer list.
	 *  @param  l  An initializer_list.
	 *
	 *  Create a %vector consisting of copies of the elements in the
	 *  initializer_list l.
	 */
	explicit vector(std::initializer_list<value_type> const& list)
	noexcept(std::is_nothrow_constructible<allocator_type>::value)
	{
		create_storage(list.size() * capacity_factor);
		assign_content(list);
	}

	/**
	 * @brief Move assignment operator.
	 * @param other: an instance to move from.
	 */
	vector& operator=(vector&& other) noexcept
	{
		if (this == &other)
			return *this;

		deallocate();
		move_content(std::move(other));

		return *this;
	}

	~vector()
	{
		deallocate();
	}

	vector& merge(vector&& other)
	{
		if (this == &other)
			return *this;

		insert(size(), other);
		other.clear();
		return *this;
	}

	/**
	 * @brief clear:	Clear string
	 * Erases the contents of the string, which becomes an empty string
	 * (with a length of 0 characters).
	 */
	void clear()
	{
		if (size()) {
			deallocate();
			create_empty();
		}
	}

	/**
	 *  @brief  Attempt to preallocate enough memory for specified number of
	 *          elements.
	 *  @param  n  Number of elements required.
	 *
	 *  This function attempts to reserve enough memory for the
	 *  %vector to hold the specified number of elements.
	 */
	void reserve(size_type new_capacity = 0)
	{
		if (new_capacity == 0) {
			clear();
			return;
		}
		if (new_capacity == capacity())
			return;

		size_type new_size = (new_capacity < size()) ? new_capacity : size();
		pointer temp = allocate(new_capacity);
		assign_content_to_other(new_size, start, temp);
		deallocate();

		start = temp;
		finish = temp + new_size;
		end_of_storage = temp + new_capacity;
	}

	/**
	 * @brief Shrink to fit
	 * Requests the string to reduce its capacity to fit its size.
	 */
	void shrink_to_fit() { reserve(size()); }

	/**
	 * @brief Assign content to the vector
	 * @param v: other vector
	 * @return: *this
	 * Assigns a new value to the vector, replacing its current contents.
	 */
	vector& assign(vector const& vec)
	{
		if(this == &vec)
			return *this;

		if(capacity() < vec.size())
			reserve(vec.size() * capacity_factor);

		assign_content(vec);

		return *this;
	}
	vector& operator=(vector const& vec)
	{
		return assign(vec);
	}

	/**
	 * @brief Erase elements from the vector.
	 * @param pos:	Position of the first element to be erased.
	 *		If this is greater than the vector size,
	 *		it does nothing.

	 * @param len:	Number of elements to erase (if the vector is shorter,
	 *		as many characters as possible are erased).
	 * @return:	*this
	 *
	 * Erases the portion of the string value that begins at the character
	 * position pos and spans len characters (or until the end of the
	 * string, if either the content is too short or if len = size.
	 * Notice that the default argument erases all characters in the string
	 * (like member function clear).
	 *
	 * Note: The first character in str is denoted by a value of 0 (not 1).
	 */
	vector& erase(size_type pos = 0, size_type len = 0)
	{
		if (pos > size())        // the user is wrong, do nothing.
			return *this;
		if (len == 0)
			len = size();

		while (pos + len < size()) { // '<' is essential
			start[pos] = start[pos + len];
			++pos;
		}
		finish = start + pos;
		sanitize();
		return *this;
	}

	/**
	 * @brief pop_back: Delete last element
	 * Erases the last element of the vector, effectively reducing its
	 * size by one. Equivalent to erase(size()-1, 1)
	 */
	void pop_back()
	{
		if (size() > 0) {
			--finish;
			sanitize();
		}
	}

	/**
	 *  @brief  Add data to the end of the %vector.
	 *  @param  x Data to be added.
	 *
	 *  This is a typical stack operation.  The function creates an
	 *  element at the end of the %vector and assigns the given data
	 *  to it.
	 */
	void push_back(const_reference element)
	{
		if(capacity() < size() + 1)
			reserve((size() + 1) * capacity_factor);
		new(finish) value_type(element);
		finish++;
	}



	/**
	 * @brief Inserts elements at the specified location in the container.
	 * @param pos:		Insertion point
	 * @param pointer:	pointer
	 * @param size:		size of the range
	 * @return		*this
	 * Inserts value before pos.
	 */
	vector& insert(size_type pos, const_pointer p, size_type p_size)
	{
		vector temp;
		if (pos >= size())
			pos = size();
		else
			temp = vector(size() - pos, start + pos);

		size_type res_size = size() + p_size;

		if (capacity() < res_size)
			reserve(res_size * capacity_factor);

		size_type i = 0, j = 0;
		for (i = pos, j = 0; j != p_size; i++, j++)
			start[i] = p[j];
		if(pos != size())
			for (j = 0; j != temp.size(); j++, i++)
				start[i] = temp[j];

		finish = start + res_size;
		return *this;
	}
	vector&
	insert(size_type pos, vector const& p, size_type inserted_size = 0)
	{
		if (!inserted_size)
			inserted_size = p.size();
		return insert(pos, p.data(), inserted_size);
	}

	/**
	 *  @brief  Subscript access to the data contained in the %vector.
	 *  @param pos The index of the element for which data should be
	 *  accessed.
	 *  @return  Read/write reference to data.
	 *
	 */
	reference operator[](size_type pos) noexcept(false)
	{
		if (pos >= size())
			throw std::out_of_range("No such element.");
		return (*(start + pos));
	}
	const_reference operator[](size_type pos) const noexcept(false)
	{
		//return operator [](pos);
		if (pos >= size())
			throw std::out_of_range("No such element.");
		return (*(start + pos));
	}

	/**
	 *   Returns a pointer such that [data(), data() + size()) is a valid
	 *   range.
	 */
	pointer	data() noexcept { return start; }
	const_pointer data() const noexcept { return start; }

};

} // namespace lab

#endif // VECTOR_H
