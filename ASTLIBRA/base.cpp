#include "base.h"




Base SearchSignature(HANDLE process, QWORD qwBaseAddr, QWORD qwSize, LPBYTE Signature, DWORD dwSize, DWORD dwDistance, bool bFlag, bool bAddressOrData)
{
	QWORD endAddr = qwBaseAddr + qwSize;
	const DWORD pageSize = 4096;

	BOOL _break = FALSE;
	int curPage = 0;//���������ڵڼ�ҳ�еĵڼ����ҵ���������
	int curIndex = 0;
	Base base;
	ZeroMemory(&base, sizeof(Base));
	BYTE page[pageSize];//ÿҳ��ȡ4096���ֽ�
	QWORD tmpAddr = qwBaseAddr;

	while (tmpAddr <= endAddr - dwSize)
	{
		::ReadProcessMemory(process, (LPCVOID)tmpAddr, &page, pageSize, 0);
		//�ڸ�ҳ�в���������
		for (int i = 0; i < pageSize; i++)
		{

			for (int j = 0; j < dwSize; j++)
			{
				//ֻҪ��һ�����������Ӧ�������˳�ѭ��
				if (Signature[j] != page[i + j])break;

				//�ҵ��˳�����ѭ��
				if (j == dwSize - 1)
				{
					_break = TRUE;
					if (bAddressOrData) //��Ҫ������
					{
						if (!bFlag) {  //����������
							curIndex = i;
							base.data[0] = page[curIndex - dwDistance - 7];							
							base.data[1] = page[curIndex - dwDistance - 6];
							base.data[2] = page[curIndex - dwDistance - 5];
							base.data[3] = page[curIndex - dwDistance - 4];
							base.data[4] = page[curIndex - dwDistance - 3];
							base.data[5] = page[curIndex - dwDistance - 2];
							base.data[6] = page[curIndex - dwDistance - 1];
							base.data[7] = page[curIndex - dwDistance - 0];
						}
						else {
							curIndex = i;
							base.data[0] = page[curIndex + dwDistance + 0];
							base.data[1] = page[curIndex + dwDistance + 1];
							base.data[2] = page[curIndex + dwDistance + 2];
							base.data[3] = page[curIndex + dwDistance + 3];
							base.data[4] = page[curIndex + dwDistance + 4];
							base.data[5] = page[curIndex + dwDistance + 5];
							base.data[6] = page[curIndex + dwDistance + 6];
							base.data[7] = page[curIndex + dwDistance + 7];
						}
					}
					else //��Ҫ�ҵ�ַ
					{
						if (!bFlag) //��ַ������
						{
							base.address = tmpAddr + i - dwDistance;
						}
						else
						{
							base.address = tmpAddr + i + dwDistance;
						}
						OutputDebugInformation("%llX RVA:%llX\r\n", base.address, base.address - qwBaseAddr);
						//printf("%llX RVA:%llX\r\n", base.address, base.address - qwBaseAddr);
					}

					break;
				}
			}
			if (_break) break;
		}
		if (_break) break;
		curPage++;
		tmpAddr += pageSize;
	}

	

	return base;
}

void OutputDebugInformation(const char* pszFormat, ...)
{
	char szbufFormat[0x260];
	char szbufFormat_Game[0x280];
	RtlZeroMemory(szbufFormat, sizeof(szbufFormat));
	RtlZeroMemory(szbufFormat_Game, sizeof(szbufFormat_Game));
	va_list argList;
	va_start(argList, pszFormat);
	vsprintf_s(szbufFormat, pszFormat, argList);
	strcat_s(szbufFormat_Game, "��֮��ƽ ");// �������ͷ����
	strcat_s(szbufFormat_Game, szbufFormat);
	OutputDebugStringA(szbufFormat_Game);
	va_end(argList);
}
