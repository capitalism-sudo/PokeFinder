#include "EntraLinkSeedSearcherModel.hpp"

#include <Core/Util/Translator.hpp>
#include <Core/Util/Utilities.hpp>

EntraLinkSeedSearcherModel::EntraLinkSeedSearcherModel(QObject *parent) : TableModel<SearcherState5<State>>(parent)
{
}

void EntraLinkSeedSearcherModel::sort(int column, Qt::SortOrder order)
{
    if (!model.empty())
    {
        emit layoutAboutToBeChanged();
        bool flag = order == Qt::AscendingOrder;
        switch (column)
        {
        case 0:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<State> &state1, const SearcherState5<State> &state2) {
                          return flag ? state1.getInitialSeed() < state2.getInitialSeed()
                                      : state1.getInitialSeed() > state2.getInitialSeed();
                      });
            break;
        case 1:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<State> &state1, const SearcherState5<State> &state2)
                      {
                          return flag ? state1.getState().getAdvances() < state2.getState().getAdvances()
                                      : state1.getState().getAdvances() > state2.getState().getAdvances();
                      });
            break;
        case 2:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<State> &state1, const SearcherState5<State> &state2)
                      { return flag ? state1.getTimer0() < state2.getTimer0() : state1.getTimer0() > state2.getTimer0(); });
            break;
        case 3:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<State> &state1, const SearcherState5<State> &state2)
                      { return flag ? state1.getDateTime() < state2.getDateTime() : state1.getDateTime() > state2.getDateTime(); });
            break;
        case 4:
            break;
        case 5:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<State> &state1, const SearcherState5<State> &state2)
                      {
                          return flag ? state1.getState().getNature() < state2.getState().getNature()
                                      : state1.getState().getNature() > state2.getState().getNature();
                      });
            break;
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            std::sort(model.begin(), model.end(),
                      [flag, column](const SearcherState5<State> &state1, const SearcherState5<State> &state2)
                      {
                          return flag
                              ? state1.getState().getIV(static_cast<u8>(column - 6)) < state2.getState().getIV(static_cast<u8>(column - 6))
                              : state1.getState().getIV(static_cast<u8>(column - 6)) > state2.getState().getIV(static_cast<u8>(column - 6));
                      });
            break;
        case 12:
            std::sort(model.begin(), model.end(),
                      [flag](const SearcherState5<State> &state1, const SearcherState5<State> &state2)
                      { return flag ? state1.getButtons() < state2.getButtons() : state1.getButtons() > state2.getButtons(); });
            break;
        }
    }
}

int EntraLinkSeedSearcherModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return 13;
}

QVariant EntraLinkSeedSearcherModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const auto &display = model[index.row()];
        const auto &state = display.getState();
        int column = index.column();
        switch (column)
        {
        case 0:
            return QString::number(display.getInitialSeed(), 16).toUpper().rightJustified(16, '0');
        case 1:
            return state.getAdvances();
        case 2:
            return QString::number(display.getTimer0(), 16).toUpper();
        case 3:
            return QString::fromStdString(display.getDateTime().toString());
        case 4:
            return QVariant();
        case 5:
            return QString::fromStdString(Translator::getNature(state.getNature()));
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            return state.getIV(static_cast<u8>(column - 6));
        case 12:
            return QString::fromStdString(Translator::getKeypresses(display.getButtons()));
        }
    }
    return QVariant();
}

QVariant EntraLinkSeedSearcherModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return header[section];
    }
    return QVariant();
}
