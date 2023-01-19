#pragma once
#include <memory>

template <class T, class Dx=std::default_delete<T>>
using TUniquePtr = std::unique_ptr<T, Dx>;

template <class T>
using TSharedPtr = std::shared_ptr<T>;

template <class T>
using TWeakPtr = std::weak_ptr<T>;
