#pragma once
#include <array>
#include <limits>
#include "engine/master.h"

namespace Engine {
    template <int amount, typename T = size_t>
    class Bitset {
        static inline constexpr int BitsInSlot = std::numeric_limits<T>::digits;
        static inline constexpr int SlotCount = (amount + BitsInSlot - 1) / BitsInSlot;
        static inline constexpr int RealAmount = SlotCount * BitsInSlot;

        std::array<T, SlotCount> Memory;
        size_t AmountOfBitsSet;

    public:
        Bitset() {
            for (int slot = 0; slot < SlotCount; slot++) {
                Memory[slot] = 0;
            }
        }

        void set(int Index) {
            if (Index >= RealAmount)
                Engine::Error("Attempted to index a bitset out of bounds!");
            size_t slot = Index / BitsInSlot;
            size_t id = Index % BitsInSlot;

            if (!get(Index)) {
                AmountOfBitsSet++;
            }

            Memory[slot] |= T(1) << id;
        }
        void unset(int Index) {
            if (Index >= RealAmount)
                Engine::Error("Attempted to index a bitset out of bounds!");
            size_t slot = Index / BitsInSlot;
            size_t id = Index % BitsInSlot;

            if (get(Index)) {
                AmountOfBitsSet--;
            }

            Memory[slot] &= ~(T(1) << id);
        }

        bool get(int Index) {
            if (Index >= RealAmount)
                Engine::Error("Attempted to index a bitset out of bounds!");
            int slot = Index / BitsInSlot;
            int id = Index % BitsInSlot;

            return Memory[slot] & (T(1) << id);
        }
        size_t amount_of_set() { return AmountOfBitsSet; }

        std::vector<uint32_t> get_indices_of_all_set_bits() {
            std::vector<uint32_t> Result;
            Result.reserve(AmountOfBitsSet);

            for (int slot = 0; slot < SlotCount; slot++) {
                T value = Memory[slot];
                while (value) {
                    Result.push_back(slot * BitsInSlot + std::countr_zero(value));
                    // This math unsets the lowest bit
                    value &= value - T(1);
                }
            }

            return Result;
        }
    };

}