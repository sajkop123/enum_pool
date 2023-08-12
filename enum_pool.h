#include <list>
#include <map>
#include <array>
#include <stdexcept>
#include <algorithm>

#include "common.h"

// template<typename T>
// void function() {
//   puts(__PRETTY_FUNCTION__);
// }

template<typename _Key,
         typename _Tp,
         size_t _MaxEnumSize = static_cast<size_t>(_Key::ENUM_MAP_MAX_SIZE)>
class enum_map {
 private:
  typedef std::map<_Key, _Tp> stlmap_container_t;

 public:
  // types for user:
  typedef _Key                                          key_type;
  typedef _Tp                                           mapped_type;
  typedef std::pair<const _Key, _Tp>                    value_type;
  typedef std::list<value_type>                         data_container_t;
  typedef typename data_container_t::reference          reference;
  typedef typename data_container_t::const_reference    const_reference;
  typedef typename data_container_t::iterator           iterator;
  typedef typename data_container_t::const_iterator     const_iterator;
  typedef typename data_container_t::pointer            pointer;
  typedef typename data_container_t::const_pointer      const_pointer;
  typedef typename data_container_t::size_type          size_type;
  typedef std::reverse_iterator<iterator>               reverse_iterator;
  typedef std::reverse_iterator<const_iterator>         const_reverse_iterator;

  typedef std::array<pointer, _MaxEnumSize> mapping_container_t;


 public:  // Member functions:
  explicit enum_map() {
    clear();
  }

  enum_map(std::initializer_list<value_type> __l) {
    __clear_m_data();
    for (auto& it : __l) {
      m_data.emplace_back(it);
    }
    __fill_m_ptr();
  }

  enum_map(const enum_map& __other) {
    m_data = __other.m_data;
    __fill_m_ptr();
  }

  enum_map(enum_map&& __other) {
    m_data = std::forward<data_container_t>(__other.m_data);
    __fill_m_ptr();
    __other.clear();
  }

  template<typename _InputIterator>
  enum_map(_InputIterator __first, _InputIterator __second) {
    clear();
    for (auto& it = __first; it != __second; ++it) {
      m_data.emplace_back(it);
    }
    __fill_m_ptr();
  }

  ~enum_map() = default;

  enum_map& operator=(const enum_map& __other) {
    m_data = __other.m_data;
    __fill_m_ptr();
    return *this;
  }

  enum_map& operator=(enum_map&& __other) {
    m_data = std::move<data_container_t>(__other.m_data);
    m_ptr = std::move<data_container_t>(__other.m_ptr);
    return *this;
  }

  enum_map& operator=(std::initializer_list<value_type> __l) {
    clear();
    for (auto& it : __l) {
      __insert_unique_(it);
    }
  }

 public:  // Element access:
  mapped_type&
  at(const key_type& __key) {
    pointer p = m_ptr[index(__key)];
    if (!p) {
      throw std::runtime_error("access data not in the map");
    }
    return (*p).second;
  }

  const mapped_type&
  at(const key_type& __key) const {
    const_pointer p = m_ptr[index(__key)];
    if (!p) {
      throw std::runtime_error("access data not in the map");
    }
    return (*p).second;
  }

  mapped_type&
  operator[](const key_type& __key) {
    pointer p = m_ptr[index(__key)];
    if (!p) {
      // need make sure mapped_type is default constructible
      m_data.emplace_back(value_type(__key, mapped_type()));
      p = std::addressof(m_data.back());
      m_ptr[index(__key)] = p;
    }
    return *p;
  }

  mapped_type&
  operator[](key_type&& __key) {
    pointer p = m_ptr[index(std::forward<key_type>(__key))];
    if (!p) {
      // need make sure mapped_type is default constructible
      m_data.emplace_back(value_type(__key, mapped_type()));
      p = std::addressof(m_data.back());
      m_ptr[index(__key)] = p;
    }
    return *p;
  }

 public:  // Iterators:
  iterator
  begin() {
    return m_data.begin();
  }

  iterator
  end() {
    return m_data.end();
  }

  const_iterator
  begin() const noexcept {
    return m_data.begin();
  }

  const_iterator
  end() const noexcept {
    return m_data.end();
  }

  const_iterator
  cbegin() const noexcept {
    return m_data.cbegin();
  }

  const_iterator
  cend() const noexcept {
    return m_data.cend();
  }

  reverse_iterator
  rbegin() noexcept {
    return m_data.rbegin();
  }

  reverse_iterator
  rend() noexcept {
    return m_data.rend();
  }

  const_reverse_iterator
  crbegin() const noexcept {
    return m_data.rbegin();
  }

  const_reverse_iterator
  crend() const noexcept {
    return m_data.rend();
  }

 public:  // Capacity:
  bool
  empty() const noexcept {
    return size();
  }

  size_type
  size() const noexcept {
    return m_data.size();
  }

  size_type
  max_size() const noexcept {
    return _MaxEnumSize;
  }

 public:  // Modifier:
  void
  clear() noexcept {
    __clear_m_data();
    __clear_m_ptr();
  }

  std::pair<iterator, bool>
  insert(const value_type& __value) {
    return __insert_unique_(__value);
  }

  std::pair<iterator, bool>
  insert(value_type&& __value) {
    return __insert_unique_(std::move(__value));
  }

  void
  insert(std::initializer_list<value_type> __l) {
    for (auto& it : __l) {
      insert(it);
    }
  }

  template<class... Args>
  std::pair<iterator, bool>
  emplace(Args&&... __args) {
    return __insert_unique_(std::forward<Args>(__args)...);
  }

  // template <typename... _Args>
	// std::pair<iterator, bool>
	// try_emplace(const key_type& __key, _Args&&... __args) {
  //   insert
  // }


  void
  erase(iterator __pos) {
    __erase_by_iter_(__pos);
  }

  void erase(const_iterator __pos) {
    __erase_by_iter_(__pos);
  }

  size_type
  erase(const key_type& __key) {
    return __erase_by_key_(__key);
  }

  size_type
  erase(key_type&& __key) {
    return __erase_by_key_(std::forward<key_type>(__key));
  }

  void swap(enum_map& other) {
    std::swap(m_data, other.m_data);
    std::swap(m_ptr, other.m_ptr);
  }

 public:  // Lookup:
  size_type
  count(const key_type& __key) {
    return m_ptr[index(__key)] ? 1 : 0;
  }

  size_type
  count(key_type&& __key) {
    return m_ptr[index(std::move(__key))] ? 1 : 0;
  }

  iterator
  find(const key_type& __key) const {
    return std::find(m_data.begin(), m_data.end(),
          [&](const value_type& v) {
            return v.first == __key;
          });
  }

  const_iterator
  find(const key_type& __key) {
    return std::find(m_data.begin(), m_data.end(),
          [&](const value_type& v) {
            return v.first == __key;
          });
  }

 private:  // private method

  inline size_t
  index(const key_type& __key) const {
    size_t index = static_cast<size_t>(__key);
    if (index < 0 || index >= _MaxEnumSize) {
      throw std::runtime_error("invalid enum to size_t");
    }
    return index;
  }

  inline size_t
  index(key_type&& __key) const {
    size_t index = static_cast<size_t>(std::forward<key_type>(__key));
    if (index < 0 || index >= _MaxEnumSize) {
      throw std::runtime_error("invalid enum to size_t");
    }
    return index;
  }

  /**
   * make insertion of a value_type object.
   *
   * @param __value
   * @return std::pair<iterator, bool>
   *    consisting of an iterator to the inserted element and a bool value
   *    set to `true` if and only if the insertion took place.
   */
  inline std::pair<iterator, bool>
  __insert_unique_(value_type&& __value) {
    size_t id = index(std::forward<value_type>(__value).first);
    pointer p = m_ptr[id];
    if (!p) {
      // not stored, push a new value_type data
      m_data.emplace_back(std::forward<value_type>(__value));
      p = std::addressof(m_data.back());
      m_ptr[id] = p;
      return {std::prev(m_data.end()), false};
    } else {
      // already stored, overwrite the mapped_type data
      // not sure if there any solution better than search...
      iterator it = std::find_if(m_data.begin(), m_data.end(),
          [__key = __value.first](const value_type& v) {
            return v.first == __key;
          });
      if (it != m_data.end()) {
        (*it).second = std::forward<value_type>(__value).second;
      }
      return {it, true};
    }
  }

  /**
   * insert in-place if the key does not exist, do nothing if the key exists.
   *
   * @param __value same as for emplace
   * @return std::pair<iterator, bool> same as for emplace
   */
  inline std::pair<iterator, bool>
  __try_insert_unique_(value_type&& __value) {
    size_t id = index(std::forward<value_type>(__value).first);
    pointer p = m_ptr[id];
    if (!p) {
      // not stored, push a new value_type data
      m_data.emplace_back(std::forward<value_type>(__value));
      p = std::addressof(m_data.back());
      m_ptr[id] = p;
      return {std::prev(m_data.end()), false};
    } else {
      // already stored, search the iterator only, do nothing.
      // not sure if there any solution better than search...
      iterator it = std::find_if(m_data.begin(), m_data.end(),
          [__key = __value.first](const value_type& v) {
            return v.first == __key;
          });
      return {it, true};
    }
  }

  inline size_type
  __erase_by_key_(key_type&& __key) {
    iterator it = std::find_if(m_data.begin(), m_data.end(),
        [&](const value_type& v) {
          return v.first == __key;
        });
    if (it != m_data.end()) {
      m_data.erase(it);
      __fill_m_ptr();
      return 1;
    }
    return 0;
  }

  inline size_type
  __erase_by_key_(const key_type& __key) {
    iterator it = std::find_if(m_data.begin(), m_data.end(),
        [&](const value_type& v) {
          return v.first == __key;
        });
    if (it != m_data.end()) {
      m_data.erase(it);
      __fill_m_ptr();
      return 1;
    }
    return 0;
  }

  inline void
  __erase_by_iter_(const_iterator __pos) {
    m_data.erase(__pos);
    __fill_m_ptr();
  }

  inline void __fill_m_ptr() {
    __clear_m_ptr();
    for (auto& data : m_data) {
      m_ptr[index(data.first)] = std::addressof(data);
    }
  }

  inline void __clear_m_data() {
    m_data.clear();
  }
  inline void __clear_m_ptr() {
    std::fill(m_ptr.begin(), m_ptr.end(), nullptr);
  }

 private:
  data_container_t m_data;
  mapping_container_t m_ptr;
};