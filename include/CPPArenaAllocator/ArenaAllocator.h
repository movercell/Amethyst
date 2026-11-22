// CPPArenaAllocator by The Mover Cell.

#pragma once

#include <string>
#include <cassert>
#include <type_traits>
#include <functional>
#include <iostream>
#include <span>

namespace mem {

    constexpr size_t ARENAALLOCATOR_MAXIMUM_ALIGNMENT = 32;

    class ArenaAllocator {

        class ObjectDestructor {
            void* Object = nullptr;
            ObjectDestructor* Next = nullptr;
            size_t Count = 0;
            void (*Destruction)(void* object, size_t count) = nullptr;

        public:
            ObjectDestructor(void* object, ObjectDestructor* next, size_t count, void (*destruction)(void* object, size_t count)) : Object(object), Next(next), Count(count), Destruction(destruction) {};
            void Destruct() {
                Destruction(Object, Count);

                if (Next) {
                    Next->Destruct();
                }
            }
        };

        template<bool Align = true>
        void* RawAlloc(size_t Size, size_t Alignment) {
            if constexpr (Align)
                currentPtr = (currentPtr + (Alignment - 1)) & ~(Alignment - 1);
            
            size_t result = currentPtr;
            currentPtr += Size;

            if (currentPtr >= size) {
                std::cerr << "An arena allocator with the tag of \"" + tag + "\" has ran out of memory!" << std::endl;
                assert(false);
            }

            return data + result;
        }

        template<typename T>
        consteval static void align_assert() {
            static_assert(alignof(T) <= ARENAALLOCATOR_MAXIMUM_ALIGNMENT, "Exceeded the maximum alignment of ArenaAllocator!(Go change ARENAALLOCATOR_MAXIMUM_ALIGNMENT if you really need this.)");
        }


        size_t size;
        std::string tag;
        std::byte* data;
        size_t currentPtr = 0;
        ObjectDestructor* lastDestructor = nullptr;
        
    public:
        ArenaAllocator(size_t Size, std::string Tag = "This arena was not given a tag.") : tag(Tag), size(Size), data(new (std::align_val_t(ARENAALLOCATOR_MAXIMUM_ALIGNMENT)) std::byte[Size]) {}
        ArenaAllocator(ArenaAllocator& other) = delete;
        ~ArenaAllocator() { Clear(); delete data; }

        template<typename T, typename... Arguments>
        T* New(Arguments... Args) {
            align_assert<T>();

            T* obj = std::construct_at(reinterpret_cast<T*>(RawAlloc(sizeof(T), alignof(T))), std::forward<Arguments>(Args)...);

            // Handle destruction in case of the object not being trivially destructible.
            if constexpr (!std::is_trivially_destructible_v<T>) {
                lastDestructor = std::construct_at(reinterpret_cast<ObjectDestructor*>(RawAlloc(sizeof(ObjectDestructor), alignof(ObjectDestructor))),
                                        reinterpret_cast<void*>(obj),
                                        lastDestructor,
                                        -1, // Not applicable.
                                        [](void* object, size_t count) { reinterpret_cast<T*>(object)->~T(); });
            }

            return obj;
        }
        template<typename T, typename... Arguments>
        T* NewArray(unsigned int Count, Arguments... Args) {
            align_assert<T>();
            if (Count == 0) return nullptr;

            // Allocate the memory + do the first object manually because it's more convenient this way.
            T* result = std::construct_at(reinterpret_cast<T*>(RawAlloc(sizeof(T) * Count, alignof(T))), std::forward<Arguments>(Args)...);

            // Construct all the remaining objects.
            for (unsigned int i = 1; i < Count; i++) {
                std::construct_at(&result[i], std::forward<Arguments>(Args)...);
            }

            // Handle destruction in case of the object not being trivially destructible.
            if constexpr (!std::is_trivially_destructible_v<T>) {
                lastDestructor = std::construct_at(reinterpret_cast<ObjectDestructor*>(RawAlloc(sizeof(ObjectDestructor), alignof(ObjectDestructor))),
                                        reinterpret_cast<void*>(result),
                                        lastDestructor,
                                        Count,
                                        [](void* object, size_t count) { for (size_t i = 0; i < count; i++) reinterpret_cast<T*>(object)[i].~T(); });
            }

            return result;
        }
        void Clear() {
            static_assert(std::is_trivially_destructible_v<ObjectDestructor>);

            if (lastDestructor)
                lastDestructor->Destruct();
    
            currentPtr = 0;
        }

        std::span<std::byte> GetBuffer() {
            return {data, currentPtr};
        }

    };

}
