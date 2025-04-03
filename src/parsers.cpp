
#include <pc/pc.hpp>
#include <pc/combinators.hpp>

namespace pc::parsers {
    auto character(std::string_view input) -> Result<char> {
        if (input.empty()) {
            return failure;
        }
        return success(input[0], input.substr(1));
    }

    auto newline(std::string_view input) -> Result<char> {
        return combinators::filter(character, [](char c) { return c == '\n'; })(input);
    };

    auto line(std::string_view input) -> Result<std::string> {
        if (input.empty()) {
            return failure;
        }

        auto it = std::find_if(input.begin(), input.end(), [](char c) { return c == '\n'; });
        if (it == input.end()) {
            return success(std::string(input), std::string_view());
        }
        return success(std::string(input.begin(), it), std::string_view(it + 1, input.end()));
    }
} // namespace pc::parsers
