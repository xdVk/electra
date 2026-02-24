# TODO

## Immediate
- [ ] Implement the `Lexer` class.
    - Constructor: accept `[start, end)` pointers.
    - Method `lex()`: iterate through characters and produce tokens.
- [ ] Add basic token types.
- [ ] Test file reading + lexer integration together.

## Medium-term
- [ ] Implement token struct with kind/value.
- [ ] Handle errors gracefully in lexer (unexpected characters, EOF).
- [ ] Add unit tests for lexer with small example files.

## Long-term
- [ ] Implement parser on top of lexer.
- [ ] Add AST node types.
- [ ] Use `mmap` to avoid copying large files into memory.
    - Create or use a C++ style wrapper.
    - Add windows supports
