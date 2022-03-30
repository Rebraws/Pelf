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

#include "peStructs.h"
#include "Pelf.h"

#include <algorithm>
#include <cassert>
#include <vector>
#include <array>
#include <type_traits>

#include <boost/hana.hpp>

namespace pelf {


/** @brief Function template that returns the number of sections (size of the
 * section table)
 *
 *
 *  @tparam Container Container used to pass the data from the Pe file
 *  @param data Bytes from the Pe file
 *
 *  @return Returns a `std::uint16_t` that represents the number of sections
 *
 * */
template<class Container>
consteval auto getPeNumberOfSections(const Container& data) requires
  container_and_convertible_v<Container, unsigned char>
{

  /* The number of sections it's at offset 6 from the Coff header , so first we
   * need to read pe header address (at offset 0x3c) */
  std::uint32_t pe_header_address{};

  std::size_t offset{ 0x3c };


  for (std::size_t i{}; i < sizeof(pe_header_address); ++i) {
    pe_header_address <<= 8;
    pe_header_address |= static_cast<std::uint32_t>(
      data.at(sizeof(pe_header_address) - 1 + offset--));
  }

  /* Offset of the Number of Sections */
  offset = pe_header_address + sizeof(DWORD) + sizeof(WORD);

  WORD number_of_sections{};

  for (std::size_t i{}; i < sizeof(number_of_sections); ++i) {
    number_of_sections <<= 8;
    number_of_sections |= data.at(sizeof(number_of_sections) - 1 + offset - i);
  }

  return number_of_sections;
}


/** @brief Pe class
 *
 *  @tparam Container The type of the container used to store the file content
 *  that needs to be parsed
 *  @tparam NumOfSections Number of Sections of the Pe File (Size of the section
 * table)
 *
 * */
template<class Container, std::size_t NumOfSections = 0>
class Pe : public Pelf<Container, Pe<Container, NumOfSections>>
{
public:
  /** @brief Pe constructor
   *
   *  @param data data to be parsed
   * */
  constexpr explicit Pe(const Container& data);

  /** @brief Returns a struct that contains all Pe headers
   *
   *  @return Returns a struct `PeHeaders` that contains the coff header and
   *    the optional header, which are represented by the hana
   *    struct `IMAGE_FILE_HEADER` and the `OptionalHeader` struct
   *
   * */
  [[nodiscard]] constexpr auto getHeaders() const noexcept -> PeHeaders;

  /** @brief  Returns the section table from the Pe file
   *
   *  If the parsing is done at compile time it returns an array with all the
   *  sections from the Pe file, each section is represented by the hana struct
   *  `IMAGE_SECTION_HEADER`, on the other side, if parsing happens at runtime
   *  it returns a std::vector with all the sections.
   *
   *  @return Returns either an array or a vector that represents the section
   * table
   *
   * */
  [[nodiscard]] constexpr auto getSections() const noexcept
    -> Table<IMAGE_SECTION_HEADER, NumOfSections>;

private:
  friend class Pelf<Container, Pe<Container, NumOfSections>>;


  /* Variables */

  static constexpr std::uint16_t mMZDSignature{
    0x4d5a
  }; /**< MZ DOS Signature*/
  static constexpr std::uint32_t mPeSignature{ 0x50450000 }; /**< PE Signature
                                                               'PE\0\0' */
  static constexpr std::uint8_t mMinPeSize{
    97
  }; /**< Minimum possible PE file size */

  std::uint32_t mPeHeaderAddress{}; /**< 32 bit value that represents the start
                                       address of the coff header */

  PeHeaders mHeaders; /**< Struct that contains all Pe headers, more precisely
                           it contains the coff header (`IMAGE_FILE_HEADER`)
                           and the optional header (`OptionalHeader`)*/

  Table<IMAGE_SECTION_HEADER, NumOfSections>
    mSections = {}; /**< Container that represents the section table */

  /* Private member functions */


  /** @brief  Checks if MZ DOS signature and PE signature are valid
   *
   *  @return Returns `true` if signatures are valid, and `false` otherwise
   * */
  [[nodiscard]] constexpr auto checkSignatures() const -> bool;


  /** @brief Checks if the MZ DOS Signature (0x5a4d) at offset 0 is valid of the
   * Pe file
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
   *  Initializes the mHeaders struct by reading the coff header and the
   * optional header from `mData`
   *
   *
   * @return Void
   * */
  constexpr auto parseHeaders() -> void;

  /** @brief Parses the sections from the Pe
   *
   *
   *  Reads the section table from `mData` and saves it into `mSections`
   *
   *  @return Void.
   * */
  constexpr auto parseSections() -> void;

  /**
   * @brief Returns the offset of the section table
   *
   * @return std::ptrdiff_t That represents the offset of the section table
   */
  constexpr auto getSectionTableOffset() const -> std::ptrdiff_t;
};


template<class Container, std::size_t NumOfSections>
constexpr Pe<Container, NumOfSections>::Pe(const Container& data)
  : Pelf<Container, Pe<Container, NumOfSections>>(data)
{

  if (!checkFileSize()) { throw PelfException{ "Invalid PE file size!" }; }

  std::size_t offset{ 0x3c };
  for (std::size_t i{}; i < sizeof(mPeHeaderAddress); ++i) {
    mPeHeaderAddress <<= 8;
    mPeHeaderAddress |= static_cast<std::uint32_t>(
      this->mData.at(sizeof(mPeHeaderAddress) - 1 + offset--));
  }

  this->parse();
}

template<class Container, std::size_t NumOfSections>
constexpr auto Pe<Container, NumOfSections>::checkMZDSignature() const -> bool
{

  return (this->mData[0] == (mMZDSignature >> 8)
          && this->mData[1] == (mMZDSignature & 0xFF));
}

template<class Container, std::size_t NumOfSections>
constexpr auto Pe<Container, NumOfSections>::checkPESignature() const -> bool
{
  /* Read PE Signature from offset mPeHeaderAddress*/
  std::uint32_t pe_signature{};
  std::size_t offset = mPeHeaderAddress;

  /* mPeSignature is set as 0x5045000, so no need to read the value backwards */
  for (std::size_t i{}; i < sizeof(pe_signature); ++i) {
    pe_signature <<= 8;
    pe_signature |= this->mData.at(offset++);
  }


  return pe_signature == mPeSignature;
}


template<class Container, std::size_t NumOfSections>
constexpr auto Pe<Container, NumOfSections>::checkSignatures() const -> bool
{
  return checkMZDSignature() && checkPESignature();
}

template<class Container, std::size_t NumOfSections>
constexpr auto Pe<Container, NumOfSections>::checkFileSize() const -> bool
{
  return this->mData.size() >= mMinPeSize;
}


template<class Container, std::size_t NumOfSections>
constexpr auto Pe<Container, NumOfSections>::getSectionTableOffset() const
  -> std::ptrdiff_t
{
  return mPeHeaderAddress + sizeof(mHeaders.mCoffHeader)
         + sizeof(mHeaders.mOptionalHeader.mDataDirectories)
             * IMAGE_NUMBER_OF_DIRECTORY_ENTRIES
         + sizeof(mHeaders.mOptionalHeader.mWsf)
         + sizeof(mHeaders.mOptionalHeader.mScf);
}

template<class Container, std::size_t NumOfSections>
constexpr auto Pe<Container, NumOfSections>::parseHeaders() -> void
{

  std::ptrdiff_t offset = mPeHeaderAddress + 4;

  this->readHeader(mHeaders.mCoffHeader, offset);

  offset += sizeof(mHeaders.mCoffHeader);

  this->readHeader(mHeaders.mOptionalHeader.mScf, offset);

  offset += sizeof(mHeaders.mOptionalHeader.mScf);

  this->readHeader(mHeaders.mOptionalHeader.mWsf, offset);

  offset += sizeof(mHeaders.mOptionalHeader.mWsf);

  for (auto& data_dir : mHeaders.mOptionalHeader.mDataDirectories) {
    this->readHeader(data_dir, offset);
    offset += sizeof(data_dir);
  }
}


template<class Container, std::size_t NumOfSections>
constexpr auto Pe<Container, NumOfSections>::getHeaders() const noexcept
  -> PeHeaders
{
  return mHeaders;
}


template<class Container, std::size_t NumOfSections>
constexpr auto Pe<Container, NumOfSections>::parseSections() -> void
{

  /* Compute section table offset */
  std::ptrdiff_t offset = getSectionTableOffset();

  if constexpr (!std::is_same_v<decltype(mSections),
                  std::vector<IMAGE_SECTION_HEADER>>) {

    for (auto& section : mSections) {
      this->readHeader(section, offset);
      offset += sizeof(section);
    }
  } else {
    for (int i{}; i < mHeaders.mCoffHeader.NumberOfSections; ++i) {
      IMAGE_SECTION_HEADER tmp_section{};
      this->readHeader(tmp_section, offset);
      offset += sizeof(tmp_section);
      mSections.push_back(tmp_section);
    }
  }
}


template<class Container, std::size_t NumOfSections>
constexpr auto Pe<Container, NumOfSections>::getSections() const noexcept
  -> Table<IMAGE_SECTION_HEADER, NumOfSections>
{
  return mSections;
}

}// namespace pelf

#endif
