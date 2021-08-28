#include "EntraLinkSeedSearcher.hpp"
#include "ui_EntraLinkSeedSearcher.h"
#include <Core/Enum/Encounter.hpp>
#include <Core/Enum/Method.hpp>
#include <Core/Gen5/Generators/StationaryGenerator5.hpp>
#include <Core/Gen5/Keypresses.hpp>
#include <Core/Parents/ProfileLoader.hpp>
#include <Core/RNG/SHA1.hpp>
#include <Core/Util/Utilities.hpp>
#include <Forms/Gen5/Profile/ProfileManager5.hpp>
#include <QDate>
#include <QSettings>
#include <QThread>
#include <future>
#include <mutex>

EntraLinkSeedSearcher::EntraLinkSeedSearcher(QWidget *parent) : QWidget(parent), ui(new Ui::EntraLinkSeedSearcher)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    updateProfiles();
    setupModels();
}

EntraLinkSeedSearcher::~EntraLinkSeedSearcher()
{
    QSettings setting;
    setting.beginGroup("entralink");
    setting.setValue("profile", ui->comboBoxProfiles->currentIndex());
    setting.setValue("geometry", this->saveGeometry());
    setting.endGroup();

    delete ui;
}

void EntraLinkSeedSearcher::setupModels()
{
    searcherModel = new EntraLinkSeedSearcherModel(ui->tableViewSearcher);
    searcherMenu = new QMenu(ui->tableViewSearcher);

    ui->tableViewSearcher->setModel(searcherModel);

    ui->textBoxCGearSeed->setValues(InputType::Seed32Bit);
    ui->textBoxCGearSeedFrame->setValues(InputType::Advance32Bit);
    ui->textBoxDelay->setValues(InputType::Advance32Bit);
    ui->textBoxDelayCalibration->setValues(InputType::Advance32Bit);
    ui->textBoxMaxAdvances->setValues(InputType::Advance32Bit);
    ui->textBoxMinAdvances->setValues(InputType::Advance32Bit);
    ui->textBoxMinClusterSize->setValues(InputType::Advance32Bit);

    QAction *outputTXTSearcher = searcherMenu->addAction(tr("Output Results to TXT"));
    QAction *outputCSVSearcher = searcherMenu->addAction(tr("Output Results to CSV"));
    connect(outputTXTSearcher, &QAction::triggered, [=]() { ui->tableViewSearcher->outputModel(false); });
    connect(outputCSVSearcher, &QAction::triggered, [=]() { ui->tableViewSearcher->outputModel(true); });

    connect(ui->pushButtonSearch, &QPushButton::clicked, this, &EntraLinkSeedSearcher::search);
    connect(ui->pushButtonProfileManager, &QPushButton::clicked, this, &EntraLinkSeedSearcher::profileManager);
    connect(ui->tableViewSearcher, &QTableView::customContextMenuRequested, this, &EntraLinkSeedSearcher::tableViewSearcherContextMenu);
}

void EntraLinkSeedSearcher::updateProfiles()
{
    connect(ui->comboBoxProfiles, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EntraLinkSeedSearcher::profileIndexChanged);

    profiles = ProfileLoader5::getProfiles();

    ui->comboBoxProfiles->clear();

    for (const auto &profile : profiles)
    {
        ui->comboBoxProfiles->addItem(QString::fromStdString(profile.getName()));
    }

    QSettings setting;
    int val = setting.value("entralink/profile", 0).toInt();
    if (val < ui->comboBoxProfiles->count())
    {
        ui->comboBoxProfiles->setCurrentIndex(val);
    }
}

bool EntraLinkSeedSearcher::hasProfiles() const
{
    return !profiles.empty();
}

void EntraLinkSeedSearcher::profileIndexChanged(int index)
{
    if (index >= 0)
    {
        currentProfile = profiles[index];

        ui->labelProfileTIDValue->setText(QString::number(currentProfile.getTID()));
        ui->labelProfileSIDValue->setText(QString::number(currentProfile.getSID()));
        ui->labelProfileMACAddressValue->setText(QString::number(currentProfile.getMac(), 16));
        ui->labelProfileDSTypeValue->setText(QString::fromStdString(currentProfile.getDSTypeString()));
        ui->labelProfileVCountValue->setText(QString::number(currentProfile.getVCount(), 16));
        ui->labelProfileTimer0Value->setText(QString::number(currentProfile.getTimer0Min(), 16) + "-"
                                             + QString::number(currentProfile.getTimer0Max(), 16));
        ui->labelProfileGxStatValue->setText(QString::number(currentProfile.getGxStat()));
        ui->labelProfileVFrameValue->setText(QString::number(currentProfile.getVFrame()));
        ui->labelProfileKeypressesValue->setText(QString::fromStdString(currentProfile.getKeypressesString()));
        ui->labelProfileGameValue->setText(QString::fromStdString(currentProfile.getVersionString()));
    }
}

void EntraLinkSeedSearcher::profileManager()
{
    auto *manager = new ProfileManager5();
    connect(manager, &ProfileManager5::updateProfiles, this, [=] { emit alertProfiles(5); });
    manager->show();
}

void EntraLinkSeedSearcher::tableViewSearcherContextMenu(QPoint pos)
{
    if (searcherModel->rowCount() == 0)
    {
        searcherMenu->popup(ui->tableViewSearcher->viewport()->mapToGlobal(pos));
    }
}

void EntraLinkSeedSearcher::search()
{
    u32 macPartial = currentProfile.getMac() & 0xFFFFFF;
    u32 minAdvances = ui->textBoxMinAdvances->getUInt();
    u32 maxAdvances = ui->textBoxMaxAdvances->getUInt();
    u32 clusterSize = ui->textBoxMinClusterSize->getUInt();
    u32 cGearSeed = ui->textBoxCGearSeed->getInt();
    u32 year = ui->spinBoxYear->text().toUInt();
    u32 cGearFrame = ui->textBoxCGearSeedFrame->getUInt();
    u32 ab = (cGearSeed - macPartial) >> 24;
    u32 cd = (cGearSeed - macPartial & 0x00FF0000) >> 16;
    u32 efgh = cGearSeed - macPartial & 0x0000FFFF;

    u32 delay = efgh + (2000 - year);
    u32 calibration = ui->textBoxDelayCalibration->getUInt();

    u32 calibratedDelay = delay + calibration;

    long offset = -calibratedDelay / 60;

    if (cd > 23) { }

    StateFilter filterIV(0, 0, 0, true, {}, {}, {}, {}, {});

    StationaryGenerator5 generatorIV(cGearFrame, cGearFrame, 0, 0, 0, 0, Method::Method5IVs, Encounter::Stationary, filterIV);

    std::vector<DateTime> possibleDates;

    for (u8 month = 0; month <= 12; month++)
    {
        QDate date(year, month, 1);
        for (u8 day = 1; day <= date.daysInMonth(); day++)
        {
            for (u8 minute = 0; minute <= 59; minute++)
            {
                for (u8 second = 0; second <= 59; second++)
                {
                    if (ab != ((month * day + minute + second) & 0xFF))
                    {
                        continue;
                    }

                    DateTime dt(year, month, day, cd, minute, second);
                    DateTime pdt(year, month, day, cd, minute, second);
                    pdt.addSeconds(offset);
                    if (dt.getDate().day() == pdt.getDate().day())
                    {
                        possibleDates.emplace_back(pdt);
                    }
                }
            }
        }
    }

    StationaryState5 ivFrame = generatorIV.generate(cGearSeed)[0];

    StateFilter filterPID(0, 0, 0, false, { 0, 0, 0, 0, 0, 0 }, { 31, 31, 31, 31, 31, 31 }, {}, {}, {});

    StationaryGenerator5 generatorPID(minAdvances, maxAdvances, currentProfile.getTID(), currentProfile.getSID(), 0, 255, Method::Method5,
                                      Encounter::EntraLink, filterPID);

    QSettings settings;
    int threads = settings.value("settings/threads").toInt();

    auto *thread = QThread::create(
        [=] { startSearch(generatorPID, ivFrame, threads, possibleDates, currentProfile, clusterSize, calibratedDelay); });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(ui->pushButtonCancel, &QPushButton::clicked, [=] { cancelSearch(); });
}

void EntraLinkSeedSearcher::startSearch(const StationaryGenerator5 &generator, const StationaryState5 &ivFrame, int threads,
                                        std::vector<DateTime> possibleDates, Profile5 profile, u32 clusterSize, u32 calibratedDelay)
{
    searching = true;

    int days = possibleDates.size();
    if (days < threads)
    {
        threads = days;
    }

    if (threads <= 0)
    {
        threads = 1;
    }

    std::vector<std::future<void>> threadContainer;

    auto daysSplit = days / threads;
    for (int i = 0; i < threads; i++)
    {
        std::vector<DateTime> dates;
        if (i == threads - 1)
        {
            dates.reserve(daysSplit);
            for (int j = i * daysSplit; j < (i * daysSplit) + daysSplit; j++)
            {
                dates.emplace_back(possibleDates[j]);
            }
            threadContainer.emplace_back(std::async([=] { generate(generator, ivFrame, dates, profile, clusterSize, calibratedDelay); }));
        }
        else
        {
            dates.reserve(daysSplit + (days % threads));
            for (int j = i * daysSplit; j < (i * daysSplit) + daysSplit + (days % threads); j++)
            {
                dates.emplace_back(possibleDates[j]);
            }

            threadContainer.emplace_back(std::async([=] { generate(generator, ivFrame, dates, profile, clusterSize, calibratedDelay); }));
        }
    }

    for (int i = 0; i < threads; i++)
    {
        threadContainer[i].wait();
    }
}

void EntraLinkSeedSearcher::cancelSearch()
{
    searching = false;
}

void EntraLinkSeedSearcher::generate(StationaryGenerator5 generator, const StationaryState5 &ivFrame, const std::vector<DateTime> &dates,
                                     Profile5 profile, u32 clusterSize, u32 calibratedDelay)
{
    SHA1 sha(profile);

    auto buttons = Keypresses::getKeyPresses(profile.getKeypresses(), profile.getSkipLR());
    auto values = Keypresses::getValues(buttons);
    for (u16 timer0 = profile.getTimer0Min(); timer0 <= profile.getTimer0Max(); timer0++)
    {
        sha.setTimer0(timer0, profile.getVCount());

        for (const auto &date : dates)
        {
            sha.setDate(date.getDate());
            sha.precompute();

            for (size_t i = 0; i < values.size(); i++)
            {
                sha.setButton(values[i]);

                if (!searching)
                {
                    return;
                }

                sha.setTime(date.getTime().hour(), date.getTime().minute(), date.getTime().second(), profile.getDSType());
                u64 seed = sha.hashSeed();

                auto states = generator.generate(seed);

                if (!states.empty())
                {
                    std::vector<SearcherState5<State>> displayStates;
                    displayStates.reserve(states.size());

                    for (const auto &state : states)
                    {
                        displayStates.emplace_back(date, seed, buttons[i], timer0, state);
                    }

                    std::lock_guard<std::mutex> lock(mutex);
                    results.insert(results.end(), displayStates.begin(), displayStates.end());
                }

                progress++;
            }
        }
    }
}
