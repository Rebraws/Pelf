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

namespace hana = boost::hana;


namespace pelf {

using WORD = std::uint16_t;
using DWORD = std::uint32_t;

#pragma pack(push, 1)     // Disable padding 
struct IMAGE_FILE_HEADER {
	BOOST_HANA_DEFINE_STRUCT(IMAGE_FILE_HEADER,
  	(WORD,  Machine),
  	(WORD,  NumberOfSections),
  	(DWORD, TimeDateStamp),
  	(DWORD, PointerToSymbolTable),
  	(DWORD, NumberOfSymbols),
  	(WORD,  SizeOfOptionalHeader),
  	(WORD,  Characteristics)
	);

	[[nodiscard]] constexpr auto getMachine() const -> WORD { 
		return hana::at_c<0>(hana::members(*this)); 
	}

	[[nodiscard]] constexpr auto getNumberOfSections() const -> WORD {
		return hana::at_c<1>(hana::members(*this)); 
	}

	[[nodiscard]] constexpr auto getTimeDateStamp() const -> DWORD {
		return hana::at_c<2>(hana::members(*this)); 
	}

	[[nodiscard]] constexpr auto getPointerToSymbolTable() const -> DWORD {
		return hana::at_c<3>(hana::members(*this));
	}

	[[nodiscard]] constexpr auto getNumberOfSymbols() const -> DWORD {
		return hana::at_c<4>(hana::members(*this));
	}

	[[nodiscard]] constexpr auto getSizeOfOptionalHeader() const -> WORD {
		return hana::at_c<5>(hana::members(*this));
	}

	[[nodiscard]] constexpr auto getCharacteristics() const -> WORD {
		return hana::at_c<6>(hana::members(*this));
	}
};
#pragma pack(pop)

} // namespace end 

#endif
