#include "TextOperationApplier.hpp"

std::string TextOperationApplier::apply(const std::string& text)
{
    auto copy = text;
    for (auto const& ins : insertions)
    {
        unsigned insertionOffset = ins.first;
        auto textToInsert = ins.second;
        copy = copy.substr(0, insertionOffset) + textToInsert + copy.substr(insertionOffset);
    }
    for (auto const& rem : removals)
    {
        auto removalFrom = rem.first;
        auto removalTo = rem.second;
        copy = copy.substr(0, removalFrom) + copy.substr(removalTo);
    }
    return copy;
}

void TextOperationApplier::insertTextAt(const std::string& text, unsigned offset)
{
    insertions[offset] += text;
}

void TextOperationApplier::removeTextInRange(unsigned int from, unsigned int to) 
{
    removals[from] = to;
}
