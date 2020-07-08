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
{
    ui->setupUi(this);
    //this->setWindowTitle(tr("Gold miner of Fortune1Coin"));
    ui->labelMineStatus->setStyleSheet("color: rgb(200, 0, 0)");

    m_settings.reset(new QSettings(tr("Fortune1Coin"), tr("Gold miner")));
    ui->lineEditEmail->setText(m_settings->value("miner/email_address").toString());

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

void MainWindow::startMining() {
    QString email = ui->lineEditEmail->text();
    if(gpu == "" || email ==  "")
        return;

    QByteArray ba = email.toUtf8();
    m_settings->setValue("miner/email_address", email);
//    qDebug() << email;
//    qDebug() <<  QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex();

    QString params;
    QString program = QCoreApplication::applicationDirPath() + "/" + gpu;
    QStringList arguments = QStringList()
            << "--pool"
            << "stratum1+tcp://ZxDkkTsVsX3MvBYGMdqT3fJimqa2cexc83VKz644HcvVbQfdBRkgr69BzVhTzRwdxQe7DRm8DESbibewKYWrhvCX2wNxo8TAE."
               + QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex()
               + "@zano.luckypool.io:8877";

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
    if (ui->m_startMiningButton->isChecked()) {
        ui->m_startMiningButton->setChecked(true);
        ui->m_startMiningButton->setText(tr("Stop mining"));
        ui->labelMineStatus->setStyleSheet("color: rgb(0, 200, 0)");
        ui->labelMineStatus->setText(tr("The miner is working... Check your statistics every day."));
       // ui->m_startMiningButton->setFixedWidth(190);
    }
}

void MainWindow::stopMining() {
    m_miner_procces->kill();
    if (!ui->m_startMiningButton->isChecked()) {
        ui->m_startMiningButton->setChecked(false);
        ui->m_startMiningButton->setText(tr("Start mining"));
        ui->labelMineStatus->setStyleSheet("color: rgb(200, 0, 0)");
        ui->labelMineStatus->setText(tr("Miner stoped"));
        //ui->m_startMiningButton->setFixedWidth(190);
    }
}

void MainWindow::startMiningClicked(bool _on) {
  if (_on) {
    startMining();
  } else {
    stopMining();
  }
}

void MainWindow::on_radioAMD_clicked()
{
    gpu = "amd";
    ui->m_startMiningButton->setEnabled(true);
}

void MainWindow::on_radioNVIDIA_clicked()
{
    gpu = "nvidia";
    ui->m_startMiningButton->setEnabled(true);
}
