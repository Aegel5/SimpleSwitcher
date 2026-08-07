

template <class T>
class MoveZero {
public:
	MoveZero() noexcept = default;
	MoveZero(T val) noexcept : value_(val) {}

	// Копирование разрешено
	MoveZero(const MoveZero&) noexcept = default;
	MoveZero& operator=(const MoveZero&) noexcept = default;

	// Конструктор перемещения (зануляет источник)
	MoveZero(MoveZero&& other) noexcept
		: value_(std::exchange(other.value_, T{})) {
	}

	// Оператор перемещения (зануляет источник)
	MoveZero& operator=(MoveZero&& other) noexcept {
		if (this != &other) {
			value_ = std::exchange(other.value_, T{});
		}
		return *this;
	}

	// Деструктор
	~MoveZero() = default;

	// Неявное преобразование в базовый тип для удобства
	operator T() const noexcept { return value_; }

	T& ref() noexcept { return value_; }

private:
	T value_{};
};

