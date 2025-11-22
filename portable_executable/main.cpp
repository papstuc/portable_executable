#include <cstdio>
#include <Windows.h>

#include "portable_executable/image.hpp"
#include "portable_executable/file.hpp"

static void run_image_tests(const portable_executable::image_t* image)
{
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

	for (const auto [relocation_block, relocation_block_va] : image->relocations())
	{
		std::printf("offset: 0x%x -> type: 0x%x\n", relocation_block.offset, relocation_block.type);
	}

	std::printf("iterating debug information...\n");

	for (const auto debug_info : image->debug_info())
	{
		std::printf("va: 0x%x -> type: 0x%x\n", debug_info.virtual_address, static_cast<std::uint32_t>(debug_info.type));
	}

	std::printf("iterating exceptions...\n");

	for (const auto runtime_function : image->runtime_functions())
	{
		const auto virtual_address = static_cast<std::uint32_t>(runtime_function.function_begin - image->as<const std::uint8_t*>());

		std::printf("va: 0x%x -> unwind code count: 0x%x\n", virtual_address, runtime_function.unwind_info->unwind_code_count);

		for (const auto unwind_opcode : *runtime_function.unwind_info)
		{
			std::printf("(unwind code) offset: 0x%x, info: 0x%x\n", unwind_opcode.offset, unwind_opcode.info);
		}
	}
}

std::int32_t main()
{
	// parse in-memory portable executable
	HMODULE user32 = LoadLibrary(L"user32");

	if (!user32)
	{
		std::printf("failed to load user32\n");

		return EXIT_FAILURE;
	}

	auto in_memory_image = reinterpret_cast<const portable_executable::image_t*>(user32);

	run_image_tests(in_memory_image);

	FreeLibrary(user32);

	std::string_view ntoskrnl_path_view = "C:\\Windows\\System32\\ntoskrnl.exe";

	// load a portable executable from filesystem
	portable_executable::file_t ntoskrnl(ntoskrnl_path_view);

	// avoid exceptions in constructor to support contexts without exception support
	if (!ntoskrnl.load())
	{
		std::printf("failed to load ntoskrnl from filesystem\n");

		return EXIT_FAILURE;
	}

	const auto ntoskrnl_image = ntoskrnl.image();

	run_image_tests(ntoskrnl_image);

	// signature made for ntoskrnl version 22h2
	std::uint8_t* hvi_is_any_hypervisor_present = ntoskrnl_image->signature_scan("40 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 33 C9 41 B9");

	std::printf("ntoskrnl!HviIsAnyHypervisorPresent -> 0x%p\n", hvi_is_any_hypervisor_present);

	return EXIT_SUCCESS;
}