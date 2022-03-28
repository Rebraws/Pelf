/** @file Pelf.h
 *  @brief Pelf class declaration
 *
 *  This file contains the Pelf class declaration, which is the base class
 *  of the Pe and Elf classes
 *
 *
 *  @author Rebraws
 *
 * */

#ifndef PELF_H_
#define PELF_H_

#include <type_traits>
#include <stdexcept>
#include "pelfExcept.h"
#include <peStructs.h>
#include <cassert>

#include <boost/hana.hpp>

namespace pelf {

namespace hana = boost::hana;

/** @brief Metafunction that returns an array if `Num` is
 *  zero, and a vector otherwise
 *
 *  @tparam Num Number of sections that table contains
 *  @tparam Struct Type of the elements the the array or vector contains
 *
 *  */
template<class Struct, std::size_t Num>
using Table = std::conditional_t<Num == 0,
  std::vector<Struct>,
  std::array<Struct, Num>>;


/**
 *
 * Checks if the template parameter Container has an alias named value_type
 *  and it also checks if that type is convertible to Type */
template<class Container, class Type>
concept container_and_convertible_v =
  std::is_nothrow_convertible_v<typename Container::value_type, Type>;


/** @brief Base class for Pe and Elf classes
 *
 *  @tparam Container The type of the container used to store the file content
 *  that needs to be parsed
 *
 *  @tparam Derived The type of the inherited class
 *  @tparam NumOfSections Number of Sections that the Pe or Elf file has
 *  */
template<class Container,
  template<typename, std::size_t...>
  class Derived,
  std::size_t... N>
class Pelf
{
public:
  /** @brief Pelf constructor
   *
   *  Takes as a parameter a container that satisfies the concept
   *  container_and_convertible_v<Container, unsigned char>
   *  and has_data_method_v<Container>
   *
   *  @param data The data to be parsed
   *  * */
  constexpr explicit Pelf(const Container& data) requires
    container_and_convertible_v<Container, unsigned char>;


  /** @brief Returns the original data passed to the Pe/Elf constructor
   *
   *  @return The original data stored in `mData`
   *
   * */
  [[nodiscard]] constexpr auto getRawData() const noexcept -> Container;

  /** @brief Returns a struct with the headers from the PE or ELF file
   *
   *
   *
   *  @return
   * */
  constexpr auto getHeaders() const noexcept -> void;


protected:
  Container mData; /**< Raw data from the file to be parsed */

  /** @brief Parses the PE/ELF headers and sections
   *
   *
   *  @return Void.
   *
   * */
  constexpr auto parse() -> void;


  /** @brief Reads the contents from `mData` at the index `offset` into the
   * struct passed by reference
   *
   *  @tparam Header struct that represents either a Pe header or an Elf header
   *
   *
   *  @return Void
   * */
  template<class Header>
  constexpr auto readHeader(Header& header, const std::ptrdiff_t offset)
    -> void;

  /** @brief Returns an Struct filled with the data from `mData` from index
   * `offset`
   *
   *  This function is only used if parsing is done at compile time, otherwise
   * std::copy() is used
   *
   *  @tparam Struct hana struct that represents a pe or elf header
   *  @param offset offset of the header
   *
   *  @return Returns a struct of the corresponding header
   *  */
  template<class Struct>
  [[nodiscard]] constexpr auto getStruct(std::size_t offset) -> Struct;
};


template<class Container,
  template<typename, std::size_t...>
  class Derived,
  std::size_t... N>
constexpr Pelf<Container, Derived, N...>::Pelf(const Container& data) requires
  container_and_convertible_v<Container, unsigned char> : mData(data)
{}


template<class Container,
  template<typename, std::size_t...>
  class Derived,
  std::size_t... N>
constexpr auto Pelf<Container, Derived, N...>::getRawData() const noexcept
  -> Container
{
  return mData;
}

template<class Container,
  template<typename, std::size_t...>
  class Derived,
  std::size_t... N>
constexpr auto Pelf<Container, Derived, N...>::getHeaders() const noexcept
  -> void
{
  static_cast<const Derived<Container, N...>&>(*this).getHeaders();
}


template<class Container,
  template<typename, std::size_t...>
  class Derived,
  std::size_t... N>
constexpr auto Pelf<Container, Derived, N...>::parse() -> void
{

  auto& pelf = static_cast<Derived<Container, N...>&>(*this);

  if (!pelf.checkFileSize()) { throw PelfException{ "Invalid File Size" }; }

  if (!pelf.checkSignatures()) { throw PelfException{ "Invalid signatures!" }; }

  pelf.parseHeaders();

  pelf.parseSections();
}


template<class Container,
  template<typename, std::size_t...>
  class Derived,
  std::size_t... N>
template<class Header>
constexpr auto Pelf<Container, Derived, N...>::readHeader(Header& header,
  const std::ptrdiff_t offset) -> void
{

  if (offset < 0) {
    throw PelfException{ "Invalid offset (negative), while parsing headers" };
  }

  if (std::is_constant_evaluated()) {
    header = this->template getStruct<Header>(static_cast<std::size_t>(offset));
  } else {

    assert(std::is_trivially_copyable<Header>::value);

    if (static_cast<std::size_t>(offset) + sizeof(header)
        < this->mData.size()) {
      std::copy(this->mData.begin() + offset,
        this->mData.begin() + offset + sizeof(header),
        reinterpret_cast<char*>(&header));
    } else {
      throw PelfException{ "Invalid Header" };
    }
  }
}


template<class Container,
  template<typename, std::size_t...>
  class Derived,
  std::size_t... N>
template<class Struct>
constexpr auto Pelf<Container, Derived, N...>::getStruct(std::size_t offset)
  -> Struct
{

  Struct s{};

  hana::for_each(hana::keys(s), [&](auto key) {
    auto& member = hana::at_key(s, key);

#pragma unroll
    for (std::size_t i{}; i < sizeof(member); ++i) {
      if (sizeof(member) > 1) { member <<= 8; }
      member |= mData.at(sizeof(member) - 1 + offset - i);
    }

    offset += sizeof(member);
  });

  return s;
}

}// end of namespace pelf

#endif
