#pragma once

#include <cstdint>
#include <stdexcept>

#define ERROR_FOR_REASON(reason) \
class reason : public Error { \
public: \
explicit reason(const char* message) : Error(Reason::reason, message) {} \
};

namespace VdfParser::Errors {
  enum class Reason : uint8_t {
    UnexpectedCharacter
  };

  class Error : public std::runtime_error {
  public:
    Error(const Reason reason, const char* message) : std::runtime_error(message), reason(reason) {}

    [[nodiscard]] Reason getReason() const {
      return reason;
    }

  private:
    Reason reason;
  };

  ERROR_FOR_REASON(UnexpectedCharacter);
}

#undef ERROR_FOR_REASON
