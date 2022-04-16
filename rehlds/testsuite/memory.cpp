/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "precompiled.h"

#ifdef _WIN32

bool HIDDEN FindModuleByAddress(size_t addr, Module *module)
{
	if (!module)
		return false;

	MEMORY_BASIC_INFORMATION mem;
	VirtualQuery((void *)addr, &mem, sizeof(mem));

	IMAGE_DOS_HEADER *dos = (IMAGE_DOS_HEADER*)mem.AllocationBase;
	IMAGE_NT_HEADERS *pe = (IMAGE_NT_HEADERS*)((unsigned long)dos + (unsigned long)dos->e_lfanew);

	if (pe->Signature != IMAGE_NT_SIGNATURE)
		return false;

	module->base = (size_t)mem.AllocationBase;
	module->size = (size_t)pe->OptionalHeader.SizeOfImage;
	module->end = module->base + module->size - 1;
	module->handle = NULL;

	ProcessModuleData(module);

	return true;
}
bool HIDDEN FindModuleByName(const char *moduleName, Module *module)
{
	if (!moduleName || !*moduleName || !module)
		return false;

	HANDLE hProcess = GetCurrentProcess();
	HMODULE hModuleDll = GetModuleHandle(moduleName);

	if (!hProcess || !hModuleDll)
		return false;

	MODULEINFO moduleInfo;
	GetModuleInformation(hProcess, hModuleDll, &moduleInfo, sizeof(moduleInfo));

	module->base = (size_t)moduleInfo.lpBaseOfDll;
	module->size = (size_t)moduleInfo.SizeOfImage;
	module->end = module->base + module->size - 1;
	module->handle = NULL;

	ProcessModuleData(module);

	return true;
}

#else // _WIN32

static Section _initialSection = {};	// will hold all process memory sections

// Parses /proc/PID/maps file for the process memory sections.
bool HIDDEN ReloadProcessMemoryInfo()
{
	Section *section = _initialSection.next;
	while (section != NULL)
	{
		Section *next = section->next;
		delete section;
		section = next;
	}
	_initialSection.next = NULL;

	char file[_MAX_FNAME];
	char buffer[2048];

	pid_t pid = getpid();
	snprintf(file, sizeof(file), "/proc/%d/maps", pid);
	file[sizeof(file)-1] = 0;

	FILE *fp = fopen(file, "rt");
	if (!fp)
		return false;

	static char mbuffer[65536];
	setbuffer(fp, mbuffer, sizeof(mbuffer));

	size_t length = 0;
	size_t start, end, size;
	char accessProtection[5];
	int inode;
	int fields;
	section = &_initialSection;
	while (!feof(fp))
	{
		if (fgets(buffer, sizeof(buffer), fp) == NULL)
			break;

		fields = sscanf(buffer, "%lx-%lx %4s %*s %*s %d %255s", &start, &end, accessProtection, &inode, file);
		//printf("%lx-%lx %4s %d %s\n", start, end, accessProtection, inode, fields > 4 ? file : "");
		if (fields < 4)
			return false;

		section->next = new Section;
		section = section->next;
		section->next = NULL;
		section->start = start;
		section->end = end;
		section->size = end - start;
		section->protection = 0;
		if (accessProtection[0] == 'r') section->protection |= PROT_READ;
		if (accessProtection[1] == 'w') section->protection |= PROT_WRITE;
		if (accessProtection[2] == 'x') section->protection |= PROT_EXEC;
		section->inode = inode;
		if (fields > 4)
		{
			strncpy(section->filename, file, _MAX_FNAME);
			section->filename[_MAX_FNAME - 1] = 0;
			section->namelen = strlen(section->filename);
		}
		else
		{
			section->filename[0] = 0;
			section->namelen = 0;
		}
	}

	fclose(fp);

	return true;
}
// Finds section by the address.
Section* HIDDEN FindSectionByAddress(size_t addr)
{
	Section *section = _initialSection.next;
	while (section != NULL)
	{
		if (section->start <= addr && addr < section->end)
			break;
		section = section->next;
	}
	return section;
}
// Finds section by the file name.
Section* HIDDEN FindSectionByName(const char *moduleName)
{
	int len = strlen(moduleName);
	Section *section = _initialSection.next;
	while (section != NULL)
	{
		if (len < section->namelen &&
			section->filename[section->namelen - len - 1] == '/' &&
			!_stricmp(section->filename + section->namelen - len, moduleName))
			break;
		section = section->next;
	}
	return section;
}
// Finds section by the address.
Section* HIDDEN GetSectionByAddress(size_t addr)
{
	Section *section = FindSectionByAddress(addr);
	if (section == NULL)
	{
		// Update sections info
		if (!ReloadProcessMemoryInfo())
			return NULL;
		section = FindSectionByAddress(addr);
	}
	return section;
}
// Finds section by the file name.
Section* HIDDEN GetSectionByName(const char *moduleName)
{
	Section *section = FindSectionByName(moduleName);
	if (section == NULL)
	{
		// Update sections info
		if (!ReloadProcessMemoryInfo())
			return NULL;
		section = FindSectionByName(moduleName);
	}
	return section;
}
// Fills module structure by info from the sections. Should be supplied with the first module section.
bool HIDDEN FillModule(Section *section, Module *module)
{
	if (!section || !module)
		return false;

	size_t base = section->start;
	size_t end = section->end;
	char *filename = section->filename;

	// Iterate thru next sections to find the end
	section = section->next;
	while (section != NULL)
	{
		if (end != section->start)		// not adjacent sections - we don't support this
			return false;
		else if (section->inode == 0)	// end of the module memory
			break;
		else
			end = section->end;
		section = section->next;
	}

	//printf("%s: %lx-%lx\n", filename, base, end);

	module->base = base;
	module->size = end - base;
	module->end = module->base + module->size - 1;
	module->handle = (size_t)dlopen(filename, RTLD_NOW);	// lock from unloading

	ProcessModuleData(module);

	return true;
}

bool HIDDEN FindModuleByAddress(size_t addr, Module *module)
{
	if (!module)
		return false;

	Section *section = GetSectionByAddress(addr);
	if (section == NULL)
		return false;

	if (section->filename[0] != '/')	// should point to a real file
		return false;

	// Start over with the name to find module start
	return FindModuleByName(section->filename + 1, module);
}
bool HIDDEN FindModuleByName(const char *moduleName, Module *module)
{
	if (!moduleName || !*moduleName || !module)
		return false;

	Section *section = GetSectionByName(moduleName);
	if (section == NULL)
		return false;

	return FillModule(section, module);
}

#endif // _WIN32


#ifdef _WIN32

inline size_t HIDDEN FindSymbol(Module *module, const char* symbolName, int index)
{
	return NULL;
}

#else // _WIN32

size_t HIDDEN FindSymbol(Module *module, const char* symbolName, int index)
{
	int i;
	link_map *dlmap;
	struct stat dlstat;
	int dlfile;
	uintptr_t map_base;
	Elf32_Ehdr *file_hdr;
	Elf32_Shdr *sections, *shstrtab_hdr, *symtab_hdr, *strtab_hdr;
	Elf32_Sym *symtab;
	const char *shstrtab, *strtab;
	uint16 section_count;
	uint32 symbol_count;
	size_t address;

	// If index > 0 then we shouldn't use dlsym, cos it will give wrong result
	if (index == 0)
	{
		address = (size_t)dlsym((void *)module->handle, symbolName);
		if (address != NULL)
			return address;
	}

	dlmap = (struct link_map *)module->handle;
	symtab_hdr = NULL;
	strtab_hdr = NULL;

	dlfile = open(dlmap->l_name, O_RDONLY);
	if (dlfile == -1 || fstat(dlfile, &dlstat) == -1)
	{
		close(dlfile);
		return NULL;
	}

	// Map library file into memory
	file_hdr = (Elf32_Ehdr *)mmap(NULL, dlstat.st_size, PROT_READ, MAP_PRIVATE, dlfile, 0);
	map_base = (uintptr_t)file_hdr;
	close(dlfile);
	if (file_hdr == MAP_FAILED)
	{
		return NULL;
	}

	if (file_hdr->e_shoff == 0 || file_hdr->e_shstrndx == SHN_UNDEF)
	{
		munmap(file_hdr, dlstat.st_size);
		return NULL;
	}

	sections = (Elf32_Shdr *)(map_base + file_hdr->e_shoff);
	section_count = file_hdr->e_shnum;
	// Get ELF section header string table
	shstrtab_hdr = &sections[file_hdr->e_shstrndx];
	shstrtab = (const char *)(map_base + shstrtab_hdr->sh_offset);

	// Iterate sections while looking for ELF symbol table and string table
	for (uint16 i = 0; i < section_count; i++)
	{
		Elf32_Shdr &hdr = sections[i];
		const char *section_name = shstrtab + hdr.sh_name;
		//printf("Seg[%d].name = '%s'\n", i, section_name);

		if (strcmp(section_name, ".symtab") == 0)
		{
			symtab_hdr = &hdr;
		}
		else if (strcmp(section_name, ".strtab") == 0)
		{
			strtab_hdr = &hdr;
		}
	}

	if (symtab_hdr == NULL || strtab_hdr == NULL)
	{
		munmap(file_hdr, dlstat.st_size);
		return NULL;
	}

	symtab = (Elf32_Sym *)(map_base + symtab_hdr->sh_offset);
	strtab = (const char *)(map_base + strtab_hdr->sh_offset);
	symbol_count = symtab_hdr->sh_size / symtab_hdr->sh_entsize;

	int mangleNameLength;
	int mangleNameLastLength = 1024;	// Is it enouph?

	// If index is 0 then we need to take first entry
	if (index == 0) index++;

	// Iterate symbol table
	int match = 1;
	for (uint32 i = 0; i < symbol_count; i++)
	{
		Elf32_Sym &sym = symtab[i];
		unsigned char sym_type = ELF32_ST_TYPE(sym.st_info);
		const char *sym_name = strtab + sym.st_name;

		// Skip symbols that are undefined or do not refer to functions or objects
		if (sym.st_shndx == SHN_UNDEF || (sym_type != STT_FUNC && sym_type != STT_OBJECT))
		{
			continue;
		}

		if (strcmp(sym_name, symbolName) == 0)
		{
			if (match == index)
			{
				address = (size_t)(dlmap->l_addr + sym.st_value);
				break;
			}
			else
			{
				match++;
			}
		}
		// Try to find lowest length mangled name then
		if (sym_name[0] == '_' && sym_name[1] == 'Z' && strstr(sym_name + 2, symbolName) != NULL && (mangleNameLength = strlen(sym_name)) < mangleNameLastLength)
		{
			mangleNameLastLength = mangleNameLength;
			address = (size_t)(dlmap->l_addr + sym.st_value);
#ifdef _DEBUG
			printf("FindSymbol (mangled name): symbol: \"%s\", address: %lx\n", sym_name, address);
#endif
		}
	}

	munmap(file_hdr, dlstat.st_size);

#ifdef _DEBUG
	printf("FindSymbol (elf): symbol: \"%s\", address: %lx\n", symbolName, address);
#endif

	return address;
}

#endif // _WIN32

#ifdef _WIN32
void ProcessModuleData(Module *module)
{
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)module->base;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		rehlds_syserror("%s: Invalid DOS header signature", __func__);
	}

	PIMAGE_NT_HEADERS NTHeaders = (PIMAGE_NT_HEADERS)((size_t)module->base + dosHeader->e_lfanew);
	if (NTHeaders->Signature != 0x4550) {
		rehlds_syserror("%s: Invalid NT header signature", __func__);
	}

	PIMAGE_SECTION_HEADER cSection = (PIMAGE_SECTION_HEADER)((size_t)(&NTHeaders->OptionalHeader) + NTHeaders->FileHeader.SizeOfOptionalHeader);

	PIMAGE_SECTION_HEADER CodeSection = NULL;

	for (int i = 0; i < NTHeaders->FileHeader.NumberOfSections; i++, cSection++) {
		if (cSection->VirtualAddress == NTHeaders->OptionalHeader.BaseOfCode)
			CodeSection = cSection;
	}

	if (CodeSection == NULL) {
		rehlds_syserror("%s: Code section not found", __func__);
	}

	module->codeSection.start = (uint32)module->base + CodeSection->VirtualAddress;
	module->codeSection.size = CodeSection->Misc.VirtualSize;
	module->codeSection.end = module->codeSection.start + module->codeSection.size;
	module->codeSection.next = NULL;
}
#else // _WIN32
void ProcessModuleData(Module *module)
{

}
#endif

#ifdef _WIN32

static DWORD oldPageProtection;

inline bool HIDDEN EnablePageWrite(size_t addr, size_t size)
{
	return VirtualProtect((void *)addr, size, PAGE_EXECUTE_READWRITE, &oldPageProtection) != 0;
}
inline bool HIDDEN RestorePageProtection(size_t addr, size_t size)
{
	bool ret = VirtualProtect((void *)addr, size, oldPageProtection, &oldPageProtection) != 0;
	FlushInstructionCache(GetCurrentProcess(), (void *)addr, size);
	return ret;
}

#else // _WIN32

bool HIDDEN EnablePageWrite(size_t addr, size_t size)
{
	size_t alignedAddr = ALIGN(addr);
	size += addr - alignedAddr;
	return mprotect((void *)alignedAddr, size, PROT_READ | PROT_WRITE | PROT_EXEC) == 0;
}
bool HIDDEN RestorePageProtection(size_t addr, size_t size)
{
	Section *section = GetSectionByAddress(addr);
	if (section == NULL)
		return false;
	size_t alignedAddr = ALIGN(addr);
	size += addr - alignedAddr;
	return mprotect((void *)alignedAddr, size, section->protection) == 0;
}

#endif // _WIN32


// Converts HEX string containing pairs of symbols 0-9, A-F, a-f with possible space splitting into byte array
size_t HIDDEN ConvertHexString(const char *srcHexString, unsigned char *outBuffer, size_t bufferSize)
{
	unsigned char *in = (unsigned char *)srcHexString;
	unsigned char *out = outBuffer;
	unsigned char *end = outBuffer + bufferSize;
	bool low = false;
	uint8 byte = 0;
	while (*in && out < end)
	{
		if (*in >= '0' && *in <= '9') { byte |= *in - '0'; }
		else if (*in >= 'A' && *in <= 'F') { byte |= *in - 'A' + 10; }
		else if (*in >= 'a' && *in <= 'f') { byte |= *in - 'a' + 10; }
		else if (*in == ' ') { in++; continue; }

		if (!low)
		{
			byte = byte << 4;
			in++;
			low = true;
			continue;
		}
		low = false;

		*out = byte;
		byte = 0;

		in++;
		out++;
	}
	return out - outBuffer;
}
size_t HIDDEN MemoryFindForward(size_t start, size_t end, const unsigned char *pattern, const unsigned char *mask, size_t len)
{
	// Ensure start is lower than the end
	if (start > end)
	{
		size_t reverse = end;
		end = start;
		start = reverse;
	}

	unsigned char *cend = (unsigned char*)(end - len + 1);
	unsigned char *current = (unsigned char*)(start);

	// Just linear search for sequence of bytes from the start till the end minus pattern length
	size_t i;
	if (mask)
	{
		// honoring mask
		while (current < cend)
		{
			for (i = 0; i < len; i++)
			{
				if ((current[i] & mask[i]) != (pattern[i] & mask[i]))
					break;
			}

			if (i == len)
				return (size_t)(void*)current;

			current++;
		}
	}
	else
	{
		// without mask
		while (current < cend)
		{
			for (i = 0; i < len; i++)
			{
				if (current[i] != pattern[i])
					break;
			}

			if (i == len)
				return (size_t)(void*)current;

			current++;
		}
	}

	return NULL;
}
size_t HIDDEN MemoryFindBackward(size_t start, size_t end, const unsigned char *pattern, const unsigned char *mask, size_t len)
{
	// Ensure start is higher than the end
	if (start < end)
	{
		size_t reverse = end;
		end = start;
		start = reverse;
	}

	unsigned char *cend = (unsigned char*)(end);
	unsigned char *current = (unsigned char*)(start - len);

	// Just linear search backward for sequence of bytes from the start minus pattern length till the end
	size_t i;
	if (mask)
	{
		// honoring mask
		while (current >= cend)
		{
			for (i = 0; i < len; i++)
			{
				if ((current[i] & mask[i]) != (pattern[i] & mask[i]))
					break;
			}

			if (i == len)
				return (size_t)(void*)current;

			current--;
		}
	}
	else
	{
		// without mask
		while (current >= cend)
		{
			for (i = 0; i < len; i++)
			{
				if (current[i] != pattern[i])
					break;
			}

			if (i == len)
				return (size_t)(void*)current;

			current--;
		}
	}

	return NULL;
}
size_t HIDDEN MemoryFindRefForwardPrefix8(size_t start, size_t end, size_t refAddress, uint8 prefixValue, bool relative)
{
	// Ensure start is lower than the end
	if (start > end)
	{
		size_t reverse = end;
		end = start;
		start = reverse;
	}

	unsigned char *cend = (unsigned char*)(end - 5 + 1);
	unsigned char *current = (unsigned char*)(start);

	// Just linear search for sequence of bytes from the start till the end minus pattern length
	while (current < cend)
	{
		if (*current == prefixValue)
		{
			if (relative)
			{
				if ((size_t)(*(size_t *)(current + 1) + current + 5) == refAddress)
					return (size_t)(void*)current;
			}
			else
			{
				if (*(size_t *)(current + 1) == refAddress)
					return (size_t)(void*)current;
			}
		}
		current++;
	}

	return NULL;
}

// Replaces double word on specified address with a new dword, returns old dword
uint32 HIDDEN HookDWord(size_t addr, uint32 newDWord)
{
	uint32 origDWord = *(size_t *)addr;
	EnablePageWrite(addr, sizeof(uint32));
	*(size_t *)addr = newDWord;
	RestorePageProtection(addr, sizeof(uint32));
	return origDWord;
}
// Exchanges bytes between memory address and bytes array
void HIDDEN ExchangeMemoryBytes(size_t origAddr, size_t dataAddr, uint32 size)
{
	EnablePageWrite(origAddr, size);
	unsigned char data[MAX_PATTERN];
	int32 iSize = size;
	while (iSize > 0)
	{
		size_t s = iSize <= MAX_PATTERN ? iSize : MAX_PATTERN;
		memcpy(data, (void *)origAddr, s);
		memcpy((void *)origAddr, (void *)dataAddr, s);
		memcpy((void *)dataAddr, data, s);
		iSize -= MAX_PATTERN;
	}
	RestorePageProtection(origAddr, size);
}

bool HIDDEN GetAddress(Module *module, Address *addr, size_t baseOffset)
{
	if (addr->originalAddress == NULL || baseOffset == NULL)
	{
		if (addr->symbolName != NULL)
		{
			// Find address under Linux
			size_t address = FindSymbol(module, addr->symbolName, addr->symbolIndex);
			if (address == NULL)
				return false;
			addr->originalAddress = address;
		}
		else
		{
			return false;
		}
	}
	else
	{
		addr->originalAddress += module->base - baseOffset;
	}

	return true;
}
bool HIDDEN HookFunction(Module *module, FunctionHook *hook)
{
	if (hook->originalAddress == NULL)
		return false;

	// Calculate and store offset for jump to our handler
	unsigned char patch[5];
	*(size_t *)&patch[1] = hook->handlerFunc - hook->originalAddress - 5;
	patch[0] = 0xE9;

	ExchangeMemoryBytes(hook->originalAddress, (size_t)patch, 5);

	return true;
}

void HIDDEN HookFunctionCall(void* hookWhat, void* hookAddr)
{
	unsigned char patch[5];
	*(size_t *)&patch[1] = (size_t)hookAddr - (size_t)hookWhat - 5;
	patch[0] = 0xE8;

	ExchangeMemoryBytes((size_t)hookWhat, (size_t)patch, 5);
}

bool HIDDEN FindDataRef(Module *module, AddressRef *ref)
{
	if (ref->originalAddress == NULL)
		return false;

	if (ref->addressRef) {
		if (ref->reverse)
			*(size_t*)ref->originalAddress = *(size_t*)ref->addressRef;
		else
			*(size_t*)ref->addressRef = ref->originalAddress;
	}

	return true;
}

#ifdef _WIN32
void FindAllCalls(Section* section, CFuncAddr** calls, uint32 findRefsTo)
{
	uint32 coderef_addr = section->start;
	coderef_addr = MemoryFindRefForwardPrefix8(coderef_addr, section->end, findRefsTo, 0xE8, true);
	while (coderef_addr) {
		CFuncAddr* cfa = new CFuncAddr(coderef_addr);
		cfa->Next = *calls;
		*calls = cfa;

		coderef_addr = MemoryFindRefForwardPrefix8(coderef_addr + 1, section->end, findRefsTo, 0xE8, true);
	}
}
#endif // _WIN32
