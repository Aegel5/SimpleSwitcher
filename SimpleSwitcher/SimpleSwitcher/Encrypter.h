#pragma once

#include "dpapi.h"

class Encrypter {
public:
    static TStatus Encrypt(TKeyHookInfo& key) {
		static_assert(sizeof(key.crypted) == CRYPTPROTECTMEMORY_BLOCK_SIZE);
		key.crypted._random_data = GetTickCount64(); // заполняем случайными данными чтобы одна и та же буква каждый раз шифровалась по разному
		IFW_RET(CryptProtectMemory(&key, CRYPTPROTECTMEMORY_BLOCK_SIZE, CRYPTPROTECTMEMORY_SAME_PROCESS));
		RETURN_SUCCESS;
    }
    static TStatus Decrypt(TKeyHookInfo& key) {
		IFW_RET(CryptUnprotectMemory(&key, CRYPTPROTECTMEMORY_BLOCK_SIZE, CRYPTPROTECTMEMORY_SAME_PROCESS));
		RETURN_SUCCESS;
    }
};