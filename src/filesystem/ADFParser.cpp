#include "engine/filesystem/Filesystem.h"
#include "engine/filesystem/ADF.h"
#include <cctype>

ADFEntry::Token ADFEntry::Tokenizer::ReadToken() {
    char currchar;
    std::string TokenContent;

    // Skip any whitespace.
    do {
        currchar = filestream.get();
    } while (std::isspace(currchar));

    // Any of the other cases
    switch (currchar) {
    case '\"':
        TokenContent.reserve(256);

        currchar = filestream.get(); // Has to be like this as to not include the starting quotation mark.
        while (!(currchar == '\"' || currchar == -1)) {
            if (currchar == '\\') currchar = filestream.get(); // For escaping special characters.

            TokenContent.push_back(currchar);
            currchar = filestream.get();
        }

        TokenContent.shrink_to_fit();

        return Token(TokenType::String, TokenContent);
    case '{':
        return Token(TokenType::StartMap);
    case '[':
        return Token(TokenType::StartArray);
    case '}':
        return Token(TokenType::EndMap);
    case ']':
        return Token(TokenType::EndArray);
    case eof:
        return Token(TokenType::EndFile);
    }

    // Unquoted string
    if (std::isgraph(currchar)) {
        TokenContent.reserve(256);

        do {
            if (currchar == '\\') currchar = filestream.get(); // For escaping special characters.

            TokenContent.push_back(currchar);
            currchar = filestream.get();
        } while (std::isgraph(currchar) && currchar != '{' && currchar != '}' && currchar != '[' && currchar != ']' && currchar != '\"');

        TokenContent.shrink_to_fit();

        return Token(TokenType::String, TokenContent);
    }

    Engine::Error("Unknown character in ADF file!(Is this even an ADF file?)(File: " + filepath + ")");
}

ADFEntry::ADFEntry(ADFType Type, Tokenizer& Tokenizer, Engine::Reference<std::string> filename) {
    Filename = filename;

    switch (Type) {
    
    case ADFType::map: {
        data = std::map<std::string, ADFEntry>();
        std::map<std::string, ADFEntry>& mapdata = std::get<std::map<std::string, ADFEntry>>(data);

        while (true) {
            Token KeyToken = Tokenizer.ReadToken();
            std::string key;

            switch (KeyToken.type) {
            case TokenType::String:
                key = std::move(KeyToken.content.value());
                break;
            case TokenType::StartMap:
                ADFError("A Map-type entry cannot be a key!");
            case TokenType::StartArray:
                ADFError("An Array-type entry cannot be a key!");
            case TokenType::EndArray:
                ADFError("Mismatched ADF closing brackets!(Tried to end a map with a square bracket)");
            case TokenType::EndMap:
            case TokenType::EndFile:
                return;
            }

            Token EntryToken = Tokenizer.ReadToken();

            switch (EntryToken.type) {
                case TokenType::String:
                    mapdata.emplace(std::move(key), ADFEntry(std::move(EntryToken.content.value()), filename));
                break;
                case TokenType::StartMap:
                    mapdata.emplace(std::move(key), ADFEntry(ADFType::map, Tokenizer, filename));
                break;
                case TokenType::StartArray:
                    mapdata.emplace(std::move(key), ADFEntry(ADFType::array, Tokenizer, filename));
                break;
                case TokenType::EndMap:
                case TokenType::EndFile:
                    ADFError("Incomplete ADF key/value pair!");
                case TokenType::EndArray:
                    ADFError("Incomplete ADF key/value pair!(And also it was closed with a square bracket!)");
            }
        }

        return;
    }
    case ADFType::array: {
        data = std::vector<ADFEntry>();
        std::vector<ADFEntry>& arraydata = std::get<std::vector<ADFEntry>>(data);

        while (true) {
            Token Token = Tokenizer.ReadToken();

            switch (Token.type) {
                case TokenType::String:
                    arraydata.emplace_back(ADFEntry(std::move(Token.content.value()), filename));
                break;
                case TokenType::StartMap:
                    arraydata.emplace_back(ADFEntry(ADFType::map, Tokenizer, filename));
                break;
                case TokenType::StartArray:
                    arraydata.emplace_back(ADFEntry(ADFType::array, Tokenizer, filename));
                break;
                case TokenType::EndArray:
                    return;
                case TokenType::EndFile:
                    ADFError("Incomplete ADF array!");
                case TokenType::EndMap:
                    ADFError("Mismatched ADF closing brackets!(Tried to end an array with a curly brace)");
            }
        }

        return;
    }
    
    }
}



void ADFEntry::ADFError(const std::string& error) const {
    std::ostringstream output;
    output << error;
    if (Filename) {
        output << "(File: " << *Filename << ")";
    } else {
        output << "(Undetermined name of file.(Dynamically generated?))";
    }

    Engine::Error(output.str());
}

ENGINEEXPORT ADFEntry ADFEntry::FromFile(const std::string& FilePath) {
    Tokenizer Tokenizer(FilePath);
    auto filename = new Engine::UnmanagedResource<std::string>(FilePath);
    return ADFEntry(ADFType::map, Tokenizer, filename);
}

void ADFEntry::ToFileStringFormatHelper(std::filebuf* buffer, const std::string& str) const {
    buffer->sputc('\"');
    for (char character : str) {
        if (character == '\"' || character == '\\') {
            buffer->sputc('\\');
        }
        buffer->sputc(character);
    }
    buffer->sputc('\"');
}

void ADFEntry::ToFileObjectFormatHelper(std::filebuf* buffer, int IndentationLevel) const {
    if (IsString()) {
        ToFileStringFormatHelper(buffer, std::get<std::string>(data));
    } else if (IsMap()) {
        buffer->sputc('{');
        if (HasChildren()) {
            buffer->sputc('\n');
            ToFile(buffer, IndentationLevel + 1);

            for (int i = 0; i < IndentationLevel; i++) {
                buffer->sputc('\t');
            }
        }

        buffer->sputc('}');
    } else {
        buffer->sputc('[');
        if (HasElements()) {
            buffer->sputc('\n');
            ToFile(buffer, IndentationLevel + 1);

            for (int i = 0; i < IndentationLevel; i++) {
                buffer->sputc('\t');
            }
        }

        buffer->sputc(']');
    }
}

ENGINEEXPORT void ADFEntry::ToFile(std::filebuf* buffer, int IndentationLevel) const {
    if (IsArray()) {
        const auto& array = GetArray();

        for (const auto& element : array) {
            for (int i = 0; i < IndentationLevel; i++) {
                buffer->sputc('\t');
            }

            element.ToFileObjectFormatHelper(buffer, IndentationLevel);
            buffer->sputc('\n');
        }
    } else {
        const auto& map = GetMap();

        for (const auto& kvpair : map) {
            for (int i = 0; i < IndentationLevel; i++) {
                buffer->sputc('\t');
            }

            ToFileStringFormatHelper(buffer, kvpair.first);
            buffer->sputc(' ');
            
            kvpair.second.ToFileObjectFormatHelper(buffer, IndentationLevel);
            buffer->sputc('\n');
        }
    }
}




// Compacted exporting, with no formatting
void ADFEntry::ToFileCompactObjectFormatHelper(std::filebuf* buffer) const {
    if (IsString()) {
        ToFileStringFormatHelper(buffer, std::get<std::string>(data));
    } else if (IsMap()) {
        buffer->sputc('{');
        if (HasChildren()) {
            ToFileCompact(buffer);
        }

        buffer->sputc('}');
    } else {
        buffer->sputc('[');
        if (HasElements()) {
            ToFileCompact(buffer);
        }

        buffer->sputc(']');
    }
}

ENGINEEXPORT void ADFEntry::ToFileCompact(std::filebuf* buffer) const {
    if (IsArray()) {
        const auto& array = GetArray();

        for (const auto& element : array) {
            element.ToFileCompactObjectFormatHelper(buffer);
        }
    } else {
        const auto& map = GetMap();

        for (const auto& kvpair : map) {
            ToFileStringFormatHelper(buffer, kvpair.first);
            
            kvpair.second.ToFileCompactObjectFormatHelper(buffer);
        }
    }
}