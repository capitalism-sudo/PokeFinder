#ifndef ENTRALINKSEEDSEARCHERMODEL_HPP
#define ENTRALINKSEEDSEARCHERMODEL_HPP

#include <Core/Gen5/States/SearcherState5.hpp>
#include <Core/Parents/States/State.hpp>
#include <Forms/Models/TableModel.hpp>

class EntraLinkSeedSearcherModel : public TableModel<SearcherState5<State>>
{
    Q_OBJECT
public:
    explicit EntraLinkSeedSearcherModel(QObject *parent);
    void sort(int column, Qt::SortOrder order) override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QStringList header
        = { tr("Seed"), tr("Advances"), tr("Timer0"), tr("Date/Time"), tr("C-Gear Date/Time"), tr("Nature"), tr("HP"), tr("Atk"),
            tr("Def"),  tr("SpA"),      tr("SpD"),    tr("Spe"),       tr("Buttons") };
};

#endif // ENTRALINKSEEDSEARCHERMODEL_HPP
