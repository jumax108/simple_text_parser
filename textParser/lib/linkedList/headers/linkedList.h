#pragma once

#include "dump/headers/dump.h"
#pragma comment(lib, "lib/dump/dump")

#include "objectFreeList/headers/objectFreeList.h"

template<typename T>
class CLinkedList {

	struct stNode {

	public:
		inline stNode* operator=(const stNode& node);
		inline bool operator==(const stNode& node);
		inline bool operator!=(const stNode& node);

	private:
		T _value;
		stNode* _parent;
		stNode* _child;
		friend class CIterator;
		friend class CLinkedList;
	};
public:
	class CIterator {
	public:
		CIterator();
		CIterator(const stNode* node);

		inline CIterator& operator=(const stNode& node);
		inline T& operator*();

		inline CIterator& operator++();
		inline CIterator& operator--();

		inline bool operator!=(const stNode* node);
		inline bool operator==(const stNode* node);

	private:
		stNode* _node;
		friend class CLinkedList;
	};

public:


	CLinkedList();

	void push_front(
		const T value // 저장할 값
	);
	void push_back(
		const T value // 저장할 값
	);

	void insert(
		const T value, // 값을 저장할 데이터
		stNode* parent  // 이 노드의 자식으로 저장됩니다.
	);

	CIterator& erase(
		CIterator& iter // 지울 노드를 전달합니다.
	);

	void clear();

	inline stNode* begin() {
		return _head._child;
	}

	inline stNode* end() {
		return &_tail;
	}

	~CLinkedList();

private:
	stNode _head;
	stNode _tail;
	CObjectFreeList<stNode> _nodeFreeList;
	

};

template<typename T>
CLinkedList<T>::CLinkedList(): _nodeFreeList(false, false){
	
	_head._parent = nullptr;
	_head._child = &_tail;

	_tail._parent = &_head;
	_tail._child = nullptr;
}

template<typename T>
CLinkedList<T>::~CLinkedList() {
	clear();
}

template<typename T>
void CLinkedList<T>::insert(T value, CLinkedList<T>::stNode* parent) {

	//stNode* node = (CLinkedList<T>::stNode*)malloc(sizeof(CLinkedList<T>::stNode));
	stNode* node = _nodeFreeList.allocObject();

	node->_value = value;
	node->_parent = parent;

	parent->_child->_parent = node;
	node->_child = parent->_child;
	parent->_child = node;
	
}

template<typename T>
void CLinkedList<T>::push_front(const T value) {
	//stNode* node = (CLinkedList<T>::stNode*)malloc(sizeof(CLinkedList<T>::stNode));
	stNode* node = _nodeFreeList.allocObject();

	node->_value = value;

	node->_parent = &_head;
	node->_child = _head._child;

	_head._child->_parent = node;
	_head._child = node;
}

template<typename T>
void CLinkedList<T>::push_back(const T value) {
	//stNode* node = (CLinkedList<T>::stNode*)malloc(sizeof(CLinkedList<T>::stNode));
	stNode* node = _nodeFreeList.allocObject();

	node->_value = value;
	node->_parent = _tail._parent;
	node->_child = &_tail;

	_tail._parent->_child = node;
	_tail._parent = node;
}

template<typename T>
void CLinkedList<T>::clear() {
	for (CLinkedList<T>::CIterator iter = begin(); iter != end(); ) {
		iter = erase(iter);
	}
}

template<typename T>
typename CLinkedList<T>::CIterator& CLinkedList<T>::erase(
	CIterator& iter // 지울 노드를 전달합니다.
) {

	CLinkedList<T>::stNode* node = iter._node;
	CLinkedList<T>::stNode* child = node->_child;

	node->_child->_parent = node->_parent;
	node->_parent->_child = node->_child;

	CIterator nextIter = ++iter;

	//free(node);
	_nodeFreeList.freeObject(node);

	return nextIter;
}

template <typename T>
typename CLinkedList<T>::stNode* CLinkedList<T>::stNode::operator=(const stNode& node){
	_value	= node._value	;
	_parent = node._parent	;
	_child	= node._child	;

	return this;
}

template <typename T>
bool CLinkedList<T>::stNode::operator==(const stNode& node) {
	return	memcmp(&_value, &node._value, sizeof(T)) == 0 &&
			_parent == node._parent &&
			_child  == node._child;
}

template <typename T>
bool CLinkedList<T>::stNode::operator!=(const stNode& node) {
	return	!(memcmp(&_value, &node._value, sizeof(T)) == 0 &&
			_parent == node._parent &&
			_child	== node._child);
}

template <typename T>
CLinkedList<T>::CIterator::CIterator() {
	_node = nullptr;
}

template <typename T>
CLinkedList<T>::CIterator::CIterator(const stNode* node) {
	_node = (stNode*)node;
}

template <typename T>
typename CLinkedList<T>::CIterator& CLinkedList<T>::CIterator::operator=(const stNode& node) {
	_node->operator=(node);
	return this;
}

template <typename T>
T& CLinkedList<T>::CIterator::operator*() {
	return _node->_value;
}

template <typename T>
typename CLinkedList<T>::CIterator& CLinkedList<T>::CIterator::operator++() {
	_node = _node->_child;
	return *this;
}

template <typename T>
typename CLinkedList<T>::CIterator& CLinkedList<T>::CIterator::operator--() {
	_node = _node->_parent;
	return *this;
}

template <typename T>
bool CLinkedList<T>::CIterator::operator!=(const stNode* node) {
	return *(this->_node) != *node;
}

template <typename T>
bool CLinkedList<T>::CIterator::operator==(const stNode* node) {
	return *(this->_node) == *node;
}