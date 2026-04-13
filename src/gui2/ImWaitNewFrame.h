inline bool ImWaitNewFrame() {
	using namespace std::chrono;

	if (_g_wantFrameDelay <= 0) {
		// simple check messages
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return false;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg); // Might trigger UI changes and update _g_wantFrameDelay
		}
	}
	else {
		float toWaitSeconds = _g_wantFrameDelay;
		do {
			auto start = steady_clock::now();

			// Wait for OS events or timeout (MsgWaitForMultipleObjectsEx uses milliseconds)
			DWORD res = ::MsgWaitForMultipleObjectsEx(
				0, NULL,
				static_cast<DWORD>(toWaitSeconds * 1000.0f),
				QS_ALLINPUT,
				MWMO_INPUTAVAILABLE | MWMO_ALERTABLE
			);

			if (res == WAIT_TIMEOUT) {
				break; // Wake up: requested delay has passed
			}

			// 2. MESSAGE PROCESSING: Handle Windows events
			MSG msg;
			while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
				if (msg.message == WM_QUIT) return false;
				::TranslateMessage(&msg);
				::DispatchMessage(&msg); // Might trigger UI changes and update _g_wantFrameDelay
			}

			// Adjust remaining wait time based on time spent processing messages
			// If DispatchMessage set a shorter delay, respect it
			duration<float> elapsed = steady_clock::now() - start;
			toWaitSeconds -= elapsed.count();
			if (toWaitSeconds > _g_wantFrameDelay) {
				// Try to reduce wait time, but not below the remaining minDelay threshol
				toWaitSeconds = _g_wantFrameDelay;
			}

		} while (toWaitSeconds > 0.001f);
	}

	// 3. BURST PHASE: Ensure a sequence of frames for UI consistency (inertia)
	{
		static int framesRemaining = 0;

		// Reset burst counter if we are starting a new interaction cycle
		if (framesRemaining <= 0) {
			framesRemaining = 3;
		}

		framesRemaining--;

		// If burst is active, force immediate next frame (0s delay).
		// Once finished, go into deep sleep (100s delay) until next event.
		_g_wantFrameDelay = (framesRemaining > 0) ? 0.0f : 200.0f;
	}

	return true;
}
