#pragma once
#include "../Concurrency/Concurrency.h"
template<typename T>
class Singleton {
private:
	static T* s_Instance;
	static Mutex s_Mutex;
	Singleton(const Singleton<T>&) = delete;
	Singleton(Singleton<T>&&) = delete;
	Singleton() = default;
	~Singleton() { Release(); }
public:

	template <typename...Args>
	static void Initialize(Args...args) {
		MutexLock lock(s_Mutex);
		if(nullptr != s_Instance) {
			delete s_Instance;
		}
		s_Instance = new T(args...);
	}
	static void Release() {
		if(nullptr != s_Instance) {
			delete s_Instance;
		}
	}
	static T* Instance() {
		return s_Instance;
	}
};

template<typename T> T* Singleton<T>::s_Instance{ nullptr };
template<typename T> Mutex Singleton<T>::s_Mutex{};