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

	for (const auto& [name, address] : image->exports())
	{
		std::printf("%s -> 0x%p\n", name.c_str(), address);
	}

	std::printf("iterating imports...\n");

	for (const auto& [module_name, import_name, address] : image->imports())
	{
		std::printf("%s!%s -> 0x%p\n", module_name.c_str(), import_name.c_str(), address);
	}

	std::printf("iterating relocations...\n");

	for (const auto& relocation : image->relocations())
	{
		std::printf("offset: %x -> type: %x\n", relocation.offset, relocation.type);
	}

	FreeLibrary(user32);

	return EXIT_SUCCESS;
}