#pragma once

template<class T>
class CComPtr {
public:
	CComPtr() = default;
	CComPtr(T* ptr): m_Ptr(ptr) {
	}
	CComPtr(const CComPtr&) = delete;
	CComPtr(CComPtr&&)noexcept = delete;
	~CComPtr() {
		if(m_Ptr) {
			delete m_Ptr;
			m_Ptr = nullptr;
		}
	}
	T* operator->() {
		return m_Ptr;
	}

	T** operator&() {
		return &m_Ptr;
	}

	operator bool() const {
		return !!m_Ptr;
	}
private:
	T* m_Ptr;
};
