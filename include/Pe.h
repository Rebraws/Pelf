/** @file Pe.h 
 *  @brief Pe class declaration
 *
 *  This file contains the Pe class declaration 
 *
 *
 *  @author Rebraws
 *  */


#ifndef PE_H_
#define PE_H_

#include "Pelf.h"
#include <algorithm>
#include <cstring>
#include <cassert>
#include "peStructs.h"
#include <boost/hana.hpp>

namespace pelf {



/** @brief Pe class 
 *
 *  @tparam Container The type of the container used to store the file content
 *  that needs to be parsed  
 *
 *
 * */

template <class Container>
class Pe : public Pelf<Container, Pe> {
public:

  /** @brief Pe constructor 
   *
   *  @param data data to be parsed
   * */
  constexpr explicit Pe(Container data);

  [[nodiscard]] constexpr auto testGetPeHeaderAddress() const -> unsigned int {
    return mPeHeaderAddress;
  }

  [[nodiscard]] constexpr auto getCoffHeader() const -> IMAGE_FILE_HEADER;


private:
  friend class Pelf<Container, Pe>;
  static constexpr std::uint16_t  mMZDSignature{0x4d5a};    /**< MZ DOS Signature */
  static constexpr std::uint32_t  mPeSignature{0x50450000};  /**< PE Signature 'PE\0\0' */
  static constexpr std::uint8_t   mMinPeSize{97};           /**< Minimum possible PE file size */
  
  std::uint32_t mPeHeaderAddress{}; /**< 32 bit value that represents the start address of the coff header */

  IMAGE_FILE_HEADER mCoffHeader{}; /**< Struct that represents the coff header */

  /** @brief  Checks if MZ DOS signature and PE signature are valid
   *
   *  @return Returns `true` if signatures are valid, and `false` otherwise
   * */
  [[nodiscard]] constexpr auto checkSignatures() const -> bool;


  /** @brief Checks if the MZ DOS Signature (0x5a4d) at offset 0 is valid 
   *
   *  @return Returns `true` if the signature is valid, `false` otherwise.
   * */
  [[nodiscard]] constexpr auto checkMZDSignature() const -> bool;
  
  /** @brief Checks if the PE Signature `PE\0\0` is valid
   *
   *  @return Returns `true` if the signature is valid, `false` otherwise.
   * */
  [[nodiscard]] constexpr auto checkPESignature() const -> bool;

    /** @brief Checks if the size is valid
   *
   *  @return Returns `true` if the size is valid, and `false` otherwise
   * */
  [[nodiscard]] constexpr auto checkFileSize() const -> bool;  


  /** @brief Parses the Coff Header, and the optional coff header
   *
   *
   * @return Void
   * */
  constexpr auto parseHeaders() -> void;

  /** @brief Reads the coff header from `mData` and saves it into mCoffHeader struct
   *
   *
   *  @return Void.
   * */
  constexpr auto readCoffHeader() -> void;

};  

template <class Container>
constexpr Pe<Container>::Pe(Container data) :
  Pelf<Container, Pe>(std::move(data)) {

  if (!checkFileSize()) {
    throw PelfException{"Invalid PE file size!"};
  }

  std::size_t offset{0x3c};
  for (std::size_t i{}; i < sizeof(mPeHeaderAddress); ++i) {
    mPeHeaderAddress <<= 8;
    mPeHeaderAddress |= static_cast<std::uint32_t>(
        this->mData[sizeof(mPeHeaderAddress) - 1 + offset--]);  
  }

}

template <class Container>
constexpr auto Pe<Container>::checkMZDSignature() const -> bool {

  return (this->mData[0] == (mMZDSignature >> 8) &&
         this->mData[1] == (mMZDSignature & 0xFF));
}

template <class Container>
constexpr auto Pe<Container>::checkPESignature() const -> bool {
  /* Read PE Signature from offset mPeHeaderAddress*/ 
  std::uint32_t pe_signature{};
  std::size_t offset = mPeHeaderAddress;

  /* mPeSignature is set as 0x5045000, so no need to read the value backwards */
  for (std::size_t i{}; i < sizeof(pe_signature); ++i) {
    pe_signature <<= 8;
    pe_signature |= this->mData[offset++]; 
  }


  return pe_signature == mPeSignature; 
    
}


template <class Container>
constexpr auto Pe<Container>::checkSignatures() const -> bool {
  return checkMZDSignature() && checkPESignature();
}

template <class Container>
constexpr auto Pe<Container>::checkFileSize() const -> bool {
  return this->mData.size() >= mMinPeSize;
}


template <class Container>
constexpr auto Pe<Container>::parseHeaders() -> void {
  
  readCoffHeader();

  

}

template <class Container>
constexpr auto Pe<Container>::readCoffHeader() -> void {
  if (std::is_constant_evaluated()){
    PelfHelper::copyStruct(this->mData, mCoffHeader, mPeHeaderAddress + 4);
  } else {
    assert(std::is_trivially_copyable<decltype(mCoffHeader)>::value);
    
    if (mPeHeaderAddress + 4 + sizeof(mCoffHeader) < this->mData.size()) {
      std::memcpy(reinterpret_cast<char *>(&mCoffHeader),
          this->mData.data() + mPeHeaderAddress + 4,
          sizeof(mCoffHeader));
    } else {
      throw PelfException{"Bad PE file, invalid Coff Header"};
    }
  }

}

template <class Container>
constexpr auto Pe<Container>::getCoffHeader() const -> IMAGE_FILE_HEADER {
  return mCoffHeader;
}

} // end of namespace

#endif