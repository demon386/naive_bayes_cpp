#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP
#include <vector>

class Example
{
public:
    Example(std::vector<std::string> words, std::string label) :
        _words(words), _label(label) {}
    Example(std::vector<std::string>&& words, std::string&& label) :
        _words(std::move(words)), _label(std::move(label)) {}

public:
    std::vector<std::string> _words;
    std::string _label;
};

#endif
