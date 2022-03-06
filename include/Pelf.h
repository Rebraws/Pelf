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


#include <boost/hana.hpp>


namespace pelf {

namespace hana = boost::hana;


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
 *  */
template<class Container, template<typename> class Derived>
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
  constexpr explicit Pelf(Container data) noexcept
    requires container_and_convertible_v<Container, unsigned char>;


  /** @brief Returns the original data passed to the Pe/Elf constructor
   *
   *  @return The original data stored in `mData`
   *
   * */
  [[nodiscard]] constexpr auto getRawData() const noexcept -> Container;


protected:
  Container mData; /**< Raw data from the file to �be parsed */

  /** @brief Parses the PE/ELF headers and sections
   *
   *
   *  @return Void.
   *
   * */
  constexpr auto parse() -> void;


  /** @brief Reads the contents from `mData` at the index `offset` into the struct passed by reference 
   *
   *  @return Void
   * */
  template <class Header>
  constexpr auto readHeader(Header& header, const std::ptrdiff_t offset) -> void;

  /** @brief Returns an Struct filled with the data from `mData` from index `offset`
   *  
   *  This function is only used if parsing is done at compile time, otherwise std::copy() is used
   *
   *
   *  @return `Struct`
   *  */
  template<class Struct>
  constexpr auto getStruct(std::size_t offset) -> Struct;
};


template<class Container, template<typename> class Derived>
constexpr Pelf<Container, Derived>::Pelf(Container data) noexcept
  requires container_and_convertible_v<Container, unsigned char>
  : mData(std::move(data)) {}


template<class Container, template<typename> class Derived>
constexpr auto Pelf<Container, Derived>::getRawData() const noexcept -> Container
{
  return mData;
}


template<class Container, template<typename> class Derived>
constexpr auto Pelf<Container, Derived>::parse() -> void
{

  auto &pelf = static_cast<Derived<Container> &>(*this);

  if (!pelf.checkFileSize()) {
    throw PelfException{ "Invalid File Size" };
  }

  if (!pelf.checkSignatures()) {
    throw PelfException{ "Invalid signatures!" };
  }

  pelf.parseHeaders();

  // pelf.parseSections();
}



template <class Container, template<typename> class Derived>
template <class Header>
constexpr auto Pelf<Container, Derived>::readHeader(Header& header, 
    const std::ptrdiff_t offset) -> void {

  if (offset < 0) {
    throw PelfException{"Invalid offset (negative), while parsing headers"};
  }

  if (std::is_constant_evaluated()) {
    header = this->template getStruct<Header>(static_cast<std::size_t>(offset));
  } else {

    assert(std::is_trivially_copyable<Header>::value);

    if (static_cast<std::size_t>(offset) + sizeof(header) < this->mData.size()) {
      std::copy(this->mData.begin() + offset,
          this->mData.begin() + sizeof(header),
          reinterpret_cast<char *>(&header));
    } else {
      throw PelfException{ "Invalid Header" };
    }
  } 
}


template<class Container, template<typename> class Derived>
template<class Struct>
constexpr auto Pelf<Container, Derived>::getStruct(std::size_t offset)
  -> Struct
{

  Struct s{};

  hana::for_each(hana::keys(s), [&](auto key) {
    auto &member = hana::at_key(s, key);
    
    #pragma unroll
    for (std::size_t i{}; i < sizeof(member); ++i) {
      if (sizeof(member) > 1) {
        member <<= 8;
      }
      member |= mData.at(sizeof(member) - 1 + offset - i);
    }

    offset += sizeof(member);
  });

  return s;
}

}// end of namespace pelf

#endif
