#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QProcess>
#include <QSettings>
#include <QByteArray>
#include <QTimer>
#include <QThread>
#include <QDesktopServices>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "Windows.h"
#include "socketclient.hpp"

class QPushButton;
class QAbstractButton;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool debug_mode = false;

private slots:
    void on_radioAMD_clicked();
    void on_radioNVIDIA_clicked();
    void communityTwitterTriggered()
    {
        QDesktopServices::openUrl(QUrl::fromUserInput(COMMUNITY_TWITTER_URL));
    }
    void communityVKTriggered()
    {
        QDesktopServices::openUrl(QUrl::fromUserInput(COMMUNITY_VK_URL));
    }
    void communityTelegramTriggered()
    {
        QDesktopServices::openUrl(QUrl::fromUserInput(COMMUNITY_TELEGRAM_URL));
    }
    void aboutThisTriggered()
    {
        QDesktopServices::openUrl(QUrl::fromUserInput(ABOUT_THIS_URL));
    }
    void aboutFT1Triggered()
    {
        QDesktopServices::openUrl(QUrl::fromUserInput(ABOUT_FT1_URL));
    }
    void on_m_actionFacebook_triggered()
    {
        QDesktopServices::openUrl(QUrl::fromUserInput(COMMUNITY_FACEBOOK_URL));
    }
    void on_m_actionReddit_triggered()
    {
        QDesktopServices::openUrl(QUrl::fromUserInput(COMMUNITY_REDDIT_URL));
    }

    void slotTimerMinerRestart();
    void slotTimerGPUMinerStats();
    void slotTimerCPUMinerStats();
    void slot_HTTP_API_GPU_Respose(QNetworkReply *reply);

    void on_lineEditEmail_textChanged(const QString &arg1);
    void on_lineEditEmail_editingFinished();
    void on_m_check_GPU_mining_stateChanged(int state);
    void on_m_check_CPU_mining_stateChanged(int state);
    void on_m_CPUCoresCombo_activated(int index);
    void on_m_startCPUMiningButton_clicked(bool checked);
    void on_m_startGPUMiningButton_clicked(bool checked);

private:
    const QString COMMUNITY_VK_URL          = "https://vk.com/gold_mining";
    const QString COMMUNITY_TELEGRAM_URL    = "https://t.me/ft1_goldmining";
    const QString COMMUNITY_TWITTER_URL     = "https://twitter.com/Fortune1Coin";
    const QString COMMUNITY_FACEBOOK_URL    = "https://facebook.com/Fortune1Coins";
    const QString COMMUNITY_REDDIT_URL      = "https://www.reddit.com/user/Fortune1Coin";
    const QString ABOUT_THIS_URL            = "https://gold.fortune1.money/";
    const QString ABOUT_FT1_URL             = "https://fortune1.money/";

    QString gpu         = "";
    QString email       = "";
    QString addr_zano_part1  = "WnhEa2tUc1ZzWDNNdkJZR01kcVQzZkppbXFhMmNleGM4M1ZLejY0NEhjdlZiUWZkQlJ";
    QString addr_xhv_part1  = "aHZ4eHhqNWFjTHFHbXVWamtueXVlclE4UFFFbVlNV1paN2NN";
    QString addr_vrsc_part2 = "2VCekJzYllub2tuUnh3SA";
    QString addr_vrsc_part1 = "UlhrbUZ5UUZ3VXVoNXNEa1JmU";
    QString addr_zano_part2  = "rZ3I2OUJ6VmhUelJ3ZHhRZTdEUm04REVTYmliZXdLWVdyaHZDWDJ3TnhvOFRBRQ";
    QString addr_xhv_part2  = "RWg3Q3FFVHNETXBRMlN4R0h6VWJlaVoyQ2o4NERWV1h3YkJ4U2RKd1dBemRUZXdtMU1YRjFpU29DdnkzdWs";
    QString address;

    Ui::MainWindow *ui;
    QScopedPointer<QSettings> m_settings;
    QProcess* m_gpu_miner_procces = new QProcess(this);
    QProcess* m_cpu_miner_procces = new QProcess(this);
    QTimer *m_timer = new QTimer(this);
    QTimer *m_gpu_miners_stats_timer = new QTimer(this);
    QTimer *m_cpu_miners_stats_timer = new QTimer(this);

    QNetworkAccessManager *m_http_api_gpu_miner_manager = new QNetworkAccessManager(this);
    SocketClient *m_CPUSocket = new SocketClient(this);
    //SocketClient *m_GPUSocket = new SocketClient(this);


    quint16 m_nNextBlockSize = 0;
    quint16 m_cpuCoresCount = 2;
    bool m_GPU_Mining_Avalible = false;
    bool m_CPU_Mining_Avalible = false;

    void startGPUMining();
    void stopGPUMining();
    void startCPUMining();
    void stopCPUMining();
    void initCpuCoresCombo();
    void onAfterStartMiner(const QString type);
    void onAfterStopMiner(const QString type);
};
#endif // MAINWINDOW_HPP
