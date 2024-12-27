## Modern C++ wrapper around the Microsoft Portable Executable file format

This is a W.I.P. library to interact with Microsoft PE files (.exe, .dll, .sys) in a modern C++ way.

Code to traverse tables (IAT, EAT, Relocs, etc...) is being abstracted by iterators to provide developers with a clean abstracted interface to base their work on.

## Examples

More samples can be found in the main.cpp file.

### Obtaining an image instance
The library currently only works with already mapped PE files, which means that sections have to be mapped to their virtual addresses.

The library however provides a way to map PE files on disk to memory using RAII.

```cpp
// obtain image from a already mapped pe
HMODULE user32 = LoadLibrary(L"user32");
auto in_memory_image = reinterpret_cast<const portable_executable::image_t*>(user32);

// obtain image from filesystem and load it into memory
portable_executable::file_t ntoskrnl("C:\\Windows\\System32\\ntoskrnl.exe");

if (!ntoskrnl.load())
{
    // handle error cases
}

const portable_executable::image_t* from_fs_image = ntoskrnl.image();
```

### Iterating sections
The section iterator provides the caller with a raw view of a section header.

```cpp
for (const auto& section : image->sections())
{
    std::printf("section name: %s -> va: 0x%x size: 0x%x\n",
                    section.to_str().c_str(),
                    section.virtual_address,
                    section.virtual_size);
}
```
### Iterating imports
The imports iterator abstracts away boilerplate code and returns the module name, function name and address of the respective import.

```cpp
for (const auto& [module_name, import_name, address] : image->imports())
{
    std::printf("%s!%s -> 0x%p\n", module_name.c_str(), import_name.c_str(), address);
}
```

### Iterating relocations
The relocations iterator provides the caller with a raw view of the offset of the relocation and its type.

```cpp
for (const auto& relocation : image->relocations())
{
    std::printf("offset: %x -> type: %x\n", relocation.offset, relocation.type);
}
```