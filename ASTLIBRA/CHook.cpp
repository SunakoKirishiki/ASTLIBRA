#include "CHook.h"

CHook::CHook(HANDLE hGameProcess, QWORD qwBaseAddr,QWORD qwVitualAddr)
{
	m_hGameProcess = hGameProcess;
	m_qwBaseAddr= qwBaseAddr;
	m_qwVirtualAddr= qwVitualAddr;
	//读取配置文件
	try
	{
		Config configSettings(ConfigFile);
		m_nGold = configSettings.Read<int>("gold");
		m_nSpar = configSettings.Read<int>("spar");
		m_nExp = configSettings.Read<int>("player_exp");
		m_nEExp =configSettings.Read<int>("equip_exp");
		m_nStrengthen = configSettings.Read<int>("equip_strengthen");
		m_fChestProbability = configSettings.Read<float>("chest_probability");
		m_nChestType = configSettings.Read<int>("chest_type");
		m_nNewChest = configSettings.Read<int>("chest_new");
		m_nMaterial = configSettings.Read<int>("material");
		m_nLove= configSettings.Read<int>("love");
		m_nBoard = configSettings.Read<int>("board");
		m_nAdvancedBoard = configSettings.Read<int>("advanced_board");
		m_nFavorability = configSettings.Read<int>("favorability");
	
	}
	catch (const Config::File_not_found& e)
	{
		std::cout << "ERROR:配置文件"<<e.filename<<"不存在, 请将配置文件放到当前目录下!\r\n按任意键退出" << std::endl;
		_getch();
		ExitProcess(0);
		
	}
	catch (const Config::Key_not_found& e)
	{
		std::cout << "ERROR:配置文件中" << e.key << "不存在, 请检查配置文件!\r\n按任意键退出" << std::endl;
		_getch();
		ExitProcess(0);		
	}
	//获取重复HOOK信息
	isFirstHook();
	//特征匹配
	if (m_nGold != -1)
	{

		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigGold, sizeof(sigGold), 0xD, 0, 0);
		assert(addr.address);
		if (GoldHook(addr.address))
		{
			std::cout << "金币HOOK成功:" << std::dec << m_nGold << "倍" << std::endl;
		}
		else
		{
			std::cout << "金币HOOK失败" << std::endl;
		}

	}
	if (m_nExp != -1)
	{
		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigExp, sizeof(sigExp), 0x5E, 0, 0);
		assert(addr.address);		
		if (ExpHook(addr.address))
		{
			std::cout << "人物经验HOOK成功:" << std::dec << m_nExp << "倍" << std::endl;
		}
		else
		{
			std::cout << "人物经验HOOK失败" << std::endl;
		}
		
	}
	
	if (m_nSpar != -1)
	{

		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigSpar, sizeof(sigSpar), 0x19, 1, 0);
		assert(addr.address);
		if (SparHook(addr.address))
		{
			std::cout << "晶体HOOK成功:" << std::dec << m_nSpar << "倍" << std::endl;
		}
		else
		{
			std::cout << "晶体HOOK失败" << std::endl;
		}

	}
	if (m_nEExp != -1)
	{

		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigEExp, sizeof(sigEExp), 0x43, 1, 0);
		assert(addr.address);
		if (EExpHook(addr.address))
		{
			std::cout << "装备经验HOOK成功:" << std::dec << m_nEExp << "倍" << std::endl;
		}
		else
		{
			std::cout << "装备经验HOOK失败" << std::endl;
		}

	}
	if (m_fChestProbability != -1.0f)
	{

		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigChestP, sizeof(sigChestP), 0xAF, 0, 0);
		assert(addr.address);
		if (ChestProbabilityHook(addr.address))
		{
			std::cout << "箱子概率HOOK成功:" << m_fChestProbability << "%" << std::endl;
		}
		else
		{
			std::cout << "箱子概率HOOK失败" << std::endl;
		}

	}
	if (m_nChestType != -1)
	{

		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigChestType, sizeof(sigChestType), 0xA, 1, 0);
		assert(addr.address);
		if (ChestTypeHook(addr.address))
		{
			std::cout << "箱子类型HOOK成功:";
			switch (m_nChestType)
			{
			case 0:
				std::cout << "随机出箱子" << std::endl;
				break;
			case 1:
				std::cout << "必出木箱子" << std::endl;
				break;
			case 2:
				std::cout << "必出铜箱子" << std::endl;
				break;
			case 3:
				std::cout << "必出银箱子" << std::endl;
				break;
			case 4:
				std::cout << "必出金箱子" << std::endl;
				break;
			default:
				break;
			}
		}
		else
		{
			std::cout << "箱子类型HOOK失败" << std::endl;
		}

	}
	if (m_nStrengthen != -1)
	{
		//先记录地址
		if (-1 == m_stHOOKDATA.m_Strengthen_1_Hook_Addr)
		{
			Base addr_1 = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigStrengthen_1, sizeof(sigStrengthen_1), 0x0, 0, 0);
			m_stHOOKDATA.m_Strengthen_1_Hook_Addr = addr_1.address;
			WriteHookData();
		}		
		Base addr_2 = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigStrengthen_2, sizeof(sigStrengthen_2), 0x50, 1, 0);

		if (Strengthen_1_Hook(m_stHOOKDATA.m_Strengthen_1_Hook_Addr)&& Strengthen_2_Hook(addr_2.address))
		{
			std::cout << "装备强化经验HOOK成功:" << std::dec << m_nStrengthen << "倍" << std::endl;
		}
		else
		{
			std::cout << "装备强化经验HOOK失败" << std::endl;
		}
	}
	if (m_nNewChest != -1)
	{
		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigChestP, sizeof(sigChestP), 0xBC, 0, 0);
		assert(addr.address);
		if (NewChestHook(addr.address))
		{
			std::cout << "新章箱子HOOK成功 模式:";
			if (m_nNewChest)
			{
				std::cout << "必出新章箱子" << std::endl;
			}
			else
			{
				std::cout << "随机出新章箱子" << std::endl;
			}
		}
		else
		{
			std::cout << "新章箱子HOOK失败" << std::endl;
		}
		
	}
	if (m_nMaterial != -1)
	{
		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigMaterial, sizeof(sigMaterial), 0x23, 1, 0);
		assert(addr.address);
		if (MaterialHook(addr.address))
		{
			std::cout << "素材掉落HOOK成功 模式:" << std::dec << m_nMaterial << std::endl;
		}
		else
		{
			std::cout << "素材掉落HOOK失败" << std::endl;
		}

	}
	if (m_nLove != -1)
	{
		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigLove, sizeof(sigLove), 0x1A, 0, 0);
		assert(addr.address);
		if (LoveHook(addr.address))
		{
			std::cout << "爱情掉落HOOK成功 模式:" << std::dec << m_nLove << std::endl;
		}
		else
		{
			std::cout << "爱情掉落HOOK失败" << std::endl;
		}

	}
	if (m_nBoard != -1)
	{
		Base addr_1 = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigBoard_1, sizeof(sigBoard_1), 0x20, 1, 0);
		Base addr_2 = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigBoard_2, sizeof(sigBoard_2), 0x16, 1, 0);
		assert(addr_1.address);
		assert(addr_2.address);
		if (Board_1_Hook(addr_1.address)&&Board_2_Hook(addr_2.address))
		{
			std::cout << "装备盘掉落HOOK成功: ";
			switch (m_nBoard)
			{
			case 0:
				std::cout << "随机出装备盘" << std::endl;
				break;
			case 1:
				std::cout << "出装备的武器装备盘" << std::endl;
				break;
			case 2:
				std::cout << "出装备的盾牌装备盘" << std::endl;
				break;
			case 3:
				std::cout << "出装备的护甲装备盘" << std::endl;
				break;
			case 4:
				std::cout << "出装备的第一格饰品装备盘" << std::endl;
				break;
			case 5:
				std::cout << "出装备的第二格饰品装备盘" << std::endl;
				break;
			case 6:
				std::cout << "出装备的第三格饰品装备盘" << std::endl;
				break;
			case 7:
				std::cout << "出装备的第四格饰品装备盘" << std::endl;
				break;
			case 8:
				std::cout << "出装备的第五格饰品装备盘" << std::endl;
				break;
			case 9:
				std::cout << "出装备的第六格饰品装备盘" << std::endl;
				break;
			case 10:
				std::cout << "出装备的第七格饰品装备盘" << std::endl;
				break;
			case 11:
				std::cout << "出装备的第八格饰品装备盘" << std::endl;
				break;
			default:
				break;
			}
		}
		else
		{
			std::cout << "装备盘掉落HOOK失败" << std::endl;
		}

	}
	if (m_nAdvancedBoard != -1)
	{
		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigAdvancedBoard, sizeof(sigAdvancedBoard), 0x3B, 1, 0);
		assert(addr.address);
		if (AdvancedBoardHook(addr.address))
		{
			std::cout << "高级装备盘掉落HOOK成功 模式:" << std::dec << m_nAdvancedBoard << std::endl;
		}
		else
		{
			std::cout << "高级装备盘掉落HOOK失败" << std::endl;
		}

	}
	if (m_nFavorability != -1)
	{
		Base addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigChestP, sizeof(sigChestP), 0xE7, 0, 0);
		assert(addr.address);
		if (FavorabilityHook(addr.address))
		{
			std::cout << "好感度HOOK成功:" << std::dec << m_nFavorability << "倍" << std::endl;
		}
		else
		{
			std::cout << "好感度HOOK失败" << std::endl;
		}

	}
}

bool CHook::GoldHook(QWORD addr)
{

	//在写入跳转之前先获取全局变量的偏移
	if (-1 == m_stHOOKDATA.m_rva_gold)  //这个位置第一次HOOK,特征匹配数据
	{		
		m_stHOOKDATA.m_rva_gold=GetHookData(addr, (LPBYTE)sigGold, sizeof(sigGold), 0x8, 0, 6);
		WriteHookData();
	}

	BYTE byBuff[] = { 0x48,0xBA,0x00,0x00,0xED,0x00,0x22,0x02,0x00,0x00,0xFF,0xD2,0x90};
	QWORD qwOffset = GetOffset(m_qwVirtualAddr, Gold);
	memcpy(byBuff + 2, &qwOffset, sizeof(qwOffset));
	bool ret1 = WriteMemory(addr, byBuff, sizeof(byBuff));

	BYTE byCodeBuff[] = { 0x48,0x69,0xC9,0x0D,0x00,0x00,0x00,0x48,0xBA,0xD0,0xDB,0x5B,0x6C,0xF6,0x7F,0x00,0x00,0x01,0x0A,0x01,0xC8,0x48,0x83,0xC4,0x08,0x0F,0x29,0x74,0x24,0x30,0x48,0x83,0xEC,0x08,0xC3 };
	memcpy(byCodeBuff + 3, &m_nGold, sizeof(m_nGold));

	//接下来要通过RVA计算全局变量的偏移	
	QWORD va1 = m_qwBaseAddr + m_stHOOKDATA.m_rva_gold;
	memcpy(byCodeBuff + 9, &va1, sizeof(va1));
	
	bool ret2 = WriteMemory(qwOffset, byCodeBuff, sizeof(byCodeBuff));

	return ret1 * ret2;
}

bool CHook::ExpHook(QWORD addr)
{
	//在写入跳转之前先获取全局变量的偏移
	if (-1 == m_stHOOKDATA.m_rva_exp_1)  //这个位置第一次HOOK,特征匹配数据
	{
		m_stHOOKDATA.m_rva_exp_1 = GetHookData(addr, (LPBYTE)sigExp, sizeof(sigExp), 0x55, 0, 10);
		WriteHookData();
	}
	if (-1 == m_stHOOKDATA.m_rva_exp_2)  //这个位置第一次HOOK,特征匹配数据
	{
		m_stHOOKDATA.m_rva_exp_2 = GetHookData(addr, (LPBYTE)sigExp, sizeof(sigExp), 0x4F, 0, 16);
		WriteHookData();
	}

	BYTE byBuff[] = { 0x48,0xB8,0x00,0x00,0xED,0x00,0x22,0x02,0x00,0x00,0xFF,0xD0,0x90,0x90,0x90,0x90 };
	QWORD qwOffset = GetOffset(m_qwVirtualAddr, Exp);
	memcpy(byBuff + 2, &qwOffset, sizeof(qwOffset));
	bool ret1 = WriteMemory(addr, byBuff, sizeof(byBuff));

	BYTE byCodeBuff[] = { 0xF3,0x0F,0x2C,0xCE,0x48,0x69,0xC9,0x10,0x27,0x00,0x00,0x48,0xB8,0xCC,0xDB,0x5B,0x6C,0xF6,0x7F,0x00,0x00,0x01,0x08,0x48,0xB8,0xD4,0x03,0x6A,0x6D,0xF6,0x7F,0x00,0x00,0x01,0x08,0xC3 };
	memcpy(byCodeBuff + 7, &m_nExp, sizeof(m_nExp));
	//接下来要通过RVA计算全局变量的偏移
	QWORD va1 = m_qwBaseAddr + m_stHOOKDATA.m_rva_exp_1;
	QWORD va2 = m_qwBaseAddr + m_stHOOKDATA.m_rva_exp_2;
	memcpy(byCodeBuff + 13, &va1, sizeof(va1));
	memcpy(byCodeBuff + 25, &va2, sizeof(va2));
	bool ret2 = WriteMemory(qwOffset, byCodeBuff, sizeof(byCodeBuff));

	return ret1*ret2;
}

bool CHook::SparHook(QWORD addr)
{
	BYTE byBuff[] = { 0x48,0xB8,0x00,0x00,0xED,0x00,0x22,0x02,0x00,0x00,0xFF,0xD0,0x90,0x90 };
	QWORD qwOffset = GetOffset(m_qwVirtualAddr, Spar);
	memcpy(byBuff + 2, &qwOffset, sizeof(qwOffset));
	bool ret1 = WriteMemory(addr, byBuff, sizeof(byBuff));

	BYTE byCodeBuff[] = { 0xB8,0x56,0x55,0x55,0x55,0xF7,0xE9,0x4D,0x8B,0xDE,0x45,0x0F,0xB6,0xC8,0x69,0xDB,0x00,0x00,0x01,0x00,0xC3 };
	memcpy(byCodeBuff + 16, &m_nSpar, sizeof(m_nSpar));

	bool ret2 = WriteMemory(qwOffset, byCodeBuff, sizeof(byCodeBuff));

	return ret1*ret2;
}

bool CHook::EExpHook(QWORD addr)
{
	BYTE byBuff[] = { 0x48,0xB8,0x00,0x00,0xED,0x00,0x22,0x02,0x00,0x00,0xFF,0xD0};
	QWORD qwOffset = GetOffset(m_qwVirtualAddr, EExp);
	memcpy(byBuff + 2, &qwOffset, sizeof(qwOffset));
	bool ret1 = WriteMemory(addr, byBuff, sizeof(byBuff));

	BYTE byCodeBuff[] = { 0xF3,0x0F,0x2C,0xD8,0x69,0xDB,0x00,0x00,0x01,0x00,0xC3 };
	int _EExp = m_nEExp * 1.5;
	memcpy(byCodeBuff + 6, &_EExp, sizeof(_EExp));

	
	bool ret2 = WriteMemory(qwOffset, byCodeBuff, sizeof(byCodeBuff));

	return ret1*ret2 ;
}

bool CHook::ChestProbabilityHook(QWORD addr)
{

	BYTE byBuff[] = { 0x90,0x48,0xB8,0x00,0x00,0xED,0x00,0x22,0x02,0x00,0x00,0xFF,0xD0,0xEB };
	QWORD qwOffset = GetOffset(m_qwVirtualAddr, ChestProbability);
	memcpy(byBuff + 3, &qwOffset, sizeof(qwOffset));
	bool ret1 = WriteMemory(addr, byBuff, sizeof(byBuff));

	BYTE byCodeBuff[] = { 0xB8,0x00,0x00,0x00,0x3F,0x66,0x0F,0x6E,0xC8,0xC3 };

	memcpy(byCodeBuff + 1, &m_fChestProbability, sizeof(m_fChestProbability));

	bool ret2 = WriteMemory(qwOffset, byCodeBuff, sizeof(byCodeBuff));


	return ret1 * ret2;
}

bool CHook::ChestTypeHook(QWORD addr)
{
	//在写入跳转之前先获取全局变量的偏移
	if (-1 == m_stHOOKDATA.m_rva_chesttype_1)  //这个位置第一次HOOK,特征匹配数据
	{
		m_stHOOKDATA.m_rva_chesttype_1 = GetHookData(addr, (LPBYTE)sigChestType, sizeof(sigChestType), 0xF, 1, 9);
		WriteHookData();
	}
	if (-1 == m_stHOOKDATA.m_rva_chesttype_2)  //这个位置第一次HOOK,特征匹配数据
	{
		m_stHOOKDATA.m_rva_chesttype_2 = GetHookData(addr, (LPBYTE)sigChestType, sizeof(sigChestType), 0x16, 1, 16);
		WriteHookData();
	}

	BYTE byBuff[] = { 0x48,0xB8,0x00,0x00,0xED,0x00,0x22,0x02,0x00,0x00,0xFF,0xD0,0x90,0x90,0x90,0x90 };
	QWORD qwOffset = GetOffset(m_qwVirtualAddr, ChestType);
	memcpy(byBuff + 2, &qwOffset, sizeof(qwOffset));
	bool ret1 = WriteMemory(addr, byBuff, sizeof(byBuff));

	BYTE byCodeBuff[] = { 0x48,0xB9,0x00,0x00,0xB4,0x91,0xFA,0x7F,0x00,0x00,0x48,0x8D,0x01,0x48,0xB9,0x00,0x00,0xB4,0x91,0xFA,0x7F,0x00,0x00,0x48,0x8D,0x11,0x31,0xC9,0xB3,0x01,0xC3 };
	BYTE _ = m_nChestType - 1;
	memcpy(byCodeBuff + 29, &_, sizeof(_));
	if (!m_nChestType)  //不修改bl
	{
		byCodeBuff[28] = 0x90;
		byCodeBuff[29] = 0x90;
	}
	//接下来要通过RVA计算全局变量的偏移
	QWORD va1 = m_qwBaseAddr + m_stHOOKDATA.m_rva_chesttype_1;
	QWORD va2 = m_qwBaseAddr + m_stHOOKDATA.m_rva_chesttype_2;
	memcpy(byCodeBuff + 2, &va1, sizeof(va1));
	memcpy(byCodeBuff + 15, &va2, sizeof(va2));
	bool ret2 = WriteMemory(qwOffset, byCodeBuff, sizeof(byCodeBuff));

	return ret1 * ret2;
}

bool CHook::Strengthen_1_Hook(QWORD addr)
{
	BYTE byBuff[] = { 0x48,0xB8,0x00,0x00,0xED,0x00,0x22,0x02,0x00,0x00,0xFF,0xD0};
	QWORD qwOffset = GetOffset(m_qwVirtualAddr, Strengthen);
	memcpy(byBuff + 2, &qwOffset, sizeof(qwOffset));
	bool ret1 = WriteMemory(addr, byBuff, sizeof(byBuff));

	BYTE byCodeBuff[] = { 0x4D,0x69,0xC0,0x0,0x0,0x0,0x0,0x41,0x8B,0xD9,0x49,0x8B,0xC3,0x41,0x80,0xFA,0x05,0x7D,0x01,0xC3,0x59,0x48,0xB9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xE1 };
	memcpy(byCodeBuff + 3, &m_nStrengthen, sizeof(m_nStrengthen));
	//接下来要计算流程中跳转回去的绝对地址
	QWORD va1 = addr + 0x1C;
	memcpy(byCodeBuff + 23, &va1, sizeof(va1));

	bool ret2 = WriteMemory(qwOffset, byCodeBuff, sizeof(byCodeBuff));

	return ret1 * ret2;
	
}

bool CHook::Strengthen_2_Hook(QWORD addr)
{
	//在写入跳转之前先获取全局变量的偏移
	if (-1 == m_stHOOKDATA.m_rva_strengthen)  //这个位置第一次HOOK,特征匹配数据
	{
		m_stHOOKDATA.m_rva_strengthen = GetHookData(addr, (LPBYTE)sigStrengthen_2, sizeof(sigStrengthen_2), 0x53, 1, 7);
		WriteHookData();
	}

	BYTE byBuff[] = { 0x48,0xB8,0x00,0x00,0xED,0x00,0x22,0x02,0x00,0x00,0xFF,0xD0 };
	QWORD qwOffset = GetOffset(m_qwVirtualAddr, Strengthen)+0x80;
	memcpy(byBuff + 2, &qwOffset, sizeof(qwOffset));
	bool ret1 = WriteMemory(addr, byBuff, sizeof(byBuff));

	BYTE byCodeBuff[] = { 0x4D,0x69,0xC0,0x0A,0x00,0x00,0x00,0x48,0xB8,0x24,0xFF,0xDF,0xA6,0xF7,0x7F,
		0x00,0x00,0x44,0x8B,0x08,0x80,0xF9,0x05,0x7D,0x01,0xC3,0x58,0x48,0xB8,0x24,0xFF,0xDF,0xA6,0xF7,0x7F,0x00,0x00,0xFF,0xE0 };
	memcpy(byCodeBuff + 3, &m_nStrengthen, sizeof(m_nStrengthen));

	//接下来要计算流程中跳转回去的绝对地址
	QWORD va1 = addr + 0x1F;
	memcpy(byCodeBuff + 29, &va1, sizeof(va1));
	//接下来要通过RVA计算全局变量的偏移	
	QWORD va2 = m_qwBaseAddr + m_stHOOKDATA.m_rva_strengthen;
	memcpy(byCodeBuff + 9, &va2, sizeof(va2));

	bool ret2 = WriteMemory(qwOffset, byCodeBuff, sizeof(byCodeBuff));

	return ret1 * ret2;
}

bool CHook::NewChestHook(QWORD addr)
{
	//在写入跳转之前先获取全局变量的偏移
	if (-1 == m_stHOOKDATA.m_rva_newchest)  //这个位置第一次HOOK,特征匹配数据
	{
		Base _addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)sigChestP, sizeof(sigChestP), 0xB4, 0, 1);
		m_stHOOKDATA.m_rva_newchest = _addr.stData.lData;
		WriteHookData();
	}
	bool ret;
	BYTE byChange[9] = { 0xBB,0x01 ,0x00 ,0x00 ,0x00 ,0x90 ,0x90 ,0x90 ,0x90 };
	BYTE byReset[9] = { 0x41,0x0F ,0xB6 ,0x9C ,0x2E ,0x0 ,0x0 ,0x0 ,0x0 };
	if (m_nNewChest)
	{
		ret= WriteMemory(addr,byChange, sizeof(byChange));		
	}
	else
	{
		memcpy(byReset + 5, &m_stHOOKDATA.m_rva_newchest, sizeof(m_stHOOKDATA.m_rva_newchest));
		ret = WriteMemory(addr, byReset, sizeof(byReset));
	}

	return ret;
}

bool CHook::MaterialHook(QWORD addr)
{
	bool ret1, ret2;
	BYTE byChange[] = { 0xB8,0x01,0x00,0x00,0x00 };
	BYTE byReset[] = { 0x3B,0xC1,0x0F,0x4C,0xC1 };
	WORD wPos1 = 0x167F;
	WORD wPos2 = 0x1175;
	
	if (m_nMaterial)
	{		
		ret1 = WriteMemory(addr, byChange, sizeof(byChange));
	}
	else
	{
		ret1 = WriteMemory(addr, byReset, sizeof(byReset));
	}
	ret2=WriteMemory(addr+0x9F, (LPBYTE)&wPos1, sizeof(wPos1));//还原
	ret2=WriteMemory(addr + 0xA4, (LPBYTE)&wPos2, sizeof(wPos2));//还原
	

	if (m_nMaterial == 2)
	{
		return ret1 * ret2 * FoodHook(addr);
	}
	return ret1 * ret2;
}

bool CHook::FoodHook(QWORD addr)
{		
	WORD wNop = 0x9090;

	bool ret1 = WriteMemory(addr + 0x9F, (LPBYTE)&wNop, sizeof(wNop));
	bool ret2 = WriteMemory(addr + 0xA4, (LPBYTE)&wNop, sizeof(wNop));
	
	return ret1 * ret2;
}

bool CHook::LoveHook(QWORD addr)
{
	bool ret1, ret2;
	WORD wNop = 0x9090;
	WORD wPos1 = 0x6A7C;
	WORD wPos2 = 0x3D75;

	if (m_nLove)
	{
		ret1 = WriteMemory(addr, (LPBYTE)&wNop, sizeof(wNop));
		ret2 = WriteMemory(addr + 0x2D, (LPBYTE)&wNop, sizeof(wNop));
	}
	else
	{
		ret1 = WriteMemory(addr, (LPBYTE)&wPos1, sizeof(wPos1));
		ret2 = WriteMemory(addr + 0x2D, (LPBYTE)&wPos2, sizeof(wPos2));
	}
	return ret1 * ret2;
}

bool CHook::Board_1_Hook(QWORD addr)
{
	bool ret1, ret2,ret3,ret4=true;
	WORD wNop = 0x9090;
	WORD wPos1 = 0x2875;   //cmp     edi, 46h ; 'F' jnz     short loc_1403B6A15
	WORD wPos2 = 0x1377;   //cmp     eax, 46h ; 'F' ja      short loc_1403B6A15
	BYTE byBoardType[] = { 0xB0,0x0,0x90,0x90 };
	BYTE byBoardTypeReset[] = { 0x8B,0x44,0x24,0x24 };
	BYTE byJewelryId[] = { 0x24,0x28 };
	BYTE byJewelryIdReset[] = { 0x84,0x28 };

	if (m_nBoard)
	{
		ret1 = WriteMemory(addr, (LPBYTE)&wNop, sizeof(wNop));
		ret2 = WriteMemory(addr + 0x15, (LPBYTE)&wNop, sizeof(wNop));
		BYTE byType = m_nBoard >= 4 ? 4 : m_nBoard;
		memcpy(byBoardType + 1, &byType, sizeof(byType));
		ret3 = WriteMemory(addr + 0x17, (LPBYTE)&byBoardType, sizeof(byBoardType));
		if (m_nBoard >= 4)
		{
			BYTE byJewelry = (m_nBoard-4)*4+0x28;
			memcpy(byJewelryId + 1, &byJewelry, sizeof(byJewelry));
			ret4 = WriteMemory(addr + 0x12A, (LPBYTE)&byJewelryId, sizeof(byJewelryId));
		}
		
	}
	else
	{
		ret1 = WriteMemory(addr, (LPBYTE)&wPos1, sizeof(wPos1));
		ret2 = WriteMemory(addr, (LPBYTE)&wPos2, sizeof(wPos2));
		ret3 = WriteMemory(addr + 0x17, (LPBYTE)&byBoardTypeReset, sizeof(byBoardTypeReset));
		ret4 = WriteMemory(addr + 0x12A, (LPBYTE)&byJewelryIdReset, sizeof(byJewelryIdReset));
		
	}
	return ret1 * ret2* ret3* ret4;
}

bool CHook::Board_2_Hook(QWORD addr)
{
	bool ret1,ret2;	
	 
	BYTE byChange[] = { 0xBD,0x01,0x00,0x00,0x00,0x90,0x90,0x90 };
	BYTE byReset[] = { 0x85, 0xC0, 0x0F, 0x8E, 0x95, 0x00, 0x00, 0x00 };
	BYTE byChestChange[] = { 0x90,0x90,0x90,0x90,0x90,0x90 };
	BYTE byChestReset[] = { 0x0F, 0x8D, 0x70, 0x01, 0x00, 0x00 };
	
	if (m_nBoard)
	{		
		BYTE byType = m_nBoard >= 4 ? 4 : m_nBoard;
		memcpy(byChange + 1, &byType, sizeof(byType));
		ret1 = WriteMemory(addr, (LPBYTE)&byChange, sizeof(byChange));
		ret2 = WriteMemory(addr-0xD9, (LPBYTE)&byChestChange, sizeof(byChestChange));
	}
	else
	{
		ret1 = WriteMemory(addr, (LPBYTE)&byReset, sizeof(byReset));
		ret2 = WriteMemory(addr - 0xD9, (LPBYTE)&byChestReset, sizeof(byChestReset));
	}
	return ret1*ret2;
}

bool CHook::AdvancedBoardHook(QWORD addr)
{
	bool ret1, ret2;
	WORD wNop = 0x9090;
	WORD wPos = 0x5775;
	BYTE byChange[] = { 0x41,0xBB,0x02,0x00,0x00,0x00,0x90,0x90,0x90,0x90,0x90,0x90,0x90 };
	BYTE byReset[] = { 0x44,0x8B,0xD8,0x4D,0x03,0xDB,0x49,0xC1,0xFB,0x20,0x41,0xFE,0xC3 };
	if (m_nAdvancedBoard)
	{
		ret1 = WriteMemory(addr, (LPBYTE)&wNop, sizeof(wNop));
		ret2 = WriteMemory(addr +7, (LPBYTE)&byChange, sizeof(byChange));
	}
	else
	{
		ret1 = WriteMemory(addr, (LPBYTE)&wPos, sizeof(wPos));
		ret2 = WriteMemory(addr + 7, (LPBYTE)&byReset, sizeof(byReset));
	}
	return ret1 * ret2;
}

bool CHook::FavorabilityHook(QWORD addr)
{
	//在写入跳转之前先获取全局变量的偏移
	if (-1 == m_stHOOKDATA.m_rva_favorability)  //这个位置第一次HOOK,特征匹配数据
	{
		m_stHOOKDATA.m_rva_favorability = GetHookData(addr, (LPBYTE)sigChestP, sizeof(sigChestP), 0xE2, 0, 7);
		WriteHookData();
	}
	DWORD dwDog = GetHookData(addr-0x38, (LPBYTE)sigChestP, sizeof(sigChestP), 0x11A, 0, 6);  //狗好感度RVA

	BYTE byBuff[] = { 0x48,0xBB,0x00,0x00,0xED,0x00,0x22,0x02,0x00,0x00,0xFF,0xD3,0x90 };
	QWORD qwOffset = GetOffset(m_qwVirtualAddr, Favorability);
	memcpy(byBuff + 2, &qwOffset, sizeof(qwOffset));
	bool ret1 = WriteMemory(addr, byBuff, sizeof(byBuff));

	BYTE byCodeBuff[] = { 0x3C,0x00,0x74,0x37,0x3C,0x03,0xBB,0x04,0x00,0x00,0x00,0x0F,0x44,0xC3,0x74,0x12,0x3C,0x06,0x75,0x04,0xB0,//20
		0x04,0xEB,0x0A,0x3C,0x04,0xBB,0x03,0x00,0x00,0x00,0x0F,0x44,0xC3,//33
		0xFF,0xC8,0x48,0xBB,0xA8,0x1D,0x66,0x23,0xF6,0x7F,0x00,0x00,0x66,0x6B,0xC0,0x04,0x48,0x01,0xC3,0x81,0x03,0x20,0x00,0x00,0x00,
		0x48,0xBB,0x26,0x0E,0x66,0x23,0xF6,0x7F,0x00,0x00,0x80,0x3B,0x00,0x75,0x01,0xC3,0x5B,0x48,0xBB,0x3B,
		0x25,0xDF,0x21,0xF6,0x7F,0x00,0x00,0xFF,0xE3 };

	DWORD _ = m_nFavorability - 1;
	memcpy(byCodeBuff + 55, &_, sizeof(_));

	//接下来要通过RVA计算全局变量的偏移
	QWORD va1 = m_qwBaseAddr + dwDog;
	QWORD va2 = m_qwBaseAddr + m_stHOOKDATA.m_rva_favorability;
	QWORD va3 = addr + 0xFF;   //跳转回去的地址
	memcpy(byCodeBuff + 38, &va1, sizeof(va1));
	memcpy(byCodeBuff + 61, &va2, sizeof(va2));
	memcpy(byCodeBuff + 78, &va3, sizeof(va3));
	bool ret2 = WriteMemory(qwOffset, byCodeBuff, sizeof(byCodeBuff));

	return ret1 * ret2;
}

bool CHook::WriteMemory(QWORD addr,LPBYTE byBuff, size_t size)
{
	DWORD dwOldProtect;
	VirtualProtectEx(m_hGameProcess, (LPVOID)addr, size, PAGE_READWRITE, &dwOldProtect);
	bool ret = WriteProcessMemory(m_hGameProcess, (LPVOID)addr, byBuff, size, NULL);
	VirtualProtectEx(m_hGameProcess, (LPVOID)addr, size, dwOldProtect, &dwOldProtect);
	return ret;
}

QWORD CHook::GetOffset(QWORD VirtualAddr, HookType type)
{
	QWORD qwOffset = 0;
	
	switch (type)
	{
	case Gold:
		qwOffset = VirtualAddr+0x100;
		break;
	case Spar:
		qwOffset = VirtualAddr + 0x200;
		break;
	case Exp:
		qwOffset = VirtualAddr;
		break;
	case EExp:
		qwOffset = VirtualAddr + 0x300;
		break;
	case Strengthen:
		qwOffset = VirtualAddr + 0x600;
		break;
	case ChestProbability:
		qwOffset = VirtualAddr + 0x400;
		break;
	case ChestType:
		qwOffset = VirtualAddr + 0x500;
		break;
	case NewChest:
		break;
	case Material:	
		break;
	case Love:
		break;
	case Board:
		break;
	case AdvancedBoard:
		break;
	case Favorability:
		qwOffset = VirtualAddr + 0x700;
		break;
	default:
		break;
	}
	assert(qwOffset);
	return qwOffset;
}

bool CHook::isFirstHook()
{
	bool ret;
	::ReadProcessMemory(m_hGameProcess, (LPCVOID)m_qwBaseAddr, &m_stHOOKDATA, sizeof(m_stHOOKDATA), 0);
	if (m_stHOOKDATA.m_flag==-1)  //不是第一次HOOK
	{
		std::cout << "该游戏窗口修改过..." << std::endl;
		ret = true;
	}
	else  //是第一次HOOK
	{
		std::cout << "该游戏窗口没有修改过..." << std::endl;
		ret = false;
		memset(&m_stHOOKDATA, 0xFF, sizeof(m_stHOOKDATA));
		WriteHookData();
	}
	return ret;
}

DWORD CHook::GetHookData(QWORD addr, LPBYTE Signature, DWORD dwSize, DWORD dwDistance, bool bFlag, DWORD offset)
{
	Base _addr = SearchSignature(m_hGameProcess, m_qwBaseAddr, 模块大小, (LPBYTE)Signature, dwSize, dwDistance, bFlag,1);
	if (bFlag)
	{
		//printf("0x%X|", addr + _addr.stData.hData + offset - m_qwBaseAddr);
		OutputDebugInformation("0x%X|", addr + _addr.stData.hData + offset - m_qwBaseAddr);
		return addr + _addr.stData.hData + offset - m_qwBaseAddr;
	}
	else
	{
		//printf("0x%X|", addr + _addr.stData.lData + offset - m_qwBaseAddr);
		OutputDebugInformation("0x%X|", addr + _addr.stData.lData + offset - m_qwBaseAddr);
		return addr + _addr.stData.lData + offset - m_qwBaseAddr;
	}
	
}

bool CHook::WriteHookData()
{
	bool ret=WriteMemory(m_qwBaseAddr, (LPBYTE)&m_stHOOKDATA, sizeof(m_stHOOKDATA));  //写入PE文件头的内存位置
	return ret;
}
