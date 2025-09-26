#include <string>

class Parser {
public:
    Parser(const std::string& text) : pos(0), text(text) {}
    
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;

    [[nodiscard]] inline char peek()        const { return text[pos]; }
    // could make these skip n characters but we'll see if thats necessary....
    [[nodiscard]] inline char consume() { return text[pos++]; }
    inline void next() { pos++; };
    
    [[nodiscard]] size_t get_pos() const { return pos; }

private:
    size_t pos;
    const std::string text;
};
