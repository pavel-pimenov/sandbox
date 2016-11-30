#include <iostream>
#include <vector>

#include <windows.h>
#include <Psapi.h>
#include <tchar.h>

#pragma comment(lib, "psapi.lib")

DWORD getWorkingSet()
{
	PROCESS_MEMORY_COUNTERS l_pmc = { 0 };
	const auto l_mem = GetProcessMemoryInfo(GetCurrentProcess(), &l_pmc, sizeof(l_pmc));
	return l_pmc.WorkingSetSize;
}

class A1
{
	void** m;
public:
	A1()
	{
		m = new void*[6];
	}
	~A1()
	{
		delete [] m;
	}
};
class A2
{
	void* m[6];
};

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc != 2)
	{
	 printf("run: calc-ram.exe 1 or calc-ram.exe 1\r\n");
	}
 	else
	{
        const auto type = atoi(argv[1]) == 1;
	const auto m1 = getWorkingSet();
	printf("RAM before = %u\r\n", m1);
        std::vector<void*> m(100000);
	for (int i = 0; i < m.size(); ++i)
	{
            if(type == 1)		
	        m[i] = new A1; 
            else
	        m[i] = new A2; 
  
	}
	auto m2 = getWorkingSet();
	printf("RAM after= %u Delta = %d Type = %s\r\n", m2, m2 - m1, type == 1? " m[i] = new A1" :  " m[i] = new A2");
	for (int i = 0; i < m.size(); ++i)
	{
	    delete m[i];  m[i] = NULL;
	}
	}
	return 0;
}