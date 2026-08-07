// Простой класс для реализации синглетонов с контролируемым временем жизни.
#pragma once

namespace details {
	template <typename T>
	struct Singleton {
		inline static T* ptr = nullptr;
		Singleton(T* instance) {
			assert(!Singleton<T>::ptr && "Duplicate singleton creation!");
			ptr = instance;
		}
		~Singleton() { ptr = nullptr; }
	};
}

#define ENABLE_SINGLETON(Type) \
public: \
    static Type* inst() { return details::Singleton<Type>::ptr; } \
    /* Запрещаем копирование и перемещение */ \
private: \
    details::Singleton<Type> life_guard_{this}

//Type(const Type&) = delete; \
    //Type& operator=(const Type&) = delete; \
    //Type(Type&&) = delete; \
    //Type& operator=(Type&&) = delete; \


