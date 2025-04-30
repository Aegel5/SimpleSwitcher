namespace Rand {

	inline std::default_random_engine rnd_engine{ std::random_device{}() };

	inline void reinit() { rnd_engine.seed(999); }

	namespace details {
		//std::uniform_int_distribution<uint> _uint_distrib;
		inline std::uniform_int_distribution<int> _int_distrib;
		inline auto _Integer(auto l, auto r) { return std::uniform_int_distribution<decltype(l)>(l, r)(rnd_engine); }
	}

	//auto Uint() { return _uint_distrib(rnd_engine); }
	inline auto Int() { return details::_int_distrib(rnd_engine); }
	//auto Uint(uint l, uint r) { return _Integer(l, r); }
	inline auto Int(int l, int r) { return details::_Integer(l, r); }
	//auto Ulong(ulong l, ulong r) { return _Integer(l, r); }

	inline auto Double() {
		return std::uniform_real_distribution<double>()(rnd_engine);
	}
}
