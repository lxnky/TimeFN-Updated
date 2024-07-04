#pragma once
#include <windows.h>
#include <iostream>
#include <winternl.h>
#include <vector>
#include <map>
#include <string>

class Memory
{
public:

	Memory(DWORD PID);
	HANDLE hDriver;

	uint64_t GetModuleBase(const std::string& module_name);

	template<typename T>
	T Rpm(uint64_t address);
	bool Rpm(uint64_t address, void* buffer, size_t size);

	template<typename T>
	void Wpm(uint64_t address, T value);
	bool Wpm(uint64_t address, void* buffer, size_t size);

private:
#define ioctl_read_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8692324, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ioctl_write_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8692325, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ioctl_get_module_base CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8692326, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
#define ioctl_protect_virutal_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8692327, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
#define ioctl_allocate_virtual_memory CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8692328, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

	DWORD KM_PID;

	typedef struct _K_RW_REQUEST {
		ULONG PID;
		ULONGLONG src;
		ULONGLONG dst;
		ULONGLONG size;
	} K_RW_REQUEST, * PK_RW_REQUEST;

	typedef struct _K_GET_BASE_MODULE_REQUEST {
		ULONG PID;
		ULONGLONG handle;
		WCHAR name[260];
	} K_GET_BASE_MODULE_REQUEST, * PK_GET_BASE_MODULE_REQUEST;

	BOOL Setup();
	bool ReadMemoryFromKernel(int processId, uint64_t address, void* buffer, size_t size);
	bool WriteMemoryFromKernel(int processId, uint64_t address, void* buffer, size_t size);

}; extern Memory* pMem;

template<typename T>
inline T Memory::Rpm(uint64_t address)
{
	T buffer;
	Rpm(address, &buffer, sizeof(T));
	return buffer;
}

template<typename T>
inline void Memory::Wpm(uint64_t address, T value)
{
	Wpm(address, &value, sizeof(T));
}