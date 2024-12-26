#include <cstdio>
#include <Windows.h>

#include "portable_executable/image.hpp"

std::int32_t main()
{
	HMODULE user32 = LoadLibrary(L"user32");

	if (!user32)
	{
		std::printf("failed to load user32\n");

		return EXIT_FAILURE;
	}

	auto image = reinterpret_cast<portable_executable::image_t*>(user32);

	std::printf("iterating sections...\n");

	for (const auto& section : image->sections())
	{
		std::printf("section name: %s -> va: 0x%x size: 0x%x\n", section.to_str().c_str(), section.virtual_address, section.virtual_size);
	}

	std::printf("iterating exports...\n");

	for (const auto& export_entry : image->exports())
	{
		std::printf("%s -> 0x%p\n", export_entry.name.c_str(), export_entry.address);
	}

	std::printf("iterating imports...\n");

	for (const auto& import_entry : image->imports())
	{
		std::printf("%s!%s -> 0x%p\n", import_entry.module_name.c_str(), import_entry.import_name.c_str(), import_entry.address);
	}

	FreeLibrary(user32);

	return EXIT_SUCCESS;
}