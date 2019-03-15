#pragma once
#include "EmulatorComponent.h"
#include "PlayerData.h"
#include "CustomPlayerData.h"
#include <string>

namespace DivaHook::Components
{
	class PlayerDataManager : public EmulatorComponent
	{
	public:
		PlayerDataManager();
		~PlayerDataManager();

		virtual const char* GetDisplayName() override;

		virtual void Initialize() override;
		virtual void Update() override;
		CustomPlayerData* customPlayerData;
		PlayerData* playerData;
		int ModuleEquip1 = 0;
		int ModuleEquip2 = 0;
		int SkinEquip = 0;
		int BtnSeEquip = 0;

	private:
		

		void LoadConfig();
	};
}

