/** @file elfStructs.h
 *
 *  @brief ELF headers and sections structs
 *
 *  This file contains definition of structs that represent
 *  the internals of the ELF file format
 *
 *
 *  @author Rebraws
 * */


#ifndef ELFSTRUCTS_H_
#define ELFSTRUCTS_H_

#include <cstdint>

#include <boost/hana.hpp>

namespace hana = boost::hana;

namespace pelf {


inline constexpr int ELFCLASS64 = 2;
inline constexpr int PN_XNUM = 0xffff;
inline constexpr int SHN_LORESERVE = 0xff00;
inline constexpr unsigned char EI_MAG0{
  0x7f
}; /**< Magic number identifying the File as an ELF object file */
inline constexpr unsigned char EI_MAG1{
  'E'
}; /**< Magic number identifying the File as an ELF object file */
inline constexpr unsigned char EI_MAG2{
  'L'
}; /**< Magic number identifying the File as an ELF object file */
inline constexpr unsigned char EI_MAG3{
  'F'
}; /**< Magic number identifying the File as an ELF object file */
inline constexpr unsigned char EI_CLASS{
  0x2
}; /**< Magic number identifying the architecture (in this case 64-bit)*/
inline constexpr unsigned char EI_DATA{
  0x1
}; /**< Magic number identifying the data encoding of the processor-specific
      data in the file (in this case is Two's complement, little-endian.)*/


// inline constexpr std::uint8_t EI_NIDENT = 16; /**< The size of the e_ident
// array */


struct TableSizes
{
  std::size_t sectionTable{}; /**< Number of entries in the elf section table */
  std::size_t
    programHeader{}; /**< Number of entries in the elf program header table */
};

/** @brief Hana struct that represents the 64 bit elf header
 *
 * */
#pragma pack(push, 1)// Disable padding
struct Elf64_Ehdr
{
  /** @brief Macro that defines members of the structure
   *
   *
   * */
  BOOST_HANA_DEFINE_STRUCT(Elf64_Ehdr,
    (std::uint64_t, e_ident_high), // first 8 bytes of e_ident
    (std::uint64_t, e_ident_low),  // lasy 6 bytes of e_ident
    (std::uint16_t, e_type),
    (std::uint16_t, e_machine),
    (std::uint32_t, e_version),
    (std::uint64_t, e_entry),
    (std::uint64_t, e_phoff),
    (std::uint64_t, e_shoff),
    (std::uint32_t, e_flags),
    (std::uint16_t, e_ehsize),
    (std::uint16_t, e_phentsize),
    (std::uint16_t, e_phnum),// Number of entries in the program header table
    (std::uint16_t, e_shentsize),
    (std::uint16_t, e_shnum),//
    (std::uint16_t, e_shstrndx));
};
#pragma pack(pop)

/** @brief Hana struct that represents the program header table
 *
 * */
#pragma pack(push, 1)// Disable padding
struct Elf64_Phdr
{
  /** @brief Macro that defines members of the structure
   *
   *
   * */
  BOOST_HANA_DEFINE_STRUCT(Elf64_Phdr,
    (std::uint32_t, p_type),
    (std::uint32_t, p_flags),
    (std::uint64_t, p_offset),
    (std::uint64_t, p_vaddr),
    (std::uint64_t, p_paddr),
    (std::uint64_t, p_filesz),
    (std::uint64_t, p_memsz),
    (std::uint64_t, p_align));
};
#pragma pack(pop)


/** @brief Hana struct that represents the section header table
 *
 * */
#pragma pack(push, 1)// Disable padding
struct Elf64_Shdr
{
  /** @brief Macro that defines members of the structure
   *
   *
   * */
  BOOST_HANA_DEFINE_STRUCT(Elf64_Shdr,
    (std::uint32_t, sh_name),
    (std::uint32_t, sh_type),
    (std::uint64_t, sh_flags),
    (std::uint64_t, sh_addr),
    (std::uint64_t, sh_offset),
    (std::uint64_t, sh_size),//
    (std::uint32_t, sh_link),
    (std::uint32_t, sh_info),
    (std::uint64_t, sh_addralign),
    (std::uint64_t, sh_entsize));
};
#pragma pack(pop)

/**
 * @brief Structure that contains the Elf header and the program header table
 * from an ELF file
 *
 * @tparam NumOfProgramHeader Number of headers in the program header table
 */
template<std::size_t NumOfProgramHeader = 0> struct ElfHeaders
{
  template<std::size_t N>
  using ProgramHeaders = std::conditional_t<NumOfProgramHeader == 0,
    std::vector<Elf64_Phdr>,
    std::array<Elf64_Phdr, NumOfProgramHeader>>;


  Elf64_Ehdr elfHeader = {}; /**< Elf header */
  ProgramHeaders<NumOfProgramHeader>
    programHeaders = {}; /**< Array or vector containing all program headers from
                        the program header table*/
};


}// namespace pelf


#endif