#include "engine/filesystem/Filesystem.h"
#include "engine/filesystem/ADF.h"
#include <cctype>

ADFEntry::Token ADFEntry::Tokenizer::ReadToken() {
    char currchar;
    std::string TokenContent;

    // Skip any whitespace.
    do {
        currchar = stream->get();
    } while (std::isspace(currchar));

    // Any of the other cases
    switch (currchar) {
    case '\"':
        TokenContent.reserve(256); // TODO: Okay that was not a very bright idea, probably should make a preallocated buffer

        currchar = stream->get(); // Has to be like this as to not include the starting quotation mark.
        while (!(currchar == '\"' || currchar == -1)) {
            if (currchar == '\\') currchar = stream->get(); // For escaping special characters.

            TokenContent.push_back(currchar);
            currchar = stream->get();
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
            if (currchar == '\\') currchar = stream->get(); // For escaping special characters.

            TokenContent.push_back(currchar);
            currchar = stream->get();
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
                    ADFError("Incomplete ADF key/value pair!(And also it was closed with a square bracket)");
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
                case TokenType::EndFile:
                    return;
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
    auto Stream = Filesystem::GetFileAsStream(FilePath, std::ios::in | std::ios_base::binary);
    Tokenizer Tokenizer(&Stream, FilePath);
    auto filename = new Engine::UnmanagedResource<std::string>(FilePath);
    return ADFEntry(ADFType::map, Tokenizer, filename);
}
ENGINEEXPORT ADFEntry ADFEntry::FromStream(std::istream& Stream) {
    Tokenizer Tokenizer(&Stream, "[dynamic stream]");
    return ADFEntry(ADFType::map, Tokenizer, nullptr);
}

void ADFEntry::ToStreamStringFormatHelper(std::streambuf* buffer, const std::string& str) const {
    buffer->sputc('\"');
    for (char character : str) {
        if (character == '\"' || character == '\\') {
            buffer->sputc('\\');
        }
        buffer->sputc(character);
    }
    buffer->sputc('\"');
}

void ADFEntry::ToStreamObjectFormatHelper(std::streambuf* buffer, int IndentationLevel) const {
    if (IsString()) {
        ToStreamStringFormatHelper(buffer, std::get<std::string>(data));
    } else if (IsMap()) {
        buffer->sputc('{');
        if (HasChildren()) {
            buffer->sputc('\n');
            ToStream(buffer, IndentationLevel + 1);

            for (int i = 0; i < IndentationLevel; i++) {
                buffer->sputc('\t');
            }
        }

        buffer->sputc('}');
    } else {
        buffer->sputc('[');
        if (HasElements()) {
            buffer->sputc('\n');
            ToStream(buffer, IndentationLevel + 1);

            for (int i = 0; i < IndentationLevel; i++) {
                buffer->sputc('\t');
            }
        }

        buffer->sputc(']');
    }
}

ENGINEEXPORT void ADFEntry::ToStream(std::streambuf* buffer, int IndentationLevel) const {
    if (IsArray()) {
        const auto& array = GetArray();

        for (const auto& element : array) {
            for (int i = 0; i < IndentationLevel; i++) {
                buffer->sputc('\t');
            }

            element.ToStreamObjectFormatHelper(buffer, IndentationLevel);
            buffer->sputc('\n');
        }
    } else {
        const auto& map = GetMap();

        for (const auto& kvpair : map) {
            for (int i = 0; i < IndentationLevel; i++) {
                buffer->sputc('\t');
            }

            ToStreamStringFormatHelper(buffer, kvpair.first);
            buffer->sputc(' ');
            
            kvpair.second.ToStreamObjectFormatHelper(buffer, IndentationLevel);
            buffer->sputc('\n');
        }
    }
}




// Compacted exporting, with no formatting
void ADFEntry::ToStreamCompactObjectFormatHelper(std::streambuf* buffer) const {
    if (IsString()) {
        ToStreamStringFormatHelper(buffer, std::get<std::string>(data));
    } else if (IsMap()) {
        buffer->sputc('{');
        if (HasChildren()) {
            ToStreamCompact(buffer);
        }

        buffer->sputc('}');
    } else {
        buffer->sputc('[');
        if (HasElements()) {
            ToStreamCompact(buffer);
        }

        buffer->sputc(']');
    }
}

ENGINEEXPORT void ADFEntry::ToStreamCompact(std::streambuf* buffer) const {
    if (IsArray()) {
        const auto& array = GetArray();

        for (const auto& element : array) {
            element.ToStreamCompactObjectFormatHelper(buffer);
        }
    } else {
        const auto& map = GetMap();

        for (const auto& kvpair : map) {
            ToStreamStringFormatHelper(buffer, kvpair.first);
            
            kvpair.second.ToStreamCompactObjectFormatHelper(buffer);
        }
    }
}
