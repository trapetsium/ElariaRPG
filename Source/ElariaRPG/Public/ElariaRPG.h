#pragma once
#include "CoreMinimal.h"

// Старую категорию возвращаем временно, чтобы не блокировать сборку
DECLARE_LOG_CATEGORY_EXTERN(LogElaria, Log, All);

// Новые категории (для шутера)
DECLARE_LOG_CATEGORY_EXTERN(LogCombat, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogWeapons, Log, All);