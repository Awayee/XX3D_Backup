#pragma once
template<typename T>
class TArray {
private:
	unsigned int size;
	T* data;
#define _Free if(data) delete[] data;
#define _Allocate data = new T[size];
public:
	TArray() {
		data = nullptr;
		size = 0;
	}
	TArray(const TArray<T>& other) {
		if (this != &other) {
			_Free();
			size = other.size;
			if (size > 0) {
				_Allocate();
				memcpy(data, other.data, sizeof(T) * size);
			}
			else {
				data = nullptr;
			}
		}
	}
	TArray(TArray<T>&& other) {
		data = other.data;
		size = other.size;
		other.data = nullptr;
		other.size = 0u;
	}

	TArray(unsigned int l) {
		size = l;
		_Allocate();
	}

	TArray(std::initializer_list<T> p) {
		size = p.size();
		_Allocate();
		auto begin = p.begin();
		for (uint32_t i = 0; i < size; i++) {
			data[i] = *(begin + i);
		}
	}
	TArray(T* other, unsigned int l) {
		size = l;
		if (size > 0) {
			_Allocate();
			memcpy(data, other, sizeof(T) * size);
		}
		else {
			data = nullptr;
		}
	}
	~TArray() {
		Clear();
	}

	unsigned int Size()const {
		return size;
	}

	T* Data()const {
		return data;
	}

	void Clear() {
		if (data != nullptr) {
			_Free();
		}
		size = 0;
	}

	void Swap(TArray<T>& other) {
		T* tempData = data;
		unsigned int tempSize = size;
		data = other.data;
		size = other.size;
		other.data = tempData;
		other.size = size;
	}

	T* begin()const {
		return data;
	}

	T* end()const {
		return data + size;
	}

	T& operator[](int i)const {
		return data[i];
	}

	void Resize(unsigned int l) {
		if (size != l) {
			_Free();

			size = l;
			if (l == 0) {
				data = nullptr;
			}
			else {
				_Allocate();

			}
		}
	}

	TArray<T>& operator=(const TArray<T>& other) {
		if (this != &other) {
			_Free();
			size = other.size;
			if (size > 0) {
				_Allocate();
				memcpy(data, other.data, sizeof(T) * size);
			}
			else {
				data = nullptr;
			}
		}
		return *this;
	}
};