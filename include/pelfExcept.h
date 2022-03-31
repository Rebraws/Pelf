/** @file pelfExcept.h
 *  @brief PelfExcept class declaration
 *
 *  This file contains the PelfExcept class declaration
 *
 *
 *  @author Rebraws
 *
 * */

#ifndef PELFEXCEPT_H_
#define PELFEXCEPT_H_

#include <stdexcept>
#include <mutex>// for std::call_once


namespace pelf {

using namespace std::string_literals;

/** @brief PelfException class */
class PelfException : public std::exception
{
public:
  /** @brief
   *
   *  @param msg pointer to null-terminated string with explanatory information
   * about the raised exception
   *
   * */
  explicit PelfException(const char* msg) noexcept;

  /** @brief Returns the error message threwed
   *
   *  @return Returns pointer to a null-terminated string with explanatory
   * information about the threw exception
   * */
  auto what() const noexcept -> const char* override;

private:
  const char* mMsg /**< Explanatory string */;
};

PelfException::PelfException(const char* msg) noexcept : mMsg(msg) {}

auto PelfException::what() const noexcept -> const char* { return mMsg; }

class pelfInvalidSize : public PelfException
{
public:
  pelfInvalidSize(const char* msg, std::size_t size) noexcept;

  auto what() const noexcept -> const char* override;

private:
  std::string mStrMsg;
  std::size_t mSize;
};


pelfInvalidSize::pelfInvalidSize(const char* msg, std::size_t size) noexcept
  : PelfException(msg), mSize(size)
{
  mStrMsg = PelfException::what();
  mStrMsg += ", The size of the file is: "s;
  mStrMsg += std::to_string(mSize);

}

auto pelfInvalidSize::what() const noexcept -> const char* { return mStrMsg.c_str(); }

class pelfInvalidSignature : public PelfException {
  using PelfException::PelfException;
};


}// namespace pelf

#endif
