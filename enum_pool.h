#include <list>
#include <map>
#include <array>
#include <stdexcept>

#include "common.h"

// template<typename T>
// void function() {
//   puts(__PRETTY_FUNCTION__);
// }

template<typename _Key,
         typename _Tp,
         size_t _MaxEnumSize = 10>
class enum_map {
 private:
  typedef std::map<_Key, _Tp> stlmap_container_t;

 public:
  // types for user:
  typedef _Key                                          key_type;
  typedef _Tp                                           mapped_type;
  typedef std::pair<const _Key, _Tp>                    value_type;
  typedef typename stlmap_container_t::reference        reference;
  typedef typename stlmap_container_t::const_reference  const_reference;
  typedef typename stlmap_container_t::iterator         iterator;
  typedef typename stlmap_container_t::const_iterator   const_iterator;
  typedef typename stlmap_container_t::pointer          pointer;
  typedef typename stlmap_container_t::const_pointer    const_pointer;
  typedef typename stlmap_container_t::size_type        size_type;
  typedef std::reverse_iterator<iterator>               reverse_iterator;
  typedef std::reverse_iterator<const_iterator>         const_reverse_iterator;

 private:
  typedef std::list<value_type> data_container_t;
  typedef std::array<pointer, _MaxEnumSize> mapping_container_t;

 public:
  explicit enum_map() : m_ptr({nullptr}) {}

  enum_map(std::initializer_list<value_type> __l)
      : enum_map() {
    for (auto& it : __l) {
      m_data.emplace_back(it);
    }
    fill_m_ptr();
  }

  enum_map(const enum_map& __other) {
    m_data = __other.m_data;
    fill_m_ptr();
  }

  enum_map(enum_map&& __other) {
    m_data = std::forward<data_container_t>(__other.m_data);
    fill_m_ptr();
    __other.clear();
  }

  template<typename _InputIterator>
  enum_map(_InputIterator __first, _InputIterator __second) {
    for (auto& it = __first; it != __second; ++it) {
      m_data.emplace_back(it);
    }
    fill_m_ptr();
  }

  ~enum_map() = default;

  // Member functions:

  enum_map& operator=(const enum_map& __other) {
    m_data = __other.m_data;
    fill_m_ptr();
    return *this;
  }
  enum_map& operator=(enum_map&& __other) {
    m_data = std::forward<data_container_t>(__other.m_data);
    fill_m_ptr();
    __other.clear();
    return *this;
  }
  enum_map& operator=(std::initializer_list<value_type> __l) {
    for (auto& it : __l) {
      m_data.emplace_back(it);
    }
    fill_m_ptr();
  }

  // Element access:
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

  // Iterators:

  // Capacity:
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

  // Modifier:
  void
  clear() noexcept {
    m_data.clear();
    std::fill(m_ptr.begin(), m_ptr.end(), nullptr);
  }

  // std::pair<iterator, bool>
  // insert(const value_type& __x) {

  // }



  // Lookup:

 private:
  inline size_t
  index(const key_type& __key) const {
    size_t index = static_cast<size_t>(__key);
    if (index < 0 || index >= _MaxEnumSize) {
      throw std::runtime_error("invalid enum to size_t");
    }
    return index;
  }

  inline void fill_m_ptr() {
    for (auto& data : m_data) {
      m_ptr[index(data.first)] = std::addressof(data);
    }
  }

 private:
  data_container_t m_data;
  mapping_container_t m_ptr;
};