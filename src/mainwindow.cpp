#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QCryptographicHash>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_gpu_miner_procces(new QProcess(this))
    , m_cpu_miner_procces(new QProcess(this))
    , m_timer(new QTimer())
{
    ui->setupUi(this);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimerMinerRestart()));

    ui->radioAMD->setStatusTip(tr("AMD Radeon series GPUs."));
    ui->radioNVIDIA->setStatusTip(tr("nVIDIA GeForce series GPUs."));
    ui->lineEditEmail->setStatusTip(tr("Your e-mail must be the same as the one you used to register on the site."));
    ui->lineEditEmail->setToolTip(tr("Your e-mail must be the same as the one you used to register on the site."));
    ui->m_groupPerfomanceProfile->setStatusTip(tr("Please pay attention to selecting the performance profile! Your income depends on it! The more accurate you specify a performance profile, the better your mining result is."));

    ui->m_labelGPUMinerStatus->setStyleSheet("color: rgb(200, 0, 0)");
    ui->m_labelCPUMinerStatus->setStyleSheet("color: rgb(200, 0, 0)");

    m_settings.reset(new QSettings("Fortune1Coin", "GoldMiner"));
    ui->lineEditEmail->setText(m_settings->value("miner/email_address").toString());
    email = ui->lineEditEmail->text().trimmed();

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
            ui->m_startGPUMiningButton->setChecked(false);
            ui->m_startGPUMiningButton->setText(tr("Start mining"));
            ui->m_labelGPUMinerStatus->setStyleSheet("color: rgb(200, 0, 0)");
            ui->m_labelGPUMinerStatus->setText(tr("Miner stoped"));
        });
    connect(m_cpu_miner_procces, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus){
            int e = exitCode;
            QProcess::ExitStatus es = exitStatus;
            qDebug() << e << es;
            ui->m_startCPUMiningButton->setChecked(false);
            ui->m_startCPUMiningButton->setText(tr("Start mining"));
            ui->m_labelCPUMinerStatus->setStyleSheet("color: rgb(200, 0, 0)");
            ui->m_labelCPUMinerStatus->setText(tr("Miner stoped"));
        });

    ui->m_frame_GPU_settings->setEnabled(false);
    ui->m_frame_CPU_settings->setEnabled(false);
    initCpuCoresCombo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startGPUMining() {
    bool gpu_miner_started = false;    
    email = ui->lineEditEmail->text().trimmed();
    if(gpu == "" || email ==  "" || port == "" ||  m_gpu_miner_procces->state() == QProcess::Running)
        return;

    QByteArray ba = email.toUtf8();
    m_settings->setValue("miner/email_address", email);
//    qDebug() << email;
//    qDebug() <<  QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex();
    {
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
                      << "--url=" + QByteArray::fromBase64((QString("MzcuOS4zLjI2OjQ0Mw") + "==").toUtf8())
                      << "--user=" + address
                      << "--pass=" + QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex()
                      << "--donate-level=1";

        }

    //    qDebug() << program;
    //    qDebug() << arguments;
        if (debug_mode){
            m_gpu_miner_procces->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args){
                args->flags |= CREATE_NEW_CONSOLE;
                args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
            });
        }

        m_gpu_miner_procces->start(program, arguments);
        if (m_gpu_miner_procces->waitForStarted()){
            gpu_miner_started = true;
        }

    }

    if(gpu_miner_started){
        ui->m_startGPUMiningButton->setChecked(true);
        ui->m_startGPUMiningButton->setText(tr("Stop mining"));
        ui->m_labelGPUMinerStatus->setStyleSheet("color: rgb(0, 200, 0)");
        ui->m_labelGPUMinerStatus->setText(tr("The miner is working... Check your statistics on the website every day."));
        m_timer->start(60000);
    }
}

void MainWindow::stopGPUMining() {
    m_gpu_miner_procces->kill();
    if (!m_gpu_miner_procces->waitForFinished()){
        qDebug() << m_gpu_miner_procces->error();
        return;
    }

    ui->m_startGPUMiningButton->setChecked(false);
    ui->m_startGPUMiningButton->setText(tr("Start mining"));
    ui->m_labelGPUMinerStatus->setStyleSheet("color: rgb(200, 0, 0)");
    ui->m_labelGPUMinerStatus->setText(tr("Miner stoped"));
    m_timer->stop();

}

void MainWindow::startCPUMining() {
    bool cpu_miner_started = false;
    email = ui->lineEditEmail->text().trimmed();
    if(email ==  "" ||  m_cpu_miner_procces->state() == QProcess::Running)
        return;

    QByteArray ba = email.toUtf8();
    m_settings->setValue("miner/email_address", email);
//    qDebug() << email;
//    qDebug() <<  QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex();

    QString params;
    QString program = QCoreApplication::applicationDirPath() + "/dll/";
    QStringList arguments = QStringList();

    program += "nhcpu";
    address = QByteArray::fromBase64((addr_vrsc_part1+addr_vrsc_part2+"==").toUtf8());

    QString tmpaddr = address;
    arguments << "-v"
              << "-l" << QByteArray::fromBase64((QString("NzkuMTM3LjcwLjQ4OjM5NTY") + "=").toUtf8())
              << "-u" << address + "." + QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex()
              << "-p" << "x"
              << "-t" << QString::number(m_cpuCoresCount).toUtf8();



    qDebug() << program;
    qDebug() << arguments;
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
        ui->m_startCPUMiningButton->setChecked(true);
        ui->m_startCPUMiningButton->setText(tr("Stop mining"));
        ui->m_labelCPUMinerStatus->setStyleSheet("color: rgb(0, 200, 0)");
        ui->m_labelCPUMinerStatus->setText(tr("The miner is working... Check your statistics on the website every day."));
        m_timer->start(60000);
    }
}

void MainWindow::stopCPUMining() {
    m_cpu_miner_procces->kill();
    if (!m_cpu_miner_procces->waitForFinished()){
        qDebug() << m_cpu_miner_procces->error();
        return;
    }

    ui->m_startCPUMiningButton->setChecked(false);
    ui->m_startCPUMiningButton->setText(tr("Start mining"));
    ui->m_labelCPUMinerStatus->setStyleSheet("color: rgb(200, 0, 0)");
    ui->m_labelCPUMinerStatus->setText(tr("Miner stoped"));
    m_timer->stop();

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

void MainWindow::on_radioAMD_clicked()
{
    gpu = "amd_xmr";
    if(port != "" && email != "")
        ui->m_startGPUMiningButton->setEnabled(true);
}

void MainWindow::on_radioNVIDIA_clicked()
{
    gpu = "nvidia";
    if(port != "" && email != "")
        ui->m_startGPUMiningButton->setEnabled(true);
}

void MainWindow::on_m_radioLowProfile_clicked()
{
    port = "8866";
    if(gpu != "" && email != "")
        ui->m_startGPUMiningButton->setEnabled(true);
}

void MainWindow::on_m_radioMidleProfile_clicked()
{
    port = "8877";
    if(gpu != "" && email != "")
        ui->m_startGPUMiningButton->setEnabled(true);
}

void MainWindow::on_m_radioHighProfile_clicked()
{
    port = "8888";
    if(gpu != "" && email != "")
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
    if(!gpu.isEmpty() && !port.isEmpty()){
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
        m_GPU_Mining_Avalible = false;        
    } else {
        ui->m_frame_GPU_settings->setEnabled(true);
        m_GPU_Mining_Avalible = true;
    }
}

void MainWindow::on_m_check_CPU_mining_stateChanged(int state)
{
    if (state == 0){
        ui->m_frame_CPU_settings->setEnabled(false);
        m_CPU_Mining_Avalible = false;
    } else {
        ui->m_frame_CPU_settings->setEnabled(true);
        on_lineEditEmail_editingFinished();
        m_CPU_Mining_Avalible = true;
    }

}

void MainWindow::on_m_CPUCoresCombo_activated(int index)
{
     m_cpuCoresCount = index + 1;
}


