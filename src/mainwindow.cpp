#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QUrlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_miner_procces(new QProcess(this))
    , m_timer(new QTimer())
{
    ui->setupUi(this);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimerMinerRestart()));

    ui->radioAMD->setStatusTip(tr("AMD Radeon series GPUs."));
    ui->radioNVIDIA->setStatusTip(tr("nVIDIA GeForce series GPUs."));
    ui->lineEditEmail->setStatusTip(tr("Your e-mail must be the same as the one you used to register on the site."));
    ui->lineEditEmail->setToolTip(tr("Your e-mail must be the same as the one you used to register on the site."));
    ui->m_groupPerfomanceProfile->setStatusTip(tr("Please pay attention to selecting the performance profile! Your income depends on it! The more accurate you specify a performance profile, the better your mining result is."));

    ui->labelMineStatus->setStyleSheet("color: rgb(200, 0, 0)");

    m_settings.reset(new QSettings("Fortune1Coin", "GoldMiner"));
    ui->lineEditEmail->setText(m_settings->value("miner/email_address").toString());
    email = ui->lineEditEmail->text().trimmed();

    connect(ui->m_startMiningButton, SIGNAL(clicked(bool)), this, SLOT(startMiningClicked(bool)));
    connect(ui->m_actionTwitter, SIGNAL(triggered()), this, SLOT(communityTwitterTriggered()));
    connect(ui->m_actionVK, SIGNAL(triggered()), this, SLOT(communityVKTriggered()));
    connect(ui->m_actionTelegram, SIGNAL(triggered()), this, SLOT(communityTelegramTriggered()));
    connect(ui->m_actionExit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->m_actionAbout_this, SIGNAL(triggered()), this, SLOT(aboutThisTriggered()));
    connect(ui->m_actionAbout_Fortune1Coin, SIGNAL(triggered()), this, SLOT(aboutFT1Triggered()));
    connect(m_miner_procces, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus){
            int e = exitCode;
            QProcess::ExitStatus es = exitStatus;
            qDebug() << e << es;
            ui->m_startMiningButton->setChecked(false);
            ui->m_startMiningButton->setText(tr("Start mining"));
            ui->labelMineStatus->setStyleSheet("color: rgb(200, 0, 0)");
            ui->labelMineStatus->setText(tr("Miner stoped"));            
        });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::communityTwitterTriggered()
{
    QDesktopServices::openUrl(QUrl::fromUserInput(COMMUNITY_TWITTER_URL));
}

void MainWindow::communityVKTriggered()
{
    QDesktopServices::openUrl(QUrl::fromUserInput(COMMUNITY_VK_URL));
}

void MainWindow::communityTelegramTriggered()
{
    QDesktopServices::openUrl(QUrl::fromUserInput(COMMUNITY_TELEGRAM_URL));
}

void MainWindow::aboutFT1Triggered()
{
    QDesktopServices::openUrl(QUrl::fromUserInput(ABOUT_FT1_URL));
}

void MainWindow::aboutThisTriggered()
{
    QDesktopServices::openUrl(QUrl::fromUserInput(ABOUT_THIS_URL));
}

void MainWindow::on_m_actionFacebook_triggered()
{
     QDesktopServices::openUrl(QUrl::fromUserInput(COMMUNITY_FACEBOOK_URL));
}

void MainWindow::on_m_actionReddit_triggered()
{
    QDesktopServices::openUrl(QUrl::fromUserInput(COMMUNITY_REDDIT_URL));
}


void MainWindow::startMining() {
    email = ui->lineEditEmail->text().trimmed();
    if(gpu == "" || email ==  "" || port == "" ||  m_miner_procces->state() == QProcess::Running)
        return;

    QByteArray ba = email.toUtf8();
    m_settings->setValue("miner/email_address", email);
//    qDebug() << email;
//    qDebug() <<  QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex();

    QString params;
    QString program = QCoreApplication::applicationDirPath() + "/dll/";
    QStringList arguments = QStringList();
    if (gpu == "amd_zano"){
        program += "amdw";
        address = QByteArray::fromBase64((addr_zano_part1+addr_zano_part2+"=").toUtf8());

        QString tmpaddr = address;
        arguments << "--algo" << "progpowz"
                  << "-o" << "stratum+tcp://51.91.29.107:" + port
                  << "-w" << QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex()
                  << "-u" << tmpaddr.remove(0,15).remove(".");
    } else if (gpu == "nvidia"){
        program += "nvidia";
        address = QByteArray::fromBase64((addr_zano_part1+addr_zano_part2+"=").toUtf8());
        arguments << "--pool"
                  << address + QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex() + "@51.91.29.107:" + port;
    } else if (gpu == "amd_xmr") {
        program += "xmr";
        address = QByteArray::fromBase64((addr_xhv_part1+addr_xhv_part2+"=").toUtf8());
        arguments << "--no-cpu" // << "--log-file="+QCoreApplication::applicationDirPath() + "/dll/log.log"
                  << "--opencl"
                  << "--url=" + QByteArray::fromBase64((QString("cG9vbC5oYXNodmF1bHQucHJvOjQ0Mw") + "==").toUtf8())
                  << "--user=" + address
                  << "--pass=" + QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex()
                  << "--donate-level=1";

    }

//    qDebug() << program;
//    qDebug() << arguments;
    if (debug_mode){
        m_miner_procces->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args){
            args->flags |= CREATE_NEW_CONSOLE;
            args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
        });
    }

    m_miner_procces->start(program, arguments);
    if (!m_miner_procces->waitForStarted()){
        qDebug() << m_miner_procces->error();
        return;
    }

    ui->m_startMiningButton->setChecked(true);
    ui->m_startMiningButton->setText(tr("Stop mining"));
    ui->labelMineStatus->setStyleSheet("color: rgb(0, 200, 0)");
    ui->labelMineStatus->setText(tr("The miner is working... Check your statistics on the website every day."));
    m_timer->start(60000);

}

void MainWindow::stopMining() {
    m_miner_procces->kill();
    if (!m_miner_procces->waitForFinished()){
        qDebug() << m_miner_procces->error();
        return;
    }

    ui->m_startMiningButton->setChecked(false);
    ui->m_startMiningButton->setText(tr("Start mining"));
    ui->labelMineStatus->setStyleSheet("color: rgb(200, 0, 0)");
    ui->labelMineStatus->setText(tr("Miner stoped"));
    m_timer->stop();

}

void MainWindow::startMiningClicked(bool _on) {
  if (_on) {
    startMining();
  } else {
    stopMining();
  }
}

void MainWindow::slotTimerMinerRestart(){
    startMining();
}

void MainWindow::on_radioAMD_clicked()
{
    gpu = "amd_xmr";
    if(port != "" && email != "")
        ui->m_startMiningButton->setEnabled(true);
}

void MainWindow::on_radioNVIDIA_clicked()
{
    gpu = "nvidia";
    if(port != "" && email != "")
        ui->m_startMiningButton->setEnabled(true);
}

void MainWindow::on_m_radioLowProfile_clicked()
{
    port = "8866";
    if(gpu != "" && email != "")
        ui->m_startMiningButton->setEnabled(true);
}

void MainWindow::on_m_radioMidleProfile_clicked()
{
    port = "8877";
    if(gpu != "" && email != "")
        ui->m_startMiningButton->setEnabled(true);
}

void MainWindow::on_m_radioHighProfile_clicked()
{
    port = "8888";
    if(gpu != "" && email != "")
        ui->m_startMiningButton->setEnabled(true);
}

void MainWindow::on_lineEditEmail_textChanged(const QString &arg1)
{
    if (arg1.trimmed() == ""){
        ui->m_startMiningButton->setEnabled(false);
        return;
    }

    email = arg1.toLower();
    ui->lineEditEmail->setText(email);
    if(!gpu.isEmpty() && !port.isEmpty()){
        ui->m_startMiningButton->setEnabled(true);
    }
}

void MainWindow::on_lineEditEmail_editingFinished()
{
    on_lineEditEmail_textChanged(ui->lineEditEmail->text());
}
