#pragma once
#include "windows.h"
#include "iostream"
#include "assert.h"
#include "conio.h"
#include "Config.h"
#include "base.h"

//����HOOK���ݵĽṹ��,�����ظ�HOOKʱ��ȡ����
typedef struct _HOOKDATA
{
	DWORD m_flag;   //�Ƿ��ǵ�һ��HOOK�ı�־λ,0����
	DWORD m_rva_gold;
	DWORD m_rva_exp_1;
	DWORD m_rva_exp_2;
	DWORD m_rva_chesttype_1;
	DWORD m_rva_chesttype_2;	
	QWORD m_Strengthen_1_Hook_Addr;   //ǿ��װ����HOOK��ַ
	DWORD m_rva_strengthen;
	DWORD m_rva_newchest;
	DWORD m_rva_favorability;
}HOOKDATA;


class CHook
{
public:
	CHook(HANDLE ,QWORD ,QWORD);
private:
	bool GoldHook(QWORD);
	bool ExpHook(QWORD);
	bool SparHook(QWORD);
	bool EExpHook(QWORD);
	bool ChestProbabilityHook(QWORD);
	bool ChestTypeHook(QWORD);
	bool Strengthen_1_Hook(QWORD);
	bool Strengthen_2_Hook(QWORD);
	bool NewChestHook(QWORD);
	bool MaterialHook(QWORD);
	bool FoodHook(QWORD);
	bool LoveHook(QWORD);
	bool Board_1_Hook(QWORD);
	bool Board_2_Hook(QWORD);
	bool AdvancedBoardHook(QWORD);
	bool FavorabilityHook(QWORD);

	bool WriteMemory(QWORD, LPBYTE, size_t);

	QWORD GetOffset(QWORD, HookType type);

	//HOOK��������Ҫ���������
	bool isFirstHook();
	DWORD GetHookData(QWORD addr, LPBYTE Signature, DWORD dwSize, DWORD dwDistance, bool bFlag, DWORD offset);
	bool WriteHookData();

private:
	//�����ļ�
	int m_nGold;
	int m_nSpar;
	int m_nExp;
	int m_nEExp;
	int m_nStrengthen;
	float m_fChestProbability;
	int m_nChestType;
	int m_nNewChest;
	int m_nMaterial;
	int m_nLove;
	int m_nBoard;
	int m_nAdvancedBoard;
	int m_nFavorability;

	HOOKDATA m_stHOOKDATA;


	HANDLE m_hGameProcess;
	QWORD m_qwBaseAddr;
	QWORD m_qwVirtualAddr;
};

