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
#include <mutex>    // for std::call_once


namespace pelf {


/** @brief PelfException class */
class PelfException : public std::exception {
public:

  /** @brief 
   *
   *  @param msg pointer to null-terminated string with explanatory information about the raised exception
   *
   * */
  explicit PelfException(const char* msg) noexcept;

  /** @brief Returns the error message threwed 
   *
   *  @return Returns pointer to a null-terminated string with explanatory information about the threw exception
   * */
  auto what() const noexcept -> const char * override;
private:
  const char* mMsg /**< Explanatory string */;
}; 

PelfException::PelfException(const char* msg) noexcept : mMsg(msg) {}

auto PelfException::what() const noexcept -> const char * {
  return mMsg;
}


}

#endif
