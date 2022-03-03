#include <Windows.h>


class Memory
{
private:
	HANDLE hProcess;


public:
	Memory(HANDLE hProc)
	{
		this->hProcess = hProc;
	}

	template <typename T>
	T ReadMemory(uintptr_t SourceAddress, size_t size)
	{
		T buffer;


	}
};