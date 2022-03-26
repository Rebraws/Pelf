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

private:
  friend class Pelf<Container, Pe, NumOfSections, NumOfProgHeaders>;

  ElfHeaders<NumOfProgHeaders> mHeaders; /**< Struct that contains the elf
                          header and the program header table */

  Table<Elf64_Shdr, NumOfSections> mSections = {}; /**< Section header table */
};


template<class Container,
  std::size_t NumOfSections,
  std::size_t NumOfProgHeaders>
constexpr Elf<Container, NumOfSections, NumOfProgHeaders>::Elf(
  const Container& data)
  : Pelf<Container, Elf, NumOfSections, NumOfProgHeaders>(data)
{}

template<class Container,
  std::size_t NumOfSections,
  std::size_t NumOfProgHeaders>
constexpr auto
  Elf<Container, NumOfSections, NumOfProgHeaders>::getHeaders() const noexcept
  -> ElfHeaders<NumOfProgHeaders>
{
  return mHeaders;
}


}// namespace pelf


#endif