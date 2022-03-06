/** @file peStructs.h 
 *  
 *  @brief Pe headers and sections structs 
 *
 *  This file contains definition of structs that represent
 *  the internals of the PE file format
 *
 *
 *  @author Rebraws
 * */

#ifndef PESTRUCTS_H_
#define PESTRUCTS_H_

#include <boost/hana.hpp>
#include <cstdint>



namespace pelf {

namespace hana = boost::hana;

/* Windows typedefs */
using WORD 			= std::uint16_t;
using DWORD 		= std::uint32_t;
using BYTE 			= std::uint8_t;
using ULONGLONG = std::uint64_t;

inline constexpr std::uint8_t IMAGE_NUMBER_OF_DIRECTORY_ENTRIES{16};


#pragma pack(push, 1)     // Disable padding 

/** @brief Struct that represents the COFF header format. It's 32/64 bit independent header
 *  
 *
 * */
struct IMAGE_FILE_HEADER {
  /** @brief Macro that defines members of the structure
   *
   *  
   * */
	BOOST_HANA_DEFINE_STRUCT(IMAGE_FILE_HEADER,
  	(WORD,  Machine),          
  	(WORD,  NumberOfSections),
  	(DWORD, TimeDateStamp),
  	(DWORD, PointerToSymbolTable),
  	(DWORD, NumberOfSymbols),
  	(WORD,  SizeOfOptionalHeader),
  	(WORD,  Characteristics)
	);

};
#pragma pack(pop)


#pragma pack(push, 1)   // disable padding
struct IMAGE_DATA_DIRECTORY {
  BOOST_HANA_DEFINE_STRUCT(IMAGE_DATA_DIRECTORY,  
		(DWORD,   VirtualAddress),
  	(DWORD,	 Size)
	);
}; 
#pragma pack(pop)

#pragma pack(push, 1)
struct StandardCoffFields {
	BOOST_HANA_DEFINE_STRUCT(StandardCoffFields,
    (WORD,                 Magic),
    (BYTE,                 MajorLinkerVersion),
    (BYTE,                 MinorLinkerVersion),
    (DWORD,                SizeOfCode),
    (DWORD,                SizeOfInitializedData),
    (DWORD,                SizeOfUninitializedData),
    (DWORD,                AddressOfEntryPoint),
    (DWORD,                BaseOfCode)
	);
};
#pragma pack(pop)

struct WindowsSpecificFields {
  BOOST_HANA_DEFINE_STRUCT(WindowsSpecificFields,
		
    (ULONGLONG,   ImageBase),
		(DWORD,       SectionAlignment),
		(DWORD,       FileAlignment),
		(WORD,        MajorOperatingSystemVersion),
		(WORD,        MinorOperatingSystemVersion),
		(WORD,        MajorImageVersion),
		(WORD,        MinorImageVersion),
		(WORD,        MajorSubsystemVersion),
		(WORD,        MinorSubsystemVersion),
		(DWORD,       Win32VersionValue),
		(DWORD,       SizeOfImage),
		(DWORD,       SizeOfHeaders),
		(DWORD,       CheckSum),
		(WORD,        Subsystem),
		(WORD,        DllCharacteristics),
		(ULONGLONG,   SizeOfStackReserve),
		(ULONGLONG,   SizeOfStackCommit),
		(ULONGLONG,   SizeOfHeapReserve),
		(ULONGLONG,   SizeOfHeapCommit),
		(DWORD,       LoaderFlags),
		(DWORD,       NumberOfRvaAndSizes)
 );
};


#pragma pack(push, 1)  // This can be removed for this structure
struct OptionalHeader {
  StandardCoffFields      mScf{};
  WindowsSpecificFields   mWfs{};
//  IMAGE_DATA_DIRECTORY  mDataDirectories[IMAGE_NUMBER_OF_DIRECTORY_ENTRIES];
};
#pragma pack(pop)









} // namespace end 

#endif
