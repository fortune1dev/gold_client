#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QProcess>
#include <QSettings>
#include <QByteArray>
#include <QTimer>
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
    bool debug_mode = false;

private slots:
    void on_radioAMD_clicked();
    void on_radioNVIDIA_clicked();
    void startMiningClicked(bool _on);
    void communityTwitterTriggered();
    void communityVKTriggered();
    void communityTelegramTriggered();
    void aboutThisTriggered();
    void aboutFT1Triggered();
    void on_m_actionFacebook_triggered();
    void on_m_actionReddit_triggered();

    void on_m_radioLowProfile_clicked();
    void on_m_radioMidleProfile_clicked();
    void on_m_radioHighProfile_clicked();

    void slotTimerMinerRestart();

    void on_lineEditEmail_textChanged(const QString &arg1);    

    void on_lineEditEmail_editingFinished();

private:
    const QString COMMUNITY_VK_URL          = "https://vk.com/gold_mining";
    const QString COMMUNITY_TELEGRAM_URL    = "https://t.me/ft1_goldmining";
    const QString COMMUNITY_TWITTER_URL     = "https://twitter.com/Fortune1Coin";
    const QString COMMUNITY_FACEBOOK_URL    = "https://facebook.com/Fortune1Coins";
    const QString COMMUNITY_REDDIT_URL      = "https://www.reddit.com/user/Fortune1Coin";
    const QString ABOUT_THIS_URL            = "https://gold.fortune1.money/";
    const QString ABOUT_FT1_URL             = "https://fortune1.money/";

    QString gpu         = "";
    QString port        = "";
    QString email       = "";
    QString addr_zano_part1  = "c3RyYXR1bTErdGNwOi8vWnhEa2tUc1ZzWDNNdkJZR01kcVQzZkppbXFhMmNleGM4M1ZLej";
    QString addr_xhv_part1  = "aHZ4eHhqNWFjTHFHbXVWamtueXVlclE4UFFFbVlNV1paN2NN";
    QString addr_zano_part2  = "Y0NEhjdlZiUWZkQlJrZ3I2OUJ6VmhUelJ3ZHhRZTdEUm04REVTYmliZXdLWVdyaHZDWDJ3TnhvOFRBRS4";
    QString addr_xhv_part2  = "RWg3Q3FFVHNETXBRMlN4R0h6VWJlaVoyQ2o4NERWV1h3YkJ4U2RKd1dBemRUZXdtMU1YRjFpU29DdnkzdWs";
    QString address;

    Ui::MainWindow *ui;
    QProcess* m_miner_procces;
    QScopedPointer<QSettings> m_settings;
    QTimer *m_timer;

    void startMining();
    void stopMining();
};
#endif // MAINWINDOW_HPP
