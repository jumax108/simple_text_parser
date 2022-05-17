#pragma once

#include <typeinfo>

#include "common.h"

#include "dump/headers/dump.h"
#pragma comment(lib, "lib/dump/dump")

#if defined(OBJECT_FREE_LIST_DEBUG)
	#include "log/headers/log.h"
	#pragma comment(lib, "lib/log/log")
#endif

#if defined(OBJECT_FREE_LIST_DEBUG)
	#define allocObject() _allocObject(__FILEW__, __LINE__)
	#define freeObject(x) _freeObject(x, __FILEW__, __LINE__)
#else
	#define allocObject() _allocObject()
	#define freeObject(x) _freeObject(x)
#endif

template<typename T>
struct stAllocNode {
	stAllocNode() {
		init();
	}
	~stAllocNode(){
	}

	void init(){
		
		_nextNode = nullptr;

		#if defined(OBJECT_FREE_LIST_DEBUG)
			_allocated = false;
			_underFlowCheck = (void*)0xF9F9F9F9F9F9F9F9;
			_overFlowCheck = (void*)0xF9F9F9F9F9F9F9F9;

			_allocSourceFileName = nullptr;
			_freeSourceFileName = nullptr;
			
			_allocLine = 0;
			_freeLine = 0;

			_allocated = false;
		#endif

		_callDestructor = false;
	}

	#if defined(OBJECT_FREE_LIST_DEBUG)
		// f9�� �ʱ�ȭ�ؼ� ����÷ο� üũ�մϴ�.
		void* _underFlowCheck;
	#endif

	// alloc �Լ����� ������ ���� ������
	T _data;
	
	#if defined(OBJECT_FREE_LIST_DEBUG)
		// f9�� �ʱ�ȭ�ؼ� �����÷ο� üũ�մϴ�.
		void* _overFlowCheck;
	#endif

	// �Ҵ��� ���� ���
	stAllocNode<T>* _nextNode;

	#if defined(OBJECT_FREE_LIST_DEBUG)
		// �ҽ� ���� �̸�
		const wchar_t* _allocSourceFileName;
		const wchar_t* _freeSourceFileName;

		// �ҽ� ����
		int _allocLine;
		int _freeLine;

		// ��尡 ��������� Ȯ��
		bool _allocated;
	#endif

	// �Ҵ� ���� �Ҹ��ڰ� ȣ��Ǿ����� ����
	// �Ҵ� ���� ����ڰ� ��ȯ�Ͽ� �Ҹ��ڰ� ȣ��Ǿ��ٸ�
	// Ŭ������ �Ҹ��� ��, �Ҹ��ڰ� ȣ��Ǹ� �ȵȴ�.
	bool _callDestructor;
};

template<typename T>
class CObjectFreeList
{
public:

	CObjectFreeList(bool runConstructor, bool runDestructor, int _capacity = 0);
	~CObjectFreeList();


	#if defined(OBJECT_FREE_LIST_DEBUG)
		T* _allocObject(const wchar_t*, int);
	#else
		T* _allocObject();
	#endif
	
	#if defined(OBJECT_FREE_LIST_DEBUG)
		int _freeObject(T* data, const wchar_t*, int);
	#else
		int _freeObject(T* data);
	#endif

	inline unsigned int getCapacity() { return _capacity; }
	inline unsigned int getUsedCount() { return _usedCnt; }

private:

	// �޸� �Ҵ�, ������ ���� ��
	HANDLE _heap;

	// ��� ������ ��带 ����Ʈ�� ���·� �����մϴ�.
	// �Ҵ��ϸ� �����մϴ�.
	stAllocNode<T>* _freeNode;

	// ��ü ��� ����
	unsigned int _capacity;

	// ���� �Ҵ�� ��� ����
	unsigned int _usedCnt;

	// �޸� ������
	// �ܼ� ����Ʈ
	struct stSimpleListNode {
		stAllocNode<T>* _ptr;
		stSimpleListNode* _next;
	};

	// freeList �Ҹ��ڿ��� �޸� ���������� ����մϴ�.
	// new�� �����͵�
	stSimpleListNode* _totalAllocList;

	// �Ҵ� ��, ������ ���� ���θ� ��Ÿ���ϴ�.
	bool _runConstructor;

	// ���� ��, �Ҹ��� ���� ���θ� ��Ÿ���ϴ�.
	bool _runDestructor;

	SRWLOCK _lock;

	#if defined(OBJECT_FREE_LIST_DEBUG)
		CLog log;
	#endif

};

template <typename T>
CObjectFreeList<T>::CObjectFreeList(bool runConstructor, bool runDestructor, int size) {
	
	_heap = HeapCreate(0, 0, 0);

	_freeNode = nullptr;
	_totalAllocList = nullptr;
	
	_capacity = size;
	_usedCnt = 0;

	_runConstructor = runConstructor;
	_runDestructor = runDestructor;
	
	#if defined(OBJECT_FREE_LIST_DEBUG)
		log.setDirectory(L"objectFreeList_Log");
		log.setPrintGroup(LOG_GROUP::LOG_DEBUG);
	#endif

	if (size == 0) {
		return;
	}

	for(int nodeCnt = 0; nodeCnt < size; ++nodeCnt){

		// �̸� �������� ������ŭ ��带 ����� ����
		stAllocNode<T>* newNode = (stAllocNode<T>*)HeapAlloc(_heap, 0, sizeof(stAllocNode<T>));

		// T type�� ���� ������ ȣ�� ���θ� ����
		if(runConstructor == false) {
			new (newNode) stAllocNode<T>;
		} else {
			newNode->init();
		}

		newNode->_nextNode = _freeNode;
		_freeNode = newNode;

		{
			// ��ü alloc list�� �߰�
			// �Ҹ��ڿ��� �ϰ������� �޸� �����ϱ� ����
			stSimpleListNode* totalAllocNode = (stSimpleListNode*)HeapAlloc(_heap, 0, sizeof(stSimpleListNode));

			totalAllocNode->_ptr = newNode;
			totalAllocNode->_next = _totalAllocList;

			_totalAllocList = totalAllocNode;

		}

	}


}

template <typename T>
CObjectFreeList<T>::~CObjectFreeList() {
	
	#if defined(OBJECT_FREE_LIST_DEBUG)
		log(L"memoryLeak.txt", LOG_GROUP::LOG_DEBUG, L"����������������������������������������������������������������");
		wchar_t typeName[50];
		size_t convertSize;
		mbstowcs_s(&convertSize, typeName, typeid(T).name(), 50);
		log(L"memoryLeak.txt", LOG_GROUP::LOG_DEBUG, L"type: %s", typeName);
	#endif

	while(_totalAllocList != nullptr){
		stSimpleListNode* allocListNode = _totalAllocList;
		stAllocNode<T>* freeNode = allocListNode->_ptr;

		#if defined(OBJECT_FREE_LIST_DEBUG)
			if(freeNode->_allocated == true){
				
				log(L"memoryLeak.txt", LOG_GROUP::LOG_DEBUG, L"alloc file name: %s", freeNode->_allocSourceFileName);
				log(L"memoryLeak.txt", LOG_GROUP::LOG_DEBUG, L"alloc line: %d", freeNode->_allocLine);
				log(L"memoryLeak.txt", LOG_GROUP::LOG_DEBUG, L"data(%d bytes): ", sizeof(T));

				wchar_t byteLine[25];
				wchar_t* lineWritePoint = byteLine;
				for(int byteCnt = 0; byteCnt < sizeof(T); byteCnt++){

					swprintf_s(lineWritePoint, 4, L"%02X ", *( ((unsigned char*)(&freeNode->_data) + byteCnt)) );
					lineWritePoint += 3;

					if((byteCnt+1) % 8 == 0 || byteCnt+1 == sizeof(T)){
						log(L"memoryLeak.txt", LOG_GROUP::LOG_DEBUG, L"%s", byteLine);
						lineWritePoint = byteLine;
					}
				}

				log(L"memoryLeak.txt", LOG_GROUP::LOG_DEBUG, L"");

			}
		#endif

		if(freeNode->_callDestructor == false){
			freeNode->~stAllocNode();
		}
		HeapFree(_heap, 0, freeNode);
		_totalAllocList = allocListNode->_next;
		HeapFree(_heap, 0, allocListNode);
	}
	
	#if defined(OBJECT_FREE_LIST_DEBUG)
		log(L"memoryLeak.txt", LOG_GROUP::LOG_DEBUG, L"����������������������������������������������������������������");
	#endif

	HeapDestroy(_heap);

}

template<typename T>
T* CObjectFreeList<T>::_allocObject(
	#if defined(OBJECT_FREE_LIST_DEBUG)
		const wchar_t* fileName, int line
	#endif
) {
	
	T* data;

	AcquireSRWLockExclusive(&_lock); {
					
		stAllocNode<T>* allocNode;

		do {

			// ��尡 ������ ���� �Ҵ�
			if(_freeNode == nullptr){
			
				allocNode = (stAllocNode<T>*)HeapAlloc(_heap, 0, sizeof(stAllocNode<T>));
				stSimpleListNode* totalAllocNode = (stSimpleListNode*)HeapAlloc(_heap, 0, sizeof(stSimpleListNode));

				totalAllocNode->_ptr = allocNode;
				totalAllocNode->_next = _totalAllocList;
				_totalAllocList = totalAllocNode;

				_capacity += 1;

				if (_runConstructor == false) {
					new (allocNode) stAllocNode<T>;
				}
				else {
					allocNode->init();
				}

				break;
			
			}

			// ��尡 ������ free node�� ����
			allocNode = _freeNode;
			_freeNode = _freeNode->_nextNode;

		} while (false);

		_usedCnt += 1;
	
		// �Ҹ��� ȣ�� ���� �ʱ�ȭ
		allocNode->_callDestructor = false;

		// debug ���
		#if defined(OBJECT_FREE_LIST_DEBUG)
			// ��带 ��������� üũ��
			allocNode->_allocated = true;

			// �Ҵ� ��û�� �ҽ����ϰ� �ҽ������� �����
			allocNode->_allocSourceFileName = fileName;
			allocNode->_allocLine = line;
		#endif
	
		data = &allocNode->_data;

		// ������ ����
		if(_runConstructor == true){
			new (data) T();
		}
	
	} ReleaseSRWLockExclusive(&_lock);

	return data;
}

template <typename T>
int CObjectFreeList<T>::_freeObject(T* data	
	#if defined(OBJECT_FREE_LIST_DEBUG)
		, const wchar_t* fileName, int line
	#endif
) {

	AcquireSRWLockExclusive(&_lock); {

		stAllocNode<T>* usedNode = (stAllocNode<T>*)(((char*)data) + objectFreeList_stack_lock::DATA_TO_NODE_PTR);
		
		// overflow üũ �� debug ���
		#if defined(OBJECT_FREE_LIST_DEBUG)
			// �ߺ� free üũ
			if(usedNode->_allocated == false){
				CDump::crash();
			}

			// �����÷ο� üũ
			if((unsigned __int64)usedNode->_overFlowCheck != 0xF9F9F9F9F9F9F9F9){
				CDump::crash();
			}

			// ����÷ο� üũ
			if((unsigned __int64)usedNode->_underFlowCheck != 0xF9F9F9F9F9F9F9F9){
				CDump::crash();
			}

			// ����� ����� �÷��׸� ����
			usedNode->_allocated = false;
		#endif

		// �Ҹ��� ����
		if(_runDestructor == true){
			data->~T();
			usedNode->_callDestructor = true;
		}

		// ���� ���ÿ� ����
		usedNode->_nextNode = _freeNode;
		_freeNode = usedNode;

		// ��� ī���� ����
		_usedCnt -= 1;

	} ReleaseSRWLockExclusive(&_lock);

	return 0;

}