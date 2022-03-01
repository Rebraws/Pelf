#include "pelfExcept.h"

namespace pelf {

PelfException::PelfException(const char* msg) noexcept : mMsg(msg) {}

auto PelfException::what() const -> const char * {
  return mMsg;
}

}

