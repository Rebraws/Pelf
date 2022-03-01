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

namespace pelf {



/** Checks if the template parameter Container has an alias named value_type
 *  and it also checks if that type is convertible to Type */  
template <class Container, class Type>
concept container_and_convertible_v = 
  std::is_nothrow_convertible_v<typename Container::value_type, Type>;


/** @brief Base class for Pe and Elf classes
 *
 *  @tparam Container The type of the container used to store the file content
 *  that needs to be parsed
 *
 *  @tparam Derived The type of the inherited class 
 *  */
template <class Container, template<typename> class Derived>
class Pelf {
public:
  
  /** @brief Pelf constructor
   *
   *  Takes as a parameter a container that satisfies the concept
   *  container_and_convertible_v<Container, unsigned char>
   *
   *  @param data The data to be parsed
   *  * */ 
  constexpr explicit Pelf(Container data)
    noexcept 
    requires container_and_convertible_v<Container, unsigned char>;

  /** @brief Parses the file contents stored in mData by calling internal member function _parse
   *
   *  @return Void.
   *
   * */
  constexpr auto parse() -> void;

  /** @brief Parses the data from `mData`, this member function can only be execute once per class instance
   *
   *  @return Void.
   * */
  constexpr auto _parse() -> void;


  /** @brief Returns the original data passed to the Pe/Elf constructor
   *
   *  @return The original data stored in `mData` 
   *
   * */
  [[nodiscard]] constexpr auto getRawData() const noexcept -> Container;




protected:
  Container mData; /**< Raw data from the file to be parsed */
  std::once_flag flag /**< Flag to ensure that parsing happens only once */;
};


template <class Container, template<typename> class Derived>
inline constexpr Pelf<Container, Derived>::Pelf(Container data) 
  noexcept
  requires container_and_convertible_v<Container, unsigned char> :
  mData(std::move(data)) {}



template <class Container, template<typename> class Derived>
inline constexpr auto Pelf<Container, Derived>::getRawData() const noexcept -> Container {
  return mData;
}


template <class Container, template<typename> class Derived>
inline constexpr auto Pelf<Container, Derived>::parse() -> void {

  std::call_once(flag, [&](){ 
      this->_parse();
      });
  }

template <class Container, template<typename> class Derived>
inline constexpr auto Pelf<Container, Derived>::_parse() -> void {
  
  auto& pelf = static_cast<Derived<Container>& >(*this);

  if (!pelf.checkFileSize()) {
    throw PelfException{"Invalid File Size"};
  }
  
  if (!pelf.checkSignatures()) {
    throw PelfException{"Invalid signatures!"};
  }

  //pelf.parseHeaders();
  //pelf.parseSections();


}

} // end of namespace pelf

#endif
