/*
 * This file is part of PokéFinder
 * Copyright (C) 2017-2021 by Admiral_Fish, bumba, and EzPzStreamz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef GENERATOR5_HPP
#define GENERATOR5_HPP

#include <Core/Gen5/States/StationaryState5.hpp>
#include <Core/Parents/Generators/StationaryGenerator.hpp>

enum Encounter : u8;

class StationaryGenerator5 : public StationaryGenerator
{
public:
    StationaryGenerator5() = default;
    StationaryGenerator5(u32 initialAdvances, u32 maxAdvances, u16 tid, u16 sid, u8 gender, u8 genderRatio, Method method,
                         Encounter encounter, const StateFilter &filter);
    std::vector<StationaryState5> generate(u64 seed) const;

private:
    u8 idBit;
    Encounter encounter;
    u8 gender;

    std::vector<StationaryState5> generateRoamerIVs(u64 seed) const;
    std::vector<StationaryState5> generateIVs(u64 seed) const;
    std::vector<StationaryState5> generateRoamerCGear(u64 seed) const;
    std::vector<StationaryState5> generateCGear(u64 seed) const;
    std::vector<StationaryState5> generateStationary(u64 seed) const;
    std::vector<StationaryState5> generateRoamer(u64 seed) const;
    std::vector<StationaryState5> generateGift(u64 seed) const;
    std::vector<StationaryState5> generateEntraLink(u64 seed) const;
    std::vector<StationaryState5> generateGiftEgg(u64 seed) const;
    std::vector<StationaryState5> generateHiddenGrotto(u64 seed) const;
};

#endif // GENERATOR5_HPP
