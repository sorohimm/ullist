//
// Created by sorohimm on 29.04.2021.
// Copyright © 2020 Vladimir Mashir.
//

#ifndef UNROLLED_LINKED_LIST_ULLIST_HPP
#define UNROLLED_LINKED_LIST_ULLIST_HPP

#include <algorithm>
#include <exception>
#include <initializer_list>
#include <memory>
#include <sstream>
#include <string>

namespace ull {

template <class T, size_t nodeSize = 5>
class ullist {
 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using self = ullist;
  using self_reference = ullist&;

  ullist();
  explicit ullist(size_t);
  ullist(std::initializer_list<value_type>);
  ~ullist();
  reference operator[](size_t);
  const_reference operator[](size_t) const;
  bool operator==(std::initializer_list<value_type>) const noexcept;
  self_reference operator=(std::initializer_list<value_type>);
  void push_back(const_reference);
  void pop_back();
  const_reference back() const;
  const_reference front() const;
  reference back();
  reference front();
  size_t inline size() const noexcept;
  size_t inline nodeCount() const noexcept;
  void insert(size_t, const_reference);
  void resize(size_t);
  void remove(size_t);
  void clear();
  bool empty() noexcept;
  void bufferedClear();
  std::string str() const;

 private:
  // template<class Allocator = std::allocator<value_type>>
  struct Node {
    Node* prev_;
    Node* next_;
    value_type* data_;
    size_t size_;
    std::allocator<value_type> allocator_;

    Node();
    Node(Node*, Node*);
    ~Node();
    void clear();
    void push_back(const_reference);
    const_reference back() const;
    const_reference front() const;
    reference back();
    reference front();
    void pop_back();
    void insert(size_t, const_reference);
    value_type remove(size_t);
    reference operator[](size_t);
    const_reference operator[](size_t) const;
  };

  Node* head_;
  Node* tail_;
  Node* cache_;
  size_t size_;
  size_t nodeCount_;

  Node* popData();
  void pushData(Node*);
  Node* newNode();
  Node* appendNewTailNode();
  Node* appendNewTailNode(Node*);
  Node* appendNewHeadNode();
  Node* appendNewHeadNode(Node*);
  void deleteNode(Node*);
  void at(size_t, Node*&, size_t&) const;
  bool isNodeFull(Node*);
  bool isNodeEmpty(Node*);
  void link(Node*, Node*);
  void unlink(Node*, Node*);

  bool isInBounds(size_t offset, size_t bounds, size_t index) const {
    return index >= offset && index < offset + bounds;
  }
};

#define ULL template <class value_type, size_t nodeSize> \
            ullist<value_type, nodeSize>::
#define ULL_NODE template <class value_type, size_t nodeSize> \
                 ullist<value_type, nodeSize>::Node::
#define ULL_IMPL(rt) template <class value_type, size_t nodeSize> \
                     rt ullist<value_type, nodeSize>::
#define ULL_TEMPL_IMPL(type) template <class value_type, size_t nodeSize> \
  typename ullist<value_type, nodeSize>::type ullist<value_type, nodeSize>::

#define ULL_NODE_IMPL(rt) template <class value_type, size_t nodeSize> \
                          rt ullist<value_type, nodeSize>::Node::
#define ULL_NODE_TEMPL_IMPL(type) template <class value_type, size_t nodeSize> \
                                  typename ullist<value_type, nodeSize>::type  \
                                  ullist<value_type, nodeSize>::Node::

ULL ullist() : head_(nullptr), tail_(nullptr), cache_(nullptr), size_(0),
                nodeCount_(0) {}

ULL ullist(size_t _n)
    : head_(nullptr), tail_(nullptr), cache_(nullptr), size_(_n) {
  if (_n > 0) {
    size_t lim = ceil(static_cast<double>(_n) / nodeSize);
    nodeCount_ = lim;
    for (size_t i = 0; i < lim; ++i) {
      pushData(new Node);
    }
  }
}

ULL ullist(std::initializer_list<value_type> _init_list) {
  nodeCount_ = 0;
  size_ = 0;
  head_ = nullptr;
  tail_ = nullptr;
  cache_ = nullptr;

  *this = _init_list;
}

ULL ~ullist() {
  if (head_) {
    Node* current = head_;
    while (current) {
      Node* tmp = current;
      current = current->next_;
      if (current) {
        delete tmp;
      }
    }
  }
  while (cache_) {
    Node* tmp = cache_;
    cache_ = cache_->next_;
    if (cache_) {
      delete tmp;
    }
  }
}

ULL_TEMPL_IMPL(reference) operator[](size_t _i) {
  if (isInBounds(static_cast<size_t>(0), size_, _i)) {
    Node* current = nullptr;
    size_t off = 0;
    at(_i, current, off);
    return current->operator[](off);
  }
  throw std::out_of_range("ullist::the index is out of bounds of the array");
}

ULL_TEMPL_IMPL(const_reference) operator[](size_t _i) const {
  if (isInBounds(static_cast<size_t>(0), size_, _i)) {
    Node* current = nullptr;
    size_t off = 0;
    at(_i, current, off);
    return current->operator[](off);
  }
  throw std::out_of_range("ullist::the index is out of bounds of the array");
}

ULL_IMPL(bool)
operator==(std::initializer_list<value_type> _list) const noexcept {
  if (_list.size() != size_) {
    return false;
  }
  for (size_t i = 0; i < size_ - 1; ++i) {
    if (this->operator[](i) != *(_list.begin() + i)) {
      return false;
    }
  }
  return true;
}

ULL_TEMPL_IMPL(self_reference)operator=(
    std::initializer_list<value_type> _list) {
  if (*this == _list) {
    return *this;
  }
  if (!empty()) {
    bufferedClear();
  }

  size_t i = 0;
  while (size_ != _list.size()) {
    Node* current = newNode();
    while (!isNodeFull(current) && size_ < _list.size()) {
      ++size_;
      current->push_back(*(_list.begin() + i));
      ++i;
    }

    if (this->nodeCount_ == 0) {
      appendNewHeadNode(current);
    } else {
      appendNewTailNode(current);
    }
  }
  return *this;
}

ULL_IMPL(void) push_back(const_reference _value) {
  if (isNodeFull(tail_)) {
    appendNewTailNode();
  }
  tail_->push_back(_value);
  ++size_;
}

ULL_TEMPL_IMPL(reference) back() {
  if (tail_) {
    return tail_->back();
  }
  throw std::out_of_range("ullist::back called on empty ullist");
}

ULL_TEMPL_IMPL(reference) front() {
  if (head_) {
    return head_->front();
  }
  throw std::out_of_range("ullist::front called on empty ullist");
}

ULL_TEMPL_IMPL(const_reference) back() const {
  if (tail_) {
    return tail_->back();
  }
  throw std::out_of_range("ullist::back called on empty ullist");
}

ULL_TEMPL_IMPL(const_reference) front() const {
  if (head_) {
    return tail_->front();
  }
  throw std::out_of_range("ullist::front called on empty ullist");
}

ULL_IMPL(void) pop_back() {
  if (size_ > 0) {
    tail_->pop_back();
    if (isNodeEmpty(tail_)) {
      deleteNode(tail_);
    }
    --size_;
  } else {
    throw std::out_of_range("ullist::pop_back called on empty ullist");
  }
}

ULL_IMPL(size_t) size() const noexcept { return size_; }

ULL_IMPL(size_t) nodeCount() const noexcept { return nodeCount_; }

ULL_IMPL(void) insert(size_t _i, const_reference _value) {
  if (_i >= size_) {
    push_back(_value);
  } else if (_i == 0) {
    if (isNodeFull(head_)) {
      appendNewHeadNode();
    }
    head_->insert(0, _value);
    ++size_;
  } else {
    Node* target = nullptr;
    size_t offset = 0;
    at(_i, target, offset);
    if (isNodeFull(target)) {
      if (offset == 0) { // вставляем до
        Node* insert;
        if (isNodeFull(target->prev_)) {
          insert = newNode();
          link(target->prev_, insert);
          link(insert, target);
          ++nodeCount_;
        } else {
          insert = target->prev_;
        }
        insert->push_back(_value);
      } else {  // вставка в середину нода
        Node* insert = nullptr;
        size_t shiftLen = nodeSize - offset;
        if (target->next_ && target->next_->size_ + shiftLen <= nodeSize) {
          // в этом случае у правого next_ нода есть место для смещения
          // перемещаемся на _next
          insert = target->next_;
          std::move(insert->data_ - 1,
                             insert->data_ + insert->size_ - 1,
                             insert->data_ + insert->size_ + shiftLen - 1);
          // перемещаем элементы из target в _next
          std::move(target->data_ + offset, target->data_ + nodeSize,
                    insert->data_);
          target->size_ -= shiftLen;
          insert->size_ += shiftLen;
        } else {  // если target == tail_ или target->next_ заполнена или нет
                  // места, чтобы сдвинуть вправо
                  // в этом случае _value уйдет в новый нод
          insert = newNode();
          if (target == tail_) {
            link(tail_, insert);
            tail_ = insert;
          } else {
            link(insert, target->next_);
            link(target, insert);
          }
          // пермещаем элементы и target в новый нод
          std::move(target->data_ + offset, target->data_ + nodeSize,
                    insert->data_);

          target->size_ -= shiftLen;
          insert->size_ += shiftLen;
          ++nodeCount_;
        }
        target->push_back(_value);
      }
    } else {
      target->insert(offset, _value);
    }
    ++size_;
  }
}

ULL_IMPL(void) resize(size_t _n) {
  if (_n == size_) {
    throw std::bad_alloc();
  }
  size_t newNodeCount = ceil(static_cast<float>(_n) / nodeSize);
  if (_n > size_) {
    if (newNodeCount == nodeCount_) {
      size_ = _n;
      return;
    } else if (nodeCount_ == 0) {
      while (nodeCount_ != newNodeCount) {
        if (this->nodeCount_ == 0) {
          appendNewHeadNode();
        } else {
          appendNewTailNode();
        }
      }
    } else {
      while (nodeCount_ != newNodeCount) {
        appendNewTailNode();
      }
    }
  } else if (_n < size_) {
    while (size_ != _n) {
      pop_back();
    }
  }
  size_ = _n;
}

ULL_IMPL(void) remove(size_t _i) {
  if (size_ == 0) {
    throw std::out_of_range("ullist::remove called on empty ullist");
  }
  if (_i >= size_) {
    throw std::out_of_range("ullist::the index is out of bounds of the array");
  }

  Node* target = nullptr;
  size_t offset;
  at(_i, target, offset);

  target->remove(offset);
  if (isNodeEmpty(target)) {
    deleteNode(target);
  }
  --size_;
}

ULL_IMPL(void) clear() {
  try {
    bufferedClear();
  } catch (...) {
    throw std::out_of_range("ullist::clear called on empty ullist");
  }

  while (cache_) {
    Node* temp = cache_;
    cache_ = cache_->next_;
    delete temp;
  }
}

ULL_IMPL(void) bufferedClear() {
  if (empty()) {
    throw std::out_of_range("ullist::bufferClear called on empty ullist");
  }
  for (Node* current = head_; current;) {
    Node* temp = current;
    current = current->next_;
    if (current) {
      delete temp;
    }
  }
  head_ = nullptr;
  tail_ = nullptr;
  nodeCount_ = 0;
  size_ = 0;
}

ULL_IMPL(std::string) str() const {
  std::stringstream outstream;
  outstream << "{";

  Node* current = head_;

  for (size_t nodeIdx = 0; current; ++nodeIdx, current = current->next_) {
    if (nodeIdx > 0) {
      outstream << ", ";
    }
    outstream << "[";
    for (size_t i = 0; i < current->size_; ++i) {
      if (i != 0) outstream << ", ";
      outstream << current->data_[i];
    }
    outstream << "]";
  }

  outstream << "}";
  return outstream.str();
}

ULL_IMPL(bool) empty() noexcept { return size_ == 0; }

/// Ullist private methods

ULL_TEMPL_IMPL(Node*) popData() {
  Node* temp = cache_;
  cache_ = cache_->next_;
  temp->next_ = nullptr;
  return temp;
}

ULL_IMPL(void) pushData(ullist::Node* _node) {
  if (cache_) {
    _node->next_ = cache_;
  }
  cache_ = _node;
}

ULL_TEMPL_IMPL(Node*) newNode() { return cache_ ? popData() : new Node; }

ULL_TEMPL_IMPL(Node*) appendNewTailNode() {
  ++nodeCount_;
  Node* append = newNode();
  if (!tail_) {
    return head_ = tail_ = append;
  } else {
    link(tail_, append);
    return tail_ = append;
  }
}

ULL_TEMPL_IMPL(Node*) appendNewTailNode(Node* _newTail) {
  ++nodeCount_;
  if (!tail_) {
    return head_ = tail_ = _newTail;
  } else {
    link(tail_, _newTail);
    return tail_ = _newTail;
  }
}

ULL_TEMPL_IMPL(Node*) appendNewHeadNode() {
  ++nodeCount_;
  Node* append = newNode();
  if (!head_) {
    return head_ = tail_ = append;
  } else {
    link(append, head_);
    return head_ = append;
  }
}

ULL_TEMPL_IMPL(Node*) appendNewHeadNode(Node* _newHead) {
  ++nodeCount_;
  if (!head_) {
    return head_ = tail_ = _newHead;
  } else {
    link(_newHead, head_);
    return head_ = _newHead;
  }
}

ULL_IMPL(void) deleteNode(Node* _node) {
  if (_node->prev_ && _node->next_) {
    link(_node->prev_, _node->next_);
    _node->prev_ = _node->next_ = nullptr;
  } else {
    if (tail_ == _node) {
      tail_ = tail_->prev_;
    }
    if (head_ == _node) {
      head_ = head_->next_;
    }

    if (_node->next_) {
      unlink(_node, _node->next_);
    }
    if (_node->prev_) {
      unlink(_node->prev_, _node);
    }
  }

  --nodeCount_;
  size_ -= _node->size_;

  if (cache_) {
    delete _node;
  }
  else {
    _node->clear();
    pushData(_node);
  }
}

ULL_IMPL(void)
at(size_t _i, Node*& _inputNode, size_t& offset) const {
  Node* current = head_;
  size_t i = 0;
  while (current) {
    if (i + current->size_ > _i || _i < nodeSize) {
      break;
    }
    i += current->size_;
    current = current->next_;
  }
  _inputNode = current;
  offset = _i - i;
}

ULL_IMPL(bool) isNodeFull(ullist::Node*_node) {
  return _node == nullptr || _node->size_ == nodeSize;
  ;
}

ULL_IMPL(bool) isNodeEmpty(Node*_node) {
  return _node != nullptr && _node->size_ == 0;
}

ULL_IMPL(void) link(Node* _left, Node* _right) {
  _left->next_ = _right;
  _right->prev_ = _left;
}

ULL_IMPL(void) unlink(Node* _left, Node*_right) {
  _left->next_ = nullptr;
  _right->prev_ = nullptr;
}

/// Node implementation

ULL_NODE Node() : prev_(nullptr), next_(nullptr), size_(0u) {
  data_ = allocator_.allocate(nodeSize);
  for (size_t i = 0; i < nodeSize; ++i) {
    data_[i] = 0;
  }
}

ULL_NODE Node(Node* _prev, Node*_next) : prev_(_prev), next_(_next), size_(0u) {
  data_ = allocator_.allocate(nodeSize);
  _prev->prev_ = this;
  _next->next_ = this;
}

ULL_NODE ~Node() {
  if (data_) {
    allocator_.deallocate(data_, nodeSize);
    data_ = nullptr;
  }
}

ULL_NODE_IMPL(void) clear() {
  if (std::is_destructible<value_type>::value) {
    allocator_.deallocate(data_, nodeSize);
  }
  size_ = 0;
}

ULL_NODE_IMPL(void) push_back(const_reference value) {
  if (size_ + 1 <= nodeSize) {
    data_[size_++] = value;
  } else {
    throw std::overflow_error("ullist_node::ullist node is full");
  }
}

ULL_NODE_TEMPL_IMPL(const_reference) back() const { return back(); }

ULL_NODE_TEMPL_IMPL(const_reference) front() const { return front(); }

ULL_NODE_TEMPL_IMPL(reference) back() {
  if (data_) {
    return data_[size_ - 1];
  }
  throw std::out_of_range("ullist_node::back called on empty ullist node");
}

ULL_NODE_TEMPL_IMPL(reference) front() {
  if (data_) {
    return data_[0];
  }
  throw std::out_of_range("ullist_node::front called on empty ullist node");
}

ULL_NODE_IMPL(void) pop_back() {
  if (data_) {
    data_[--size_] = 0;
  }
}

ULL_NODE_IMPL(void) insert(size_t i, const_reference _value) {
  if (i >= size_) {
    push_back(_value);
  } else {
    std::move(data_ + i, data_ + size_, data_ + i + 1);
    data_[i] = _value;
    ++size_;
  }
}

ULL_NODE_IMPL(value_type) remove(size_t _i) {
  if (_i >= size_ - 1) {
    return back();
  } else {
    value_type oldData =
        std::is_move_assignable<value_type>() ? std::move(data_[_i]) : data_[_i];
    std::move(data_ + _i + 1, data_ + size_, data_ + _i);
    --size_;
    return oldData;
  }
}

ULL_NODE_TEMPL_IMPL(reference) operator[](size_t _i) { return data_[_i]; }

ULL_NODE_TEMPL_IMPL(const_reference) operator[](size_t _i) const {
  return data_[_i];
}

}  // namespace ull

/*
  ULL_TEMPL_IMPL(Node*) nodeAtIndex(size_t _i) {
  if (isInBounds(static_cast<size_t>(0), size_, _i)) {
    Node* current = head_;
    size_t i = _i;

    while (true) {
      size_t check = i - current->size_;
      if (check <= 0) {
        return current;
      }
      i = check;
    }
  }
}*/

/*
 ULL_TEMPL_IMPL(Node*) insertNewNodeAt(size_t i) {
  ++nodeCount_;
  if (nodeCount_ == 0) {
    return head_ = tail_ = newNode();
  } else if (i == 0) {
    return appendNewHeadNode();
  } else if (i >= nodeCount_) {
    return appendNewTailNode();
  } else {
    Node* target = nodeAt(i);
    Node* insert = newNode();
    link(insert, insert->next_);
    link(insert->prev_, insert);
    return insert;
  }
}*/

/*ULL_TEMPL_IMPL(Node*) nodeAt(size_t _i) {
  if (isInBounds(static_cast<size_t>(0), nodeCount_, _i)) {
    Node* current = head_;
    for (size_t i = 0; i < _i; ++i) {
      current = current->next_;
    }
    return current;
  }
}*/
#endif  // UNROLLED_LINKED_LIST_ULLIST_HPP
