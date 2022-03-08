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
#include <array>


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
  WindowsSpecificFields   mWsf{};
  std::array<IMAGE_DATA_DIRECTORY, IMAGE_NUMBER_OF_DIRECTORY_ENTRIES> mDataDirectories{};
};
#pragma pack(pop)


struct PeHeaders {
  IMAGE_FILE_HEADER mCoffHeader{}; /**< Struct that represents the coff header */
  OptionalHeader mOptionalHeader;  /**< Struct that represents the optional header */

  /* Methods to make access to headers more readable for the user */
  [[nodiscard]] constexpr auto getCoffHeader() const noexcept 
    -> IMAGE_FILE_HEADER {
    return mCoffHeader;
  }
  
  [[nodiscard]] constexpr auto getOptionalHeader() const noexcept -> OptionalHeader {
    return mOptionalHeader;
  }

  [[nodiscard]] constexpr auto getStandardCoffFields() const noexcept
    -> StandardCoffFields {
    return mOptionalHeader.mScf;
  } 

  [[nodiscard]] constexpr auto getWindowsSpecificFields() const noexcept
    -> WindowsSpecificFields {
    return mOptionalHeader.mWsf;
  }

  [[nodiscard]] constexpr auto getDataDirectories() const noexcept {
      return mOptionalHeader.mDataDirectories;
    }

};

#pragma pack(push, 1)
struct IMAGE_SECTION_HEADER {
  BOOST_HANA_DEFINE_STRUCT(IMAGE_SECTION_HEADER,
    (ULONGLONG,  Name),
    (ULONGLONG,  PhysAddressAndVirtSize),
    (DWORD, VirtualAddress),
    (DWORD, SizeOfRawData),
    (DWORD, PointerToRawData),
    (DWORD, PointerToRelocations),
    (DWORD, PointerToLinenumbers),
    (WORD,  NumberOfRelocations),
    (WORD,  NumberOfLinenumbers),
    (DWORD, Characteristics)
  );
}; 
#pragma pack(pop)


} // namespace end 

#endif
