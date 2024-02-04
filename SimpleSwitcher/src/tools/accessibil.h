
inline void AllowAccessibilityShortcutKeys(bool bAllowKeys) {

     STICKYKEYS g_StartupStickyKeys = {sizeof(STICKYKEYS), 0};
     TOGGLEKEYS g_StartupToggleKeys = {sizeof(TOGGLEKEYS), 0};
     FILTERKEYS g_StartupFilterKeys = {sizeof(FILTERKEYS), 0};
    
     SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &g_StartupStickyKeys, 0);
     SystemParametersInfo(SPI_GETTOGGLEKEYS, sizeof(TOGGLEKEYS), &g_StartupToggleKeys, 0);
     SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &g_StartupFilterKeys, 0);

    if (bAllowKeys) {
        // Restore StickyKeys/etc to original state and enable Windows key
        //STICKYKEYS sk = g_StartupStickyKeys;
        //TOGGLEKEYS tk = g_StartupToggleKeys;
        //FILTERKEYS fk = g_StartupFilterKeys;

        //SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &g_StartupStickyKeys, 0);
        //SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &g_StartupToggleKeys, 0);
        //SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &g_StartupFilterKeys, 0);
    } else {
        // Disable StickyKeys/etc shortcuts but if the accessibility feature is on,
        // then leave the settings alone as its probably being usefully used

        STICKYKEYS skOff = g_StartupStickyKeys;
        if ((skOff.dwFlags & SKF_STICKYKEYSON) == 0) {
            // Disable the hotkey and the confirmation
            skOff.dwFlags &= ~SKF_HOTKEYACTIVE;
            skOff.dwFlags &= ~SKF_CONFIRMHOTKEY;

            SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &skOff, 0);
        }

        TOGGLEKEYS tkOff = g_StartupToggleKeys;
        if ((tkOff.dwFlags & TKF_TOGGLEKEYSON) == 0) {
            // Disable the hotkey and the confirmation
            tkOff.dwFlags &= ~TKF_HOTKEYACTIVE;
            tkOff.dwFlags &= ~TKF_CONFIRMHOTKEY;

            SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), &tkOff, 0);
        }

        FILTERKEYS fkOff = g_StartupFilterKeys;
        if ((fkOff.dwFlags & FKF_FILTERKEYSON) == 0) {
            // Disable the hotkey and the confirmation
            fkOff.dwFlags &= ~FKF_HOTKEYACTIVE;
            fkOff.dwFlags &= ~FKF_CONFIRMHOTKEY;

            SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fkOff, 0);
        }
    }
}