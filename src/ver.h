namespace details {
	static const char* SW_VERSION = "6.7.0a";
}

inline const char* GET_SW_VERSION() {
#ifdef PUBLIC_RELEASE
	return details::SW_VERSION;
#endif
	static std::string ver{ details::SW_VERSION + "_U"s };
	return ver.c_str();
}
