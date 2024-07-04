#pragma once
#include "rat.h"

Memory::Memory(DWORD PID) {
	KM_PID = PID;
	Setup();
}

BOOL Memory::Setup() {
	hDriver = CreateFileA(("\\\\.\\privatedriver"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if (hDriver == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

uint64_t Memory::GetModuleBase(const std::string& module_name) {

	if (hDriver == INVALID_HANDLE_VALUE)
		return 0;

	K_GET_BASE_MODULE_REQUEST request;
	request.PID = KM_PID;
	request.handle = 0;

	std::wstring wstr{ std::wstring(module_name.begin(), module_name.end()) };
	memset(request.name, 0, sizeof(WCHAR) * 260);
	wcscpy(request.name, wstr.c_str());

	DWORD junk;

	if (DeviceIoControl(hDriver, ioctl_get_module_base, &request, sizeof(K_GET_BASE_MODULE_REQUEST), &request, sizeof(K_GET_BASE_MODULE_REQUEST), &junk, 0)) {
		return request.handle;
	}

	return request.handle;
}

bool Memory::Rpm(uint64_t address, void* buffer, size_t size)
{
	return ReadMemoryFromKernel(KM_PID, address, buffer, size);
}

bool Memory::ReadMemoryFromKernel(int processId, uint64_t address, void* buffer, size_t size)
{
	K_RW_REQUEST request = {};

	request.PID = processId;
	request.src = address;
	request.dst = (uint64_t)buffer;
	request.size = size;

	DWORD junk = 0;
	auto status = DeviceIoControl(hDriver, ioctl_read_memory, &request, sizeof(K_RW_REQUEST), &request, sizeof(K_RW_REQUEST), &junk, (LPOVERLAPPED)NULL);
	if (!NT_SUCCESS(status)) {
		return false;
	}

	return (buffer != NULL);
}

bool Memory::Wpm(uint64_t address, void* buffer, size_t size)
{
	return WriteMemoryFromKernel(KM_PID, address, buffer, size);
}

bool Memory::WriteMemoryFromKernel(int processId, uint64_t address, void* buffer, size_t size)
{
	K_RW_REQUEST request = {};

	request.PID = processId;
	request.src = address;
	request.dst = (uint64_t)buffer;
	request.size = size;

	DWORD junk = 0;
	auto status = DeviceIoControl(hDriver, ioctl_write_memory, &request, sizeof(K_RW_REQUEST), &request, sizeof(K_RW_REQUEST), &junk, (LPOVERLAPPED)NULL);
	if (!NT_SUCCESS(status)) {
		return false;
	}

	return (buffer != NULL);
}