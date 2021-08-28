#ifndef ENTRALINKSEEDSEARCHER_HPP
#define ENTRALINKSEEDSEARCHER_HPP

#include <Core/Gen5/Generators/StationaryGenerator5.hpp>
#include <Core/Gen5/Profile5.hpp>
#include <Core/Util/Global.hpp>
#include <Forms/Models/Gen5/EntraLinkSeedSearcherModel.hpp>
#include <QMenu>
#include <QWidget>
#include <mutex>

namespace Ui
{
    class EntraLinkSeedSearcher;
}

class EntraLinkSeedSearcher : public QWidget
{
    Q_OBJECT
signals:
    void alertProfiles(int);

public:
    explicit EntraLinkSeedSearcher(QWidget *parent = nullptr);
    ~EntraLinkSeedSearcher() override;
    bool hasProfiles() const;

private:
    Ui::EntraLinkSeedSearcher *ui;
    EntraLinkSeedSearcherModel *searcherModel = nullptr;
    std::vector<Profile5> profiles;
    Profile5 currentProfile;
    QMenu *searcherMenu = nullptr;
    bool searching = false;
    std::atomic<int> progress;
    std::vector<SearcherState5<State>> results;
    std::mutex mutex;

    void updateProfiles();
    void setupModels();
    void startSearch(const StationaryGenerator5 &generator, const StationaryState5 &ivFrame, int threads,
                     std::vector<DateTime> possibleDates, Profile5 profile, u32 clusterSize, u32 calibratedDelay);
    void generate(StationaryGenerator5 generator, const StationaryState5 &ivFrame, const std::vector<DateTime> &dates, Profile5 profile,
                  u32 clusterSize, u32 calibratedDelay);

private slots:
    void search();
    void cancelSearch();
    void profileIndexChanged(int index);
    void profileManager();
    void tableViewSearcherContextMenu(QPoint pos);
};

#endif // ENTRALINKSEEDSEARCHER_HPP
