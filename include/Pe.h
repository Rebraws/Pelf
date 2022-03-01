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

private:
  friend class Pelf<Container, Pe>;

  static constexpr std::uint16_t  mMZDSignature{0x4d5a};  /**< MZ DOS Signature */
  static constexpr std::uint32_t  mPeSignture{0x50450000};          /**< PE Signature 'PE\0\0' */
  static constexpr std::uint8_t   mMinPeSize{97};         /**< Minimum possible PE file size */
  
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
};  

template <class Container>
inline constexpr Pe<Container>::Pe(Container data) :
  Pelf<Container, Pe>(std::move(data)) {}

template <class Container>
inline constexpr auto Pe<Container>::checkMZDSignature() const -> bool {

  return (this->mData[0] == (mMZDSignature >> 8) &&
         this->mData[1] == (mMZDSignature & 0xFF));
}

template <class Container>
inline constexpr auto Pe<Container>::checkPESignature() const -> bool {
  return true; 
  
}


template <class Container>
inline constexpr auto Pe<Container>::checkSignatures() const -> bool {
  return checkMZDSignature() && checkPESignature();
}

template <class Container>
inline constexpr auto Pe<Container>::checkFileSize() const -> bool {
  return this->mData.size() >= mMinPeSize;
}

} // end of namespace
#endif
