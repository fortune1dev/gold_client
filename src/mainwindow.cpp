#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->radioAMD->setStatusTip(tr("AMD Radeon series GPUs."));
    ui->radioNVIDIA->setStatusTip(tr("nVIDIA GeForce series GPUs."));
    ui->lineEditEmail->setStatusTip(tr("Your e-mail must be the same as the one you used to register on the site."));
    ui->lineEditEmail->setToolTip(tr("Your e-mail must be the same as the one you used to register on the site."));


    ui->m_labelGPUMinerStatus->setStyleSheet("color: rgb(200, 0, 0)");
    ui->m_labelCPUMinerStatus->setStyleSheet("color: rgb(200, 0, 0)");

    m_settings.reset(new QSettings("Fortune1Coin", "GoldMiner"));
    ui->lineEditEmail->setText(m_settings->value("miner/email_address").toString());
    email = ui->lineEditEmail->text().trimmed();

    connect(m_timer, &QTimer::timeout, this, &MainWindow::slotTimerMinerRestart);
    connect(m_gpu_miners_stats_timer, &QTimer::timeout, this, &MainWindow::slotTimerGPUMinerStats);
    connect(m_cpu_miners_stats_timer, &QTimer::timeout, this, &MainWindow::slotTimerCPUMinerStats);
    connect(m_http_api_gpu_miner_manager, &QNetworkAccessManager::finished, this, &MainWindow::slot_HTTP_API_GPU_Respose);
    connect(ui->m_actionTwitter, SIGNAL(triggered()), this, SLOT(communityTwitterTriggered()));
    connect(ui->m_actionVK, SIGNAL(triggered()), this, SLOT(communityVKTriggered()));
    connect(ui->m_actionTelegram, SIGNAL(triggered()), this, SLOT(communityTelegramTriggered()));
    connect(ui->m_actionExit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->m_actionAbout_this, SIGNAL(triggered()), this, SLOT(aboutThisTriggered()));
    connect(ui->m_actionAbout_Fortune1Coin, SIGNAL(triggered()), this, SLOT(aboutFT1Triggered()));
    connect(m_gpu_miner_procces, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus){
            int e = exitCode;
            QProcess::ExitStatus es = exitStatus;
            qDebug() << e << es;
            onAfterStopMiner("gpu");
        });
    connect(m_cpu_miner_procces, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus){
            int e = exitCode;
            QProcess::ExitStatus es = exitStatus;
            qDebug() << e << es;
            onAfterStopMiner("cpu");
        });

    gpu = m_settings->value("miner/gpu_chip").toString();
    if (gpu != ""){
        ui->m_frame_GPU_settings->setEnabled(true);
        ui->m_check_GPU_mining->setChecked(true);
        if (gpu == "nvidia"){
            ui->radioNVIDIA->setChecked(true);
            ui->m_startGPUMiningButton->setEnabled(true);
        } else{
            ui->radioAMD->setChecked(true);
            ui->m_startGPUMiningButton->setEnabled(true);
        }
    } else {
        ui->m_frame_GPU_settings->setEnabled(false);
    }

    if (m_settings->value("miner/cpu_active").toBool() == false)
        ui->m_frame_CPU_settings->setEnabled(false);
    else {
        ui->m_check_CPU_mining->setChecked(true);
    }
    initCpuCoresCombo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startGPUMining() {
    bool gpu_miner_started = false;
    email = ui->lineEditEmail->text().trimmed();
    if(gpu == "" || email ==  "" || m_gpu_miner_procces->state() == QProcess::Running)
        return;

    QByteArray ba = email.toUtf8();
    m_settings->setValue("miner/email_address", email);
    m_settings->setValue("miner/gpu_chip", gpu);

    QString program = QCoreApplication::applicationDirPath() + "/miners/";
    QStringList arguments = QStringList();
    if (gpu == "nvidia" /*|| gpu == "amd_xmr"*/){
        program += "t-rex/t-rex.exe";
        address = QByteArray::fromBase64((addr_zano_part1+addr_zano_part2+"==").toUtf8());
        arguments << "-a" << "progpowz"
                  << "--coin" << "zano"
                  << "-o" << QByteArray::fromBase64((QString("c3RyYXR1bSt0Y3A6Ly96YW5vLmx1Y2t5cG9vbC5pbzo4ODc3")).toUtf8())
                  << "-w" << QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex()
                  << "-u" << address
                  << "--fan" << "t:67"
                  << "--pl" << "85"
                  << "--api-bind-http" << "127.0.0.1:54972"
                  << "--api-read-only"
                  << "-b" << "0";
    } else if (gpu == "amd_xmr") {
        /*program += "xmrig/xmrig.exe";
        address = QByteArray::fromBase64((addr_xhv_part1+addr_xhv_part2+"=").toUtf8());
        arguments << "--opencl"
                  << "--url=" + QByteArray::fromBase64((QString("MzcuOS4zLjI2OjQ0Mw") + "==").toUtf8())
                  << "--user=" + address
                  << "--pass=" + QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex()
                  << "--donate-level=1"
                  << "--http-host=127.0.0.1"
                  << "--http-port=54972";*/

        program += "wildrig/wildrig.exe";
        address = QByteArray::fromBase64((addr_zano_part1+addr_zano_part2+"==").toUtf8());
        arguments << "-a" << "progpowz"
                  << "-o" << QByteArray::fromBase64((QString("c3RyYXR1bSt0Y3A6Ly96YW5vLmx1Y2t5cG9vbC5pbzo4ODc3")).toUtf8())
                  << "-w" << QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex()
                  << "-u" << address
                  << "--api-port" << "54972"
                  << "--print-full"
                  << "--print-power"
                  << "--pass" << "xx";
    }

    qDebug() << program;
    qDebug() << arguments;

    if (debug_mode){
        m_gpu_miner_procces->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args){
            args->flags |= CREATE_NEW_CONSOLE;
            args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
        });
    }

    if (gpu == "nvidia")
        m_gpu_miner_procces->setWorkingDirectory(QCoreApplication::applicationDirPath() + "/miners/t-rex/");
    if (gpu == "amd_xmr")
        m_gpu_miner_procces->setWorkingDirectory(QCoreApplication::applicationDirPath() + "/miners/wildrig/");

    m_gpu_miner_procces->start(program, arguments);
    if (m_gpu_miner_procces->waitForStarted()){
        gpu_miner_started = true;
    }

    if(gpu_miner_started){
       onAfterStartMiner("gpu");
    }
}

void MainWindow::stopGPUMining() {
    m_gpu_miner_procces->kill();
    if (!m_gpu_miner_procces->waitForFinished()){
        qDebug() << m_gpu_miner_procces->error();
        return;
    }

    onAfterStopMiner("gpu");

}

void MainWindow::startCPUMining() {
    bool cpu_miner_started = false;
    email = ui->lineEditEmail->text().trimmed();
    if(email ==  "" ||  m_cpu_miner_procces->state() == QProcess::Running)
        return;

    QByteArray ba = email.toUtf8();
    m_settings->setValue("miner/email_address", email);
    m_settings->setValue("miner/cpu_active", m_CPU_Mining_Avalible);

    QString program = QCoreApplication::applicationDirPath() + "/miners/nheqminer/nheqminer.exe";
    QStringList arguments = QStringList();

    address = QByteArray::fromBase64((addr_vrsc_part1+addr_vrsc_part2+"==").toUtf8());

    arguments << "-v"
              << "-l" << QByteArray::fromBase64((QString("NzkuMTM3LjcwLjQ4OjM5NTY") + "=").toUtf8())
              << "-u" << address + "." + QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex()
              << "-p" << "x"
              << "-t" << QString::number(m_cpuCoresCount).toUtf8()
              << "-a" << "54973";

    //qDebug() << program;
    //qDebug() << arguments;

    if (debug_mode){
        m_cpu_miner_procces->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args){
            args->flags |= CREATE_NEW_CONSOLE;
            args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
        });
    }

    m_cpu_miner_procces->start(program, arguments);
    if (m_cpu_miner_procces->waitForStarted()){
        cpu_miner_started = true;
    }

    if(cpu_miner_started){
        onAfterStartMiner("cpu");
    }
}

void MainWindow::stopCPUMining() {
    m_cpu_miner_procces->kill();
    if (!m_cpu_miner_procces->waitForFinished()){
        qDebug() << m_cpu_miner_procces->error();
        return;
    }

    onAfterStopMiner("cpu");
}

void MainWindow::initCpuCoresCombo()
{
    int maxCpuCoreCount = QThread::idealThreadCount();
    if (maxCpuCoreCount == -1) {
      maxCpuCoreCount = 2;
    }

    for (int i = 0; i < maxCpuCoreCount; ++i) {
        ui->m_CPUCoresCombo->addItem(QString::number(i + 1), i + 1);
    }

    ui->m_CPUCoresCombo->setCurrentIndex(m_cpuCoresCount - 1);
}

void MainWindow::onAfterStartMiner(const QString type)
{
    if(type == "cpu") {
        ui->m_startCPUMiningButton->setChecked(true);
        ui->m_startCPUMiningButton->setText(tr("Stop mining"));
        ui->m_labelCPUMinerStatus->setStyleSheet("color: rgb(0, 200, 0)");
        ui->m_labelCPUMinerStatus->setText(tr("The miner is working... Check your statistics on the website every day."));
        ui->m_check_CPU_mining->setEnabled(false);
        m_cpu_miners_stats_timer->start(10000);
        m_CPUSocket->Connect("127.0.0.1", 54973, ui->m_labelCPUMinerStatistics);
    } else {
        ui->m_startGPUMiningButton->setChecked(true);
        ui->m_startGPUMiningButton->setText(tr("Stop mining"));
        ui->m_labelGPUMinerStatus->setStyleSheet("color: rgb(0, 200, 0)");
        ui->m_labelGPUMinerStatus->setText(tr("The miner is working... Check your statistics on the website every day."));
        ui->m_check_GPU_mining->setEnabled(false);
        ui->radioNVIDIA->setEnabled(false);
        ui->radioAMD->setEnabled(false);
        m_timer->start(60000);
        m_gpu_miners_stats_timer->start(5000);
//        if (gpu == "nvidia"){
//            m_GPUSocket->Connect("127.0.0.1", 54972, ui->m_labelGPUMinerStatistics);
//        }
    }
}

void MainWindow::onAfterStopMiner(const QString type)
{
    if(type == "cpu") {
        ui->m_startCPUMiningButton->setChecked(false);
        ui->m_startCPUMiningButton->setText(tr("Start mining"));
        ui->m_labelCPUMinerStatus->setStyleSheet("color: rgb(200, 0, 0)");
        ui->m_labelCPUMinerStatus->setText(tr("Miner stoped"));
        ui->m_check_CPU_mining->setEnabled(true);
        m_cpu_miners_stats_timer->stop();
        m_CPUSocket->Disconnect();
    } else {
        ui->m_startGPUMiningButton->setChecked(false);
        ui->m_startGPUMiningButton->setText(tr("Start mining"));
        ui->m_labelGPUMinerStatus->setStyleSheet("color: rgb(200, 0, 0)");
        ui->m_labelGPUMinerStatus->setText(tr("Miner stoped"));
        ui->m_check_GPU_mining->setEnabled(true);
        ui->radioNVIDIA->setEnabled(true);
        ui->radioAMD->setEnabled(true);
        m_timer->stop();
        m_gpu_miners_stats_timer->stop();
    }
}

void MainWindow::on_m_startGPUMiningButton_clicked(bool _on)
{
    if (_on) {
      startGPUMining();
    } else {
      stopGPUMining();
    }
}


void MainWindow::on_m_startCPUMiningButton_clicked(bool _on)
{
    if (_on) {
      startCPUMining();
    } else {
      stopCPUMining();
    }
}

void MainWindow::slotTimerMinerRestart(){
    startGPUMining();
}

void MainWindow::slotTimerGPUMinerStats()
{
    m_http_api_gpu_miner_manager->get(QNetworkRequest(QUrl("http://127.0.0.1:54972/summary")));
}

void MainWindow::slotTimerCPUMinerStats()
{
    m_CPUSocket->SendData("status\r\n");

    auto answer = m_CPUSocket->GetData();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(answer);
    QJsonObject jsonObj = jsonDoc.object();
    double curHR = 0;

    auto jsonArr = jsonObj["result"].toObject();
    curHR = jsonArr["speed_ips"].toDouble() * 1000;

    QString text = tr("Hashrate for last 15 sec: ") + QString::number(curHR) + " KH/s <br />";
    ui->m_labelCPUMinerStatistics->setText(text);
    //qDebug() << answer;

}

void MainWindow::slot_HTTP_API_GPU_Respose(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError) {
        if (gpu == "nvidia"){
            QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObj = jsonDoc.object();
            //qDebug() <<jsonObj;

            auto curHR = jsonObj.value("hashrate").toDouble()/1000000;
            auto uptime = jsonObj.value("uptime").toInt() / 60;
            auto accepted = jsonObj.value("accepted_count").toInt();
            auto GPUS = jsonObj.value("gpus").toArray();

            QString text = tr("Hashrate: ") + QString::number(curHR, 'f', 3) + "MH/s, ";
            text += tr("Accepted shares: ") + QString::number(accepted) + ", ";
            text += tr("Uptime: ") + QString::number(uptime) + tr(" min") + "<br />";

            int i=1;
            for (auto item : GPUS) {
                 //qDebug() << item.toObject();
                 text += "#" + QString::number(i) + ": " + item.toObject().value("vendor").toString() + " ";
                 text += item.toObject().value("name").toString() + " [";
                 text += tr("F: ") + QString::number(item.toObject().value("fan_speed").toInt()) + "%, ";
                 text += tr("P: ") + QString::number(item.toObject().value("power").toInt()) + "W, ";
                 text += tr("T: ") + QString::number(item.toObject().value("temperature").toInt()) + "C]";
                 i++;
            }

            ui->m_labelGPUMinerStatistics->setText(text);
        } else if (gpu == "amd_xmr"){
            QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObj = jsonDoc.object();

            auto curHR = jsonObj.value("hashrate")["total"][0].toDouble()/1000000;
            auto uptime = jsonObj.value("uptime").toInt() / 60;
            auto accepted = jsonObj.value("results")["shares_good"].toInt();
            auto GPUS = jsonObj.value("gpus").toArray();

            QString text = tr("Hashrate: ") + QString::number(curHR, 'f', 3) + "MH/s, ";
            text += tr("Accepted shares: ") + QString::number(accepted) + ", ";
            text += tr("Uptime: ") + QString::number(uptime) + tr(" min") + "<br />";

            ui->m_labelGPUMinerStatistics->setText(text);
        }
    } else {
        qDebug() << "Error API request: " << reply->error();
    }
}

void MainWindow::on_radioAMD_clicked()
{
    gpu = "amd_xmr";
    m_settings->setValue("miner/gpu_chip", gpu);
    if(email != "")
        ui->m_startGPUMiningButton->setEnabled(true);
}

void MainWindow::on_radioNVIDIA_clicked()
{
    gpu = "nvidia";
    m_settings->setValue("miner/gpu_chip", gpu);
    if(email != "")
        ui->m_startGPUMiningButton->setEnabled(true);
}


void MainWindow::on_lineEditEmail_textChanged(const QString &arg1)
{
    if (arg1.trimmed() == ""){
        ui->m_startGPUMiningButton->setEnabled(false);
        ui->m_startCPUMiningButton->setEnabled(false);
        return;
    }

    ui->m_startCPUMiningButton->setEnabled(true);

    email = arg1.toLower();
    ui->lineEditEmail->setText(email);
    if(!gpu.isEmpty()){
        ui->m_startGPUMiningButton->setEnabled(true);
    }
}

void MainWindow::on_lineEditEmail_editingFinished()
{
    on_lineEditEmail_textChanged(ui->lineEditEmail->text());
}

void MainWindow::on_m_check_GPU_mining_stateChanged(int state)
{
    if (state == 0){
        ui->m_frame_GPU_settings->setEnabled(false);
        m_settings->setValue("miner/gpu_chip", "");
        m_GPU_Mining_Avalible = false;
    } else {
        ui->m_frame_GPU_settings->setEnabled(true);
        m_settings->setValue("miner/gpu_chip", gpu);
        m_GPU_Mining_Avalible = true;
    }
}

void MainWindow::on_m_check_CPU_mining_stateChanged(int state)
{
    if (state == 0){
        ui->m_frame_CPU_settings->setEnabled(false);
        m_CPU_Mining_Avalible = false;
        m_settings->setValue("miner/cpu_active", m_CPU_Mining_Avalible);
    } else {
        ui->m_frame_CPU_settings->setEnabled(true);
        on_lineEditEmail_editingFinished();
        m_CPU_Mining_Avalible = true;
        m_settings->setValue("miner/cpu_active", m_CPU_Mining_Avalible);
    }

}

void MainWindow::on_m_CPUCoresCombo_activated(int index)
{
     m_cpuCoresCount = index + 1;
}


