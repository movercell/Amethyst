#include "engine/filesystem/Filesystem.h"
#include "engine/filesystem/ADF.h"
#include <cctype>

void ADFEntry::Tokenizer::ReadToken() {
    int currchar;

    // Skip any whitespace.
    do {
        currchar = buffer->sbumpc();
    } while (std::isspace(currchar));

    // Any of the other cases
    switch (currchar) {
    case '\"':
        CurrentContent.clear();

        currchar = buffer->sbumpc(); // Has to be like this as to not include the starting quotation mark.
        while (!(currchar == '\"' || currchar == eof)) {
            if (currchar == '\\') currchar = buffer->sbumpc(); // For escaping special characters.

            CurrentContent.push_back(currchar);
            currchar = buffer->sbumpc();
        }

        CurrentType = TokenType::String;
        return;
    case '{':
        CurrentType = TokenType::StartMap;
        return;
    case '[':
        CurrentType = TokenType::StartArray;
        return;
    case '}':
        CurrentType = TokenType::EndMap;
        return;
    case ']':
        CurrentType = TokenType::EndArray;
        return;
    case '/':
        currchar = buffer->sbumpc();

        // This type of comment.
        if (currchar == '/') {
            do {
                currchar = buffer->sbumpc();
            } while (!(currchar == '\n' || currchar == '\r' || currchar == eof));

            ReadToken();
            return;
        }
        
        /* This type of comment. */
        if (currchar == '*') {
            while (true) {
                do {
                    currchar = buffer->sbumpc();
                } while (!(currchar == '*' || currchar == eof));

                currchar = buffer->sbumpc();
                if (currchar == '/' || currchar == eof) {
                    ReadToken();
                    return;
                } else {
                    buffer->sungetc();
                }
            }
        }

        // This forward slash is actually a part of an unquoted string.
        buffer->sungetc();
        currchar = '/';
        break;
    case eof:
        CurrentType = TokenType::EndFile;
        return;
    }

    // Unquoted string
    if (std::isgraph(currchar)) {
        CurrentContent.clear();

        do {
            if (currchar == '\\') currchar = buffer->sbumpc(); // For escaping special characters.

            CurrentContent.push_back(currchar);
            currchar = buffer->sbumpc();
        } while (std::isgraph(currchar) && currchar != '{' && currchar != '}' && currchar != '[' && currchar != ']' && currchar != '\"');

        CurrentType = TokenType::String;
        return;
    }

    Engine::Error("Unknown character in an ADF file!(Is this even an ADF file?)(File: " + filepath + ")");
}

ADFEntry::ADFEntry(ADFType Type, Tokenizer& Tokenizer, Engine::Reference<std::string> filename) {
    Filename = filename;

    switch (Type) {
    
    case ADFType::map: {
        data = std::map<std::string, ADFEntry>();
        std::map<std::string, ADFEntry>& mapdata = std::get<std::map<std::string, ADFEntry>>(data);

        while (true) {
            Tokenizer.ReadToken();
            std::string key;

            switch (Tokenizer.GetCurrentTokenType()) {
            case TokenType::String:
                key = Tokenizer.GetCurrentTokenContent();
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

            Tokenizer.ReadToken();

            switch (Tokenizer.GetCurrentTokenType()) {
                case TokenType::String:
                    mapdata.emplace(std::move(key), ADFEntry(Tokenizer.GetCurrentTokenContent(), filename));
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
            Tokenizer.ReadToken();

            switch (Tokenizer.GetCurrentTokenType()) {
                case TokenType::String:
                    arraydata.emplace_back(ADFEntry(Tokenizer.GetCurrentTokenContent(), filename));
                break;
                case TokenType::StartMap:
                    arraydata.emplace_back(ADFEntry(ADFType::map, Tokenizer, filename));
                break;
                case TokenType::StartArray:
                    arraydata.emplace_back(ADFEntry(ADFType::array, Tokenizer, filename));
                break;
                case TokenType::EndMap:
                    ADFError("Mismatched ADF closing brackets!(Tried to end an array with a curly brace)");
                case TokenType::EndArray:
                case TokenType::EndFile:
                    return;
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

ADFEntry ADFEntry::FromFile(const std::string& FilePath) {
    auto Stream = Filesystem::GetFileAsStream(FilePath, std::ios::in | std::ios_base::binary);

    if (Stream.fail()) {
        Engine::Print("ADF file not found!(probably)(" + FilePath + (')'));
        return Map();
    }
    
    auto sentry = std::istream::sentry(Stream, true);
    if (!sentry) {
        Engine::Error("Failed to lock the input stream for an ADF parse!");
    }

    Tokenizer Tokenizer(Stream.rdbuf(), FilePath);
    auto filename = new Engine::UnmanagedResource<std::string>(FilePath);
    return ADFEntry(ADFType::map, Tokenizer, filename);
}
ADFEntry ADFEntry::FromStream(std::istream& Stream) {
    auto sentry = std::istream::sentry(Stream, true);
    if (!sentry) {
        Engine::Error("Failed to lock the input stream for an ADF parse!");
    }

    Tokenizer Tokenizer(Stream.rdbuf(), "[dynamic stream]");
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

void ADFEntry::ToStream(std::streambuf* buffer, int IndentationLevel) const {
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

void ADFEntry::ToStreamCompact(std::streambuf* buffer) const {
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



void ADFEntry::ToFile(const std::string& FilePath, bool isCompact) const {
    if (!IsMap()) {
        Engine::Error("Attempted to turn a non-Map-type ADF entry into a string, only a Map-type entry can be the root node of a tree!");
    }
    auto out = Filesystem::GetFileOutputStream(FilePath, std::ios::binary);

    auto sentry = std::ofstream::sentry(out);
    if (!sentry) {
        Engine::Error("Failed to create an output stream for an ADF export!");
    }

    auto buffer = out.rdbuf();
    if (isCompact) {
        ToStreamCompact(buffer);
    } else {
        ToStream(buffer, 0);
    }
}
void ADFEntry::ToStream(std::ostream Stream, bool isCompact) const {
    auto sentry = std::ostream::sentry(Stream);
    if (!sentry) {
        Engine::Error("Failed to lock the output stream for an ADF export!");
    }

    auto buffer = Stream.rdbuf();
    if (isCompact) {
        ToStreamCompact(buffer);
    } else {
        ToStream(buffer, 0);
    }
}
