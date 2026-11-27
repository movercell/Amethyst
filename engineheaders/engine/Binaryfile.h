#pragma once

#include <meta>
#include <algorithm>
#include <variant>
#include <cstring>
#include "CPPArenaAllocator/ArenaAllocator.h"
#include "engine/master.h"
#include "xxhash.h"

#define BINARY_FILE_VALIDATE_CHECKSUM

namespace Engine {

    // Need forward declaration for friendliness
    template<typename T>
    class BinaryFileConstructor;
    template<typename... Versions>
    class BinaryFileImporter;

    enum class BinaryFileStringLump : char {};

    namespace Internal {
        struct BinaryFileAnnotation {
            const uint32_t MagicNumber;
            const uint32_t Version;
        };

        template<typename T, typename... Unused>
        const std::string GetIdentifier() {
            constexpr std::meta::info info = ^^T;

            if constexpr (std::meta::has_identifier(info)) {
                return std::string(std::meta::identifier_of(std::meta::dealias(info)));
            }

            return "No identifier";
        }

        inline constexpr BinaryFileStringLump NullStringLump = BinaryFileStringLump('\0');

        struct BinaryFileCheckum {
            std::array<uint32_t, 2> data;

            BinaryFileCheckum(uint64_t from) {
                data[0] = uint32_t(from);
                data[1] = uint32_t(from >> 32);
            }
            operator uint64_t() {
                return uint64_t(data[1]) << 32 | uint64_t(data[0]);
            }
        };
    }


    template<typename LumpType>
    class BinaryFileLump {
        [[noreturn]] void OOBError() { Engine::Error("Lump addressed out of bounds!(" + Internal::GetIdentifier<LumpType>() + ")"); }
        void InvalidCheck() { if (IsInvalid()) Engine::Error("Attempted to read an invalid lump!(" + Internal::GetIdentifier<LumpType>() + ")"); }

        // When on disk this is an offset from start of file to the lump data, when in memory it is a global pointer.
        LumpType* LumpPtr = nullptr;
        // When on disk this is the size of the lump in bytes, when in memory it is a count of elements that the lump contains.
        std::uintptr_t LumpSize = 0;

        template <typename T>
        friend class BinaryFileConstructor;
        template<typename... Versions>
        friend class BinaryFileImporter;
    public:
        using iterator = LumpType*;
        using const_iterator = LumpType*;

        LumpType& operator[](uint64_t i) { InvalidCheck(); if (i < LumpSize) return LumpPtr[i]; OOBError(); }
        const LumpType& operator[](uint64_t i) const { InvalidCheck(); if (i < LumpSize) return LumpPtr[i]; OOBError(); }
        LumpType& at(uint64_t i) { InvalidCheck(); if (i < LumpSize) return LumpPtr[i]; OOBError(); }
        const LumpType& at(uint64_t i) const { InvalidCheck(); if (i < LumpSize) return LumpPtr[i]; OOBError(); }

        iterator begin() { InvalidCheck(); return LumpPtr; }
        iterator end() { InvalidCheck(); return LumpPtr + LumpSize; }
        auto rbegin() { InvalidCheck(); return std::make_reverse_iterator(end()); }
        auto rend() { InvalidCheck(); return std::make_reverse_iterator(begin()); }
        const_iterator cbegin() { InvalidCheck(); return LumpPtr; }
        const_iterator cend() { InvalidCheck(); return LumpPtr + LumpSize; }
        auto crbegin() { InvalidCheck(); return std::make_reverse_iterator(end()); }
        auto crend() { InvalidCheck(); return std::make_reverse_iterator(begin()); }

        size_t size() { return LumpSize; }

        bool IsInvalid() { return LumpPtr == nullptr || LumpSize == 0; }
    };


    class BinaryFileBaseHeader {
        uint32_t MagicNumber = 0;
        Internal::BinaryFileCheckum Checksum = 0x8888888888888888;
        uint32_t Version = 0xFFFFFFFF;

        template <typename T>
        friend class BinaryFileConstructor;
        template<typename... Versions>
        friend class BinaryFileImporter;
    };


    //! \brief Constructor for binary files. Be REALLY careful when using this.
    template<typename T>
    class BinaryFileConstructor {
        mem::ArenaAllocator Allocator;
        T* Header;

        template<typename TObj>
        void Export_Impl(TObj& Object) {
            static constexpr auto Members = std::define_static_array(std::meta::nonstatic_data_members_of(^^TObj, std::meta::access_context::unchecked()));

            template for (constexpr auto Member : Members) {
                constexpr auto TMember = std::meta::dealias(^^decltype(Object.[:Member:]));
                using TMember_t = [:TMember:];

                if constexpr (std::meta::is_pointer_type(TMember)) {
                    static_assert(false, "Attempted to export a pointer, nope.");
                }

                // If lump.
                if constexpr (std::meta::has_template_arguments(TMember) && std::meta::template_of(TMember) == ^^BinaryFileLump) {
                    auto LumpPtrCopy = Object.[:Member:].LumpPtr;
                    constexpr auto LumpType = std::meta::remove_pointer(std::meta::type_of(^^LumpPtrCopy));
                    using LumpType_t = [:LumpType:];

                    if (!Object.[:Member:].IsInvalid()) {

                        if constexpr (LumpType == ^^BinaryFileStringLump) {
                            if (*(Object.[:Member:].LumpPtr + Object.[:Member:].LumpSize - 1) != Internal::NullStringLump) {
                                Engine::Error("BinaryFileConstructor::Export: Silly, you forgot the null terminator!");
                            }
                        }

                        // Correct the address and size.
                        Object.[:Member:].LumpPtr = (LumpType_t*)(std::uintptr_t(Object.[:Member:].LumpPtr) - std::uintptr_t(Header));
                        Object.[:Member:].LumpSize *= sizeof(LumpType_t);
                    }

                } else if constexpr (std::is_class_v<TMember_t> && !std::is_union_v<TMember_t>) {
                    Export_Impl(Object.[:Member:]);
                }

            }

            // Handle the bases as well.
            static constexpr auto Bases = std::define_static_array(std::meta::bases_of(^^TObj, std::meta::access_context::unchecked()));
            template for (constexpr auto Base : Bases) {
                using BaseType = [:std::meta::type_of(Base):];
                Export_Impl<BaseType>(Object);
            }
        }

    public:
        BinaryFileConstructor(size_t Size = 33554432) : Allocator(Size, "File constructor") {
            static_assert(std::is_base_of_v<BinaryFileBaseHeader, T>, "A binary file class must be an extension of BinaryFileBaseHeader!");
            constexpr auto BinaryFileAnnotations = std::define_static_array(std::meta::annotations_of_with_type(^^T, ^^Engine::Internal::BinaryFileAnnotation));
            static_assert(BinaryFileAnnotations.size() == 1, "A binary class must have exactly 1 BinaryFile annotation!");

            Header = Allocator.New<T>();

            constexpr auto BinaryFileAnnotation = std::meta::extract<Engine::Internal::BinaryFileAnnotation>(BinaryFileAnnotations[0]);
            Header->MagicNumber = BinaryFileAnnotation.MagicNumber;
            Header->Version = BinaryFileAnnotation.Version;
        }

        T* GetHeader() { return Header; }

        template<typename LumpType>
        BinaryFileLump<LumpType> CreateLump(std::span<LumpType> From) {
            static_assert(std::is_trivially_destructible_v<LumpType>);

            BinaryFileLump<LumpType> ret;
            ret.LumpSize = From.size();
            ret.LumpPtr = Allocator.NewArray<LumpType>(From.size());
            std::copy(From.begin(), From.end(), ret.LumpPtr);

            return ret;
        }

        //! Note: Invalidates the constructor's internal data since it is in the exported state.
        std::span<std::byte> Export() {
            Export_Impl(*Header);
            auto buffer = Allocator.GetBuffer();

            constexpr int offset = offsetof(BinaryFileBaseHeader, Version);
            Header->Checksum = XXH3_64bits(buffer.data() + offset, buffer.size_bytes() - offset);

            return buffer;
        }

    };

    template<typename... Versions>
    class BinaryFileImporter {

        template<typename TObj, typename T>
        static void Import_Impl(TObj& Object, T* Fileaddress, uintptr_t EndOfFile, const std::string& Filename) {
            static constexpr auto Members = std::define_static_array(std::meta::nonstatic_data_members_of(^^TObj, std::meta::access_context::unchecked()));

            template for (constexpr auto Member : Members) {
                constexpr auto TMember = std::meta::dealias(^^decltype(Object.[:Member:]));
                using TMember_t = [:TMember:];

                if constexpr (std::meta::is_pointer_type(TMember)) {
                    static_assert(false, "Attempted to import a pointer, nope.");
                }

                // If lump.
                if constexpr (std::meta::has_template_arguments(TMember) && std::meta::template_of(TMember) == ^^BinaryFileLump) {
                    auto LumpPtrCopy = Object.[:Member:].LumpPtr;
                    constexpr auto LumpType = std::meta::remove_pointer(std::meta::type_of(^^LumpPtrCopy));
                    using LumpType_t = [:LumpType:];

                    if (Object.[:Member:].IsInvalid()) continue;

                    // Correct the address and size.
                    auto LumpStart = std::uintptr_t(Object.[:Member:].LumpPtr) + std::uintptr_t(Fileaddress);
                    auto LumpDataSize = Object.[:Member:].LumpSize;
                    auto LumpSize = LumpDataSize / sizeof(LumpType_t);
                    Object.[:Member:].LumpPtr = std::start_lifetime_as_array<LumpType_t>((void*)(LumpStart), LumpSize);
                    Object.[:Member:].LumpSize = LumpSize;
                    
                    // Protections.
                    if (LumpStart + LumpDataSize > EndOfFile) {
                        Engine::Error("BinaryFileImporter: Lump goes past end of file.(" + Internal::GetIdentifier<LumpType_t>() + ")(File: " + Filename + ")");
                    }
                    if (LumpDataSize % sizeof(LumpType_t)) {
                        Engine::Error("BinaryFileImporter: Bad lump size.(" + Internal::GetIdentifier<LumpType_t>() + ")(File: " + Filename + ")");
                    }
                    if constexpr (LumpType == ^^BinaryFileStringLump) {
                        if (!Object.[:Member:].IsInvalid() && *(BinaryFileStringLump*)(LumpStart + LumpSize - 1) != Internal::NullStringLump) {
                            Engine::Error("BinaryFileImporter: String lump lacks null terminator.(" + Internal::GetIdentifier<TObj>() + ")(File: " + Filename + ")");
                        }
                    }

                } else if constexpr (std::is_class_v<TMember_t> && !std::is_union_v<TMember_t>) {
                    Import_Impl(Object.[:Member:], Fileaddress, EndOfFile, Filename);
                }

            }

            // Handle the bases as well.
            static constexpr auto Bases = std::define_static_array(std::meta::bases_of(^^TObj, std::meta::access_context::unchecked()));
            template for (constexpr auto Base : Bases) {
                using BaseType = [:std::meta::type_of(Base):];
                Import_Impl<BaseType>(Object, Fileaddress, EndOfFile, Filename);
            }
        }

        template<typename T>
        static T* Import_Invoker(std::span<std::byte> data, const std::string& Filename) {
            T* Data = std::start_lifetime_as<T>(data.data());

            static_assert(std::is_base_of_v<BinaryFileBaseHeader, T>, "A binary file class must be an extension of BinaryFileBaseHeader!");
            constexpr auto BinaryFileAnnotations = std::define_static_array(std::meta::annotations_of_with_type(^^T, ^^Engine::Internal::BinaryFileAnnotation));
            static_assert(BinaryFileAnnotations.size() == 1, "A binary class must have exactly 1 BinaryFile annotation!");
            constexpr auto BinaryFileAnnotation = std::meta::extract<Engine::Internal::BinaryFileAnnotation>(BinaryFileAnnotations[0]);
            
            if (data.size_bytes() < sizeof(T)) {
                Engine::Error("BinaryFileImporter: Bad file size.(Not enough space for header.)(" + std::string(std::meta::identifier_of(^^T)) + ")(File: " + Filename + ")");
            }
            if (Data->MagicNumber != BinaryFileAnnotation.MagicNumber) {
                Engine::Error("BinaryFileImporter: Bad file magic number.(" + std::string(std::meta::identifier_of(^^T)) + ")(File: " + Filename + ")");
            }
            
            Import_Impl(*Data, Data, uintptr_t(Data) + data.size(), Filename);
            return Data;
        }

    public:

        static std::variant<Versions*...> Import(std::span<std::byte> data, const std::string& Filename) {
            static constexpr std::array versionarray = {^^Versions...};

            if (data.size_bytes() < sizeof(BinaryFileBaseHeader)) {
                Engine::Error("BinaryFileImporter: Bad file size.(Not enough space for base header.)(File: " + Filename + ")");
            }

            BinaryFileBaseHeader* BaseHeader = std::start_lifetime_as<BinaryFileBaseHeader>(data.data());


        #ifdef BINARY_FILE_VALIDATE_CHECKSUM
            constexpr int offset = offsetof(BinaryFileBaseHeader, Version);
            if (BaseHeader->Checksum != XXH3_64bits(data.data() + offset, data.size_bytes() - offset)) {
                Engine::Error("BinaryFileImporter: File failed checksum!(File: " + Filename + ")");
            }
        #endif


            template for (constexpr auto version : versionarray) {
                using version_t = [:version:];

                constexpr auto BinaryFileAnnotations = std::define_static_array(std::meta::annotations_of_with_type(version, ^^Engine::Internal::BinaryFileAnnotation));
                static_assert(BinaryFileAnnotations.size() == 1, "A binary class must have exactly 1 BinaryFile annotation!");
                constexpr auto BinaryFileAnnotation = std::meta::extract<Engine::Internal::BinaryFileAnnotation>(BinaryFileAnnotations[0]);
                
                if (BinaryFileAnnotation.Version == BaseHeader->Version) {
                    return Import_Invoker<version_t>(data, Filename);
                }
            }

            // Prints the first struct because it's related.
            Engine::Error("BinaryFileImporter: Bad file version!(" + Internal::GetIdentifier<Versions...>() + " does not have a version numbered " + std::to_string(BaseHeader->Version) + ")(The structure name shown is just one of the versions)(File: " + Filename + ")");
        }

    };

}

#ifdef __INTELLISENSE__ 
#define BinaryFile(MagicNumber, Version) 
#else
#define BinaryFile(MagicNumber, Version)  [[= Engine::Internal::BinaryFileAnnotation{uint32_t(MagicNumber[0] | MagicNumber[1] << 8 | MagicNumber[2] << 16 | MagicNumber[3] << 24), uint32_t(Version)} ]]
#endif