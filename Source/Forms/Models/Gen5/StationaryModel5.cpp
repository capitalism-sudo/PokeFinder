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

#include "StationaryModel5.hpp"
#include <Core/Enum/Method.hpp>
#include <Core/Util/Translator.hpp>
#include <Core/Util/Utilities.hpp>

StationaryGeneratorModel5::StationaryGeneratorModel5(QObject *parent, Method method) :
    TableModel<StationaryState5>(parent), method(method), time(false)
{
}

void StationaryGeneratorModel5::setMethod(Method method)
{
    this->method = method;
    emit headerDataChanged(Qt::Horizontal, 0, columnCount());
}

void StationaryGeneratorModel5::setUseTime(bool useTime)
{
    time = useTime;
    emit headerDataChanged(Qt::Horizontal, 0, columnCount());
}

int StationaryGeneratorModel5::columnCount(const QModelIndex &parent) const
{
    switch (method)
    {
    case Method::Method5IVs:
    case Method::Method5CGear:
        return 9;
    case Method::Method5:
        return time ? 8 : 7;
    default:
        return 0;
    }
}

QVariant StationaryGeneratorModel5::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const auto &state = model[index.row()];
        int column = getColumn(index.column());
        switch (column)
        {
        case 0:
            return state.getAdvances();
        case 1:
        {
            if (state.getCGearTime() == 0)
            {
                return tr("Skip");
            }
            else
            {
                u32 minutes = state.getCGearTime() / 3600;
                u32 seconds = (state.getCGearTime() - (3600 * minutes)) / 60;
                u32 milli = ((state.getCGearTime() % 60) * 100) / 60;
                return QString::number(minutes) + tr(":") + QString::number(seconds).rightJustified(2, '0') + tr(".")
                    + QString::number(milli).rightJustified(2, '0');
            }
            return QString::number(0);
        }
        case 2:
            return QString::fromStdString(Utilities::getChatot64(state.getSeed()));
        case 3:
            return QString::number(state.getPID(), 16).toUpper().rightJustified(8, '0');
        case 4:
        {
            u8 shiny = state.getShiny();
            return shiny == 2 ? tr("Square") : shiny == 1 ? tr("Star") : tr("No");
        }
        case 5:
            return QString::fromStdString(Translator::getNature(state.getNature()));
        case 6:
            return state.getAbility();
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            return state.getIV(static_cast<u8>(column - 7));
        case 13:
            return QString::fromStdString(Translator::getHiddenPower(state.getHidden()));
        case 14:
            return state.getPower();
        case 15:
            return QString::fromStdString(Translator::getGender(state.getGender()));
        }
    }
    return QVariant();
}

QVariant StationaryGeneratorModel5::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        section = getColumn(section);

        return header[section];
    }
    return QVariant();
}

int StationaryGeneratorModel5::getColumn(int column) const
{
    switch (method)
    {
    case Method::Method5IVs:
        return column > 0 ? column + 6 : column;
    case Method::Method5CGear:
        return column > 0 ? column + 6 : column;
    case Method::Method5:
        if (time)
        {
            return column > 6 ? column + 8 : column;
        }
        else
        {
            return column > 0 ? (column + 1 > 6 ? column + 9 : column + 1) : column;
        }

    default:
        return column;
    }
}

StationarySearcherModel5::StationarySearcherModel5(QObject *parent, Method method) :
    TableModel<SearcherState5<StationaryState5>>(parent), method(method)
{
}

void StationarySearcherModel5::setMethod(Method method)
{
    this->method = method;
    emit headerDataChanged(Qt::Horizontal, 0, columnCount());
}

void StationarySearcherModel5::setVersion(Game game)
{
    this->game = game;
    emit headerDataChanged(Qt::Horizontal, 0, columnCount());
}

void StationarySearcherModel5::sort(int column, Qt::SortOrder order)
{
    if (!model.empty())
    {
        emit layoutAboutToBeChanged();
        bool flag = order == Qt::AscendingOrder;
        switch (column)
        {
        case 0:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2) {
                          return flag ? state1.getInitialSeed() < state2.getInitialSeed()
                                      : state1.getInitialSeed() > state2.getInitialSeed();
                      });
            break;
        case 1:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      {
                          return flag ? state1.getState().getAdvances() < state2.getState().getAdvances()
                                      : state1.getState().getAdvances() > state2.getState().getAdvances();
                      });
            break;
        case 2:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2) {
                          return flag ? state1.getState().getLead() < state2.getState().getLead()
                                      : state1.getState().getLead() > state2.getState().getLead();
                      });
            break;
        case 3:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2) {
                          return flag ? state1.getState().getPID() < state2.getState().getPID()
                                      : state1.getState().getPID() > state2.getState().getPID();
                      });
            break;
        case 4:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      {
                          return flag ? state1.getState().getShiny() < state2.getState().getShiny()
                                      : state1.getState().getShiny() > state2.getState().getShiny();
                      });
            break;
        case 5:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      {
                          return flag ? state1.getState().getNature() < state2.getState().getNature()
                                      : state1.getState().getNature() > state2.getState().getNature();
                      });
            break;
        case 6:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      {
                          return flag ? state1.getState().getAbility() < state2.getState().getAbility()
                                      : state1.getState().getAbility() > state2.getState().getAbility();
                      });
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            std::sort(model.begin(), model.end(),
                      [flag, column](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      {
                          return flag
                              ? state1.getState().getIV(static_cast<u8>(column - 7)) < state2.getState().getIV(static_cast<u8>(column - 7))
                              : state1.getState().getIV(static_cast<u8>(column - 7)) > state2.getState().getIV(static_cast<u8>(column - 7));
                      });
            break;
        case 13:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      {
                          return flag ? state1.getState().getHidden() < state2.getState().getHidden()
                                      : state1.getState().getHidden() > state2.getState().getHidden();
                      });
            break;
        case 14:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      {
                          return flag ? state1.getState().getPower() < state2.getState().getPower()
                                      : state1.getState().getPower() > state2.getState().getPower();
                      });
            break;
        case 15:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      {
                          return flag ? state1.getState().getGender() < state2.getState().getGender()
                                      : state1.getState().getGender() > state2.getState().getGender();
                      });
            break;
        case 16:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      { return flag ? state1.getDateTime() < state2.getDateTime() : state1.getDateTime() > state2.getDateTime(); });
            break;
        case 17:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      { return flag ? state1.getTimer0() < state2.getTimer0() : state1.getTimer0() > state2.getTimer0(); });
            break;
        case 18:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<StationaryState5> &state1, const SearcherState5<StationaryState5> &state2)
                      { return flag ? state1.getButtons() < state2.getButtons() : state1.getButtons() > state2.getButtons(); });
            break;
        }
    }
}

int StationarySearcherModel5::columnCount(const QModelIndex &parent) const
{
    switch (method)
    {
    case Method::Method5IVs:
        return 13;
    case Method::Method5CGear:
        return (game == Game::Black || game == Game::White) ? 10 : 13;
    case Method::Method5:
        return 11;
    default:
        return 0;
    }
}

QVariant StationarySearcherModel5::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const auto &display = model[index.row()];
        const auto &state = display.getState();
        int column = getColumn(index.column());
        switch (column)
        {
        case 0:
            return method == Method::Method5CGear
                ? (display.getInitialSeed() >> 32 == 0 ? QString::number(display.getInitialSeed(), 16).toUpper().rightJustified(8, '0')
                                                       : QString::number(display.getInitialSeed(), 16).toUpper().rightJustified(16, '0'))
                : QString::number(display.getInitialSeed(), 16).toUpper().rightJustified(16, '0');
        case 1:
            return state.getAdvances();
        case 2:
        {
            switch (state.getLead())
            {
            case Lead::None:
                return tr("None");
            case Lead::Synchronize:
                return tr("Synchronize");
            case Lead::SuctionCups:
                return tr("Suction Cups");
            case Lead::CuteCharmFemale:
                return tr("Cute Charm (♀)");
            case Lead::CuteCharm25M:
                return tr("Cute Charm (25% ♂)");
            case Lead::CuteCharm50M:
                return tr("Cute Charm (50% ♂)");
            case Lead::CuteCharm75M:
                return tr("Cute Charm (75% ♂)");
            case Lead::CuteCharm875M:
            default:
                return tr("Cute Charm (87.5% ♂)");
            }
        }
        case 3:
            return QString::number(state.getPID(), 16).toUpper().rightJustified(8, '0');
        case 4:
        {
            u8 shiny = state.getShiny();
            return shiny == 2 ? tr("Square") : shiny == 1 ? tr("Star") : tr("No");
        }
        case 5:
            return QString::fromStdString(Translator::getNature(state.getNature()));
        case 6:
            return state.getAbility();
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            return state.getIV(static_cast<u8>(column - 7));
        case 13:
            return QString::fromStdString(Translator::getHiddenPower(state.getHidden()));
        case 14:
            return state.getPower();
        case 15:
            return QString::fromStdString(Translator::getGender(state.getGender()));
        case 16:
            return QString::fromStdString(display.getDateTime().toString());
        case 17:
            return QString::number(display.getTimer0(), 16).toUpper();
        case 18:
            return QString::fromStdString(Translator::getKeypresses(display.getButtons()));
        }
    }
    return QVariant();
}

QVariant StationarySearcherModel5::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        section = getColumn(section);

        return header[section];
    }
    return QVariant();
}

int StationarySearcherModel5::getColumn(int column) const
{
    switch (method)
    {
    case Method::Method5IVs:
    case Method::Method5CGear:
        return column > 1 ? (column + 5 > 14 ? column + 6 : column + 5) : column;
    case Method::Method5:
        return column > 6 ? column + 8 : column;
    default:
        return column;
    }
}
