#include "AOB memscan.h"



QWORD scanmem(HANDLE process, char* markCode, int nOffset, unsigned long dwReadLen, QWORD StartAddr, QWORD EndAddr, int InstructionLen)
{
	QWORD result;
	if (strlen(markCode) % 2 != 0) return 0;
	//特征码长度
	int len = strlen(markCode) / 2;  //获取代码的字节数

	//将特征码转换成byte型 保存在m_code 中
	BYTE* m_code = new BYTE[len];
	for (int i = 0; i < len; i++)
	{
		//定义可容纳单个字符的一种基本数据类型。
		char c[] = { markCode[i * 2], markCode[i * 2 + 1], '\0' };
		//将参数nptr字符串根据参数base来转换成长整型数
		m_code[i] = (BYTE)::strtol(c, NULL, 16);
	}
	//每次读取游戏内存数目的大小
	const DWORD pageSize = 4096;

	//每页读取4096个字节
	BYTE* page = new BYTE[pageSize];
	uintptr_t tmpAddr = StartAddr;
	//定义和特征码一样长度的标识
	int compare_one = 0;

	while (tmpAddr <= EndAddr)
	{
		::ReadProcessMemory(process, (LPCVOID)tmpAddr, page, pageSize, 0); //读取0x400000的内存数据，保存在page，长度为pageSize

		//在该页中查找特征码
		for (int i = 0; i < pageSize; i++)
		{
			if (m_code[0] == page[i])//有一个字节与特征码的第一个字节相同，则搜索
			{
				for (int j = 0; j < len - 1; j++)
				{
					if (m_code[j + 1] == page[i + j + 1])//比较每一个字节的大小，不相同则退出
					{
						compare_one++;
					}
					else
					{
						compare_one = 0;
						break;
					}//如果下个对比的字节不相等，则退出，减少资源被利用
				}

				if ((compare_one + 1) == len)
				{
					// 找到特征码处理
					//赋值时要给初始值，避免冲突
					QWORD dwAddr = tmpAddr + i + nOffset;
					QWORD ullRet = 0;
					::ReadProcessMemory(process, (void*)dwAddr, &ullRet, dwReadLen, 0);
					//cout<<dwAddr<<endl;
					//这里的dwAddr已经对应的是搜索到的地址
					//地址输出的也是10进制    需要转化为16进制 
					result = dwAddr;//记录地址
					if (InstructionLen)
					{
						ullRet += dwAddr + dwReadLen;
					}

					return result;
				}
			}
		}

		tmpAddr = tmpAddr + pageSize - len;//下一页搜索要在前一页最后长度len 开始查找，避免两页交接中间有特征码搜索不出来
	}

	return 0;
}

bool unprotect(char* ptr, int len, PDWORD oldp) {
	return VirtualProtect((LPVOID)(ptr), len, PAGE_EXECUTE_READWRITE, oldp);
}
bool protect(char* ptr, int len, PDWORD oldp) {
	DWORD dummy;
	return VirtualProtect((LPVOID)(ptr), len, *oldp, &dummy);
}