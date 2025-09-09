#pragma once

#include <functional>
#include <string>

namespace SourceParsers::Internal {
  class PeekableStream {
  public:
    explicit PeekableStream(std::string data);

    void discardWhile(const std::function<bool(char)>& predicate);

    [[nodiscard]] std::string consumeWhile(const std::function<bool(char)>& predicate);

    [[nodiscard]] char peek() const;

    [[nodiscard]] std::string peek(size_t count) const;

    void discard();

    [[nodiscard]] bool empty() const;

  private:
    std::string data;

    size_t index = 0;
  };
}
