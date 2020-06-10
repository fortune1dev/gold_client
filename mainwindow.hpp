#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QProcess>
#include <QSettings>
#include "Windows.h"

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

private:
    const QString COMMUNITY_VK_URL = "https://vk.com/gold_mining";
    const QString COMMUNITY_TELEGRAM_URL = "https://t.me/ft1_goldmining";
    const QString COMMUNITY_TWITTER_URL = "https://twitter.com/Fortune1Coin";
    const QString COMMUNITY_REDDIT_URL = "https://www.reddit.com/user/Fortune1Coin";
    const QString ABOUT_THIS_URL = "https://gold.fortune1.money/";
    const QString ABOUT_FT1_URL = "https://fortune1.money/";


    Ui::MainWindow *ui;
    QProcess* m_miner_procces;
    QScopedPointer<QSettings> m_settings;

    void startMining();
    void stopMining();

    Q_SLOT void startMiningClicked(bool _on);
    Q_SLOT void communityTwitterTriggered();
    Q_SLOT void communityVKTriggered();
    Q_SLOT void communityTelegramTriggered();
    Q_SLOT void aboutThisTriggered();
    Q_SLOT void aboutFT1Triggered();
};
#endif // MAINWINDOW_HPP
