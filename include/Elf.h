/** @file Elf.h
 *  @brief Elf class declaration
 *
 *  This file contains the Elf class declaration
 *
 *
 *  @author Rebraws
 *  */


#ifndef ELF_H_
#define ELF_H_


#include "Pelf.h"
#include "elfStructs.h"

#include <functional>

namespace pelf {


/** @brief Elf class
 *
 *  @tparam Container The type of the container used to store the file content
 *  that needs to be parsed
 *  @tparam NumOfSections Number of entries in the section table
 *  @tparam NumOfProgHeaders Number of entries in the program header table
 *
 * */
template<class Container,
  std::size_t NumOfSections = 0,
  std::size_t NumOfProgHeaders = 0>
class Elf : public Pelf<Container, Elf, NumOfSections, NumOfProgHeaders>
{
public:
  /** @brief Elf constructor
   *
   *  @param data data to be parsed
   * */
  constexpr explicit Elf(const Container& data);

  /**
   * @brief Get the Headers object
   *
   * @return ElfHeaders<NumOfProgHeaders>
   */
  [[nodiscard]] constexpr auto getHeaders() const noexcept
    -> ElfHeaders<NumOfProgHeaders>;

  /**
   * @brief Get the Sections object
   *
   * @return Table<Elf64_Shdr, NumOfSections>
   */
  [[nodiscard]] constexpr auto getSections() const noexcept
    -> Table<Elf64_Shdr, NumOfSections>;

  /**
   * @brief Get the Elf Section Table Size object
   *
   * @param data
   * @return consteval
   */

  /* Really bad design :( need to find a way to re-write this to avoid
    double parsing when the elf class is instantiated while also easy to use */
  static consteval auto getElfTablesSize(const Container& data)
    -> TableSizes requires
    container_and_convertible_v<Container, unsigned char>;

private
  :

  /* Member Variables */

  static constexpr std::uint8_t mMinElfSize{
    64
  }; /**< Minimum possible size for an ELF file*/
  static constexpr std::size_t mElfHeaderOffset{
    0
  }; /**< Represents the elf header offset (it always starts at zero)*/
  friend class Pelf<Container, Pe, NumOfSections, NumOfProgHeaders>;

  ElfHeaders<NumOfProgHeaders> mHeaders; /**< Struct that contains the elf
                          header and the program header table */

  Table<Elf64_Shdr, NumOfSections> mSections = {}; /**< Section header table */

  /* Private member functions */

  /**
   * @brief Get the Section Table Size
   *
   * @tparam Callable object
   * @param elfHeader Elf header of the file (already initialized)
   * @param read_struct function that reads a header and initializes it.
   * @return Returns the Number of Entries in the Elf section table
   */
  template<class Function>
  static constexpr auto getSectionTableSize(const Elf64_Ehdr& elfHeader,
    Function read_struct) -> std::size_t;

  /**
   * @brief Get the Program Table Size object
   *
   * @tparam Callable object
   * @param elfHeader Elf header of the file (already initialized)
   * @param read_struct function that reads a header and initializes it.
   * @return Returns the Number of Entries in the Elf program header table
   */
  template<class Function>
  static constexpr auto getProgramTableSize(const Elf64_Ehdr& elfHeader,
    Function read_struct) -> std::size_t;


  /**
   * @brief Checks if the elf file size is valid
   *
   * @return Returns `true` if the file size is valid, otherwise it returns
   * `false`
   */
  constexpr auto checkFileSize() const -> bool;

  /**
   * @brief Check if Elf file signatures are valid
   *
   * @return Returns `true` if signatures are valir, otherwise it returns
   * `false`
   */
  constexpr auto checkFileSignatures() const -> bool;
};


template<class Container,
  std::size_t NumOfSections,
  std::size_t NumOfProgHeaders>
constexpr Elf<Container, NumOfSections, NumOfProgHeaders>::Elf(
  const Container& data)
  : Pelf<Container, Elf, NumOfSections, NumOfProgHeaders>(data)
{
  if (!checkFileSize()) { throw PelfException{ "Invalid PE file size!" }; }

  
  this->parse();
}

template<class Container,
  std::size_t NumOfSections,
  std::size_t NumOfProgHeaders>
constexpr auto
  Elf<Container, NumOfSections, NumOfProgHeaders>::getHeaders() const noexcept
  -> ElfHeaders<NumOfProgHeaders>
{
  return mHeaders;
}


template<class Container,
  std::size_t NumOfSections,
  std::size_t NumOfProgHeaders>
template<class Function>
constexpr auto
  Elf<Container, NumOfSections, NumOfProgHeaders>::getSectionTableSize(
    const Elf64_Ehdr& elfHeader,
    Function read_struct) -> std::size_t
{
  /* If the offset of the section table is zero, then the size has to be zero */
  if (elfHeader.e_shoff == 0) { return 0; }

  /* If e_shnum holds a value greater or equal than SHN_LORESERVE , then the
     number of entries of the section table is in the variable sh_size which
     is member of the inital entry in section header table
      */
  if (elfHeader.e_shnum >= SHN_LORESERVE) {
    /* Read the initial entry in section header table*/
    Elf64_Shdr section_header = {};
    read_struct(section_header, elfHeader.e_shoff);
    return section_header.sh_size;
  }

  return elfHeader.e_shnum;
}

template<class Container,
  std::size_t NumOfSections,
  std::size_t NumOfProgHeaders>
template<class Function>
constexpr auto
  Elf<Container, NumOfSections, NumOfProgHeaders>::getProgramTableSize(
    const Elf64_Ehdr& elfHeader,
    Function read_struct) -> std::size_t
{
  /* If the offset of the program header table is zero, then the size has to be
   * zero */
  if (elfHeader.e_phoff == 0) { return 0; }

  /* If e_phnum holds a value greater or equal than PN_XNUM , then the
     number of entries of the program header table is in the variable sh_info
     which is member of the inital entry in section header table
      */
  if (elfHeader.e_phnum >= PN_XNUM) {
    /* Read the initial entry in section header table*/
    Elf64_Shdr section_header = {};
    read_struct(section_header, elfHeader.e_shoff);
    return section_header.sh_size;
  }

  return elfHeader.e_phnum;
}


template<class Container,
  std::size_t NumOfSections,
  std::size_t NumOfProgHeaders>
consteval auto
  Elf<Container, NumOfSections, NumOfProgHeaders>::getElfTablesSize(
    const Container& data)
    -> TableSizes requires container_and_convertible_v<Container, unsigned char>
{

  /* Lambda function that takes a reference to a header as a parameter
    and initializes its values by reading `data` */
  auto read_struct = [&](auto& header, std::size_t offset) {
    hana::for_each(hana::keys(header), [&](auto key) {
      auto& member = hana::at_key(header, key);

#pragma unroll
      for (std::size_t i{}; i < sizeof(member); ++i) {
        if (sizeof(member) > 1) { member <<= 8; }
        member |= data.at(sizeof(member) - 1 + offset - i);
      }

      offset += sizeof(member);
    });
  };


  TableSizes tables_size;

  /* Need to read some headers in order to extract the section table size
  and the program header table size */

  Elf64_Ehdr elf_header = {};

  /* Read the elf header */
  read_struct(elf_header, mElfHeaderOffset);

  tables_size.sectionTable = getSectionTableSize(elf_header, read_struct);

  tables_size.programHeader = getProgramTableSize(elf_header, read_struct);


  return tables_size;
}


template<class Container,
  std::size_t NumOfSections,
  std::size_t NumOfProgHeaders>
constexpr auto
  Elf<Container, NumOfSections, NumOfProgHeaders>::checkFileSize() const -> bool
{
  return this->mData.size() >= mMinElfSize;
}


template<class Container,
  std::size_t NumOfSections,
  std::size_t NumOfProgHeaders>
constexpr auto
  Elf<Container, NumOfSections, NumOfProgHeaders>::checkFileSignatures() const
  -> bool
{

  /* Read first 16 bytes to check magic numbers */
  unsigned char magic_numbers[16];

  std::copy(this->mData.begin(), this->mData.begin() + 16, magic_numbers);


  return magic_numbers[0] == EI_MAG0 && magic_numbers[1] == EI_MAG1
         && magic_numbers[2] == EI_MAG2 && magic_numbers[3] == EI_MAG3
         && magic_numbers[4] == EI_CLASS && magic_numbers[5] == EI_DATA;
}

}// namespace pelf


#endif