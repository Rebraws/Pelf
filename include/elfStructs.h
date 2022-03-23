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
// inline constexpr std::uint8_t EI_NIDENT = 16; /**< The size of the e_ident
// array */


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
    (std::uint16_t, e_ident),
    (std::uint16_t, e_type),
    (std::uint16_t, e_machine),
    (std::uint32_t, e_version),
    (std::uint64_t, e_entry),
    (std::uint64_t, e_phoff),
    (std::uint64_t, e_shoff),
    (std::uint32_t, e_flags),
    (std::uint16_t, e_ehsize),
    (std::uint16_t, e_phentsize),
    (std::uint16_t, e_phnum),
    (std::uint16_t, e_shentsize),
    (std::uint16_t, e_shnum),
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
    (std::uint64_t, sh_size),
    (std::uint32_t, sh_link),
    (std::uint32_t, sh_info),
    (std::uint64_t, sh_addralign),
    (std::uint64_t, sh_entsize));
};
#pragma pack(pop)


}// namespace pelf


#endif