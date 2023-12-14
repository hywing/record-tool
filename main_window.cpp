#include "main_window.h"
#include "./ui_main_window.h"
#include "streamer.h"
#include "message_manager.h"
#include "switch_button.h"
#include <QDateTime>
#include <QVBoxLayout>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_streamer(new Streamer(this))
{
    ui->setupUi(this);
    ui->importButton->setDisabled(true);
    ui->runButton->setDisabled(true);
    ui->recordButton->setDisabled(true);

    m_streamer->start();

    auto instance = MessageManager::getInstance();
    instance->setConnectCallback(std::bind(&MainWindow::setConnectFlag, this, std::placeholders::_1));
    instance->setBadAckFromServerCallback(std::bind(&MainWindow::badAckFromServer, this, std::placeholders::_1));
    instance->setRecordCallback(std::bind(&MainWindow::startRecord, this, std::placeholders::_1), std::bind(&MainWindow::stopRecord, this));
    connect(ui->connectButton, &QPushButton::clicked, this, [=]() {
        instance->open();
    });

    connect(ui->importButton, &QPushButton::clicked, this, [=]() {
        ui->runButton->setDisabled(true);
        auto fileName = QFileDialog::getOpenFileName(this, tr("Open Json"), "./", tr("Json File(*.json)"));
        if(fileName.isEmpty() == false) {
            m_jsonPath = fileName;
            QFile file(fileName);
            if(file.open(QIODevice::ReadOnly)) {
                auto valid = instance->checkJsonData(file.readAll().simplified().trimmed());
                if(valid) {
                    ui->runButton->setDisabled(false);

                }
                else {
                    QMessageBox::information(this, "error", QString("import invalid json file"));
                    ui->runButton->setDisabled(true);
                }
            }
            file.close();
        }
        else {
            qDebug() << "import file error ...";
        }
    });

    connect(ui->runButton, &QPushButton::clicked, this, [=]() {
        if(m_jsonPath.isEmpty() == false) {
            QFile file(m_jsonPath);
            if(file.open(QIODevice::ReadOnly)) {
                auto valid = instance->checkJsonData(file.readAll().simplified().trimmed());
                if(valid) {
                    ui->runButton->setDisabled(false);
                    ui->recordButton->setDisabled(false);
                }
                else {
                    QMessageBox::information(this, "error", QString("run invalid json file"));
                    ui->runButton->setDisabled(true);
                }
            }
            file.close();
            instance->runCommand();

            QMessageBox::information(this, "info", QString("run command ok"));
        }
        else {
            QMessageBox::information(this, "error", QString("run file not exist"));
        }
    });

    connect(ui->recordButton, &QPushButton::clicked, this, [=]() {
        auto name = QString("%1-.pcap").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz"));
        QFileDialog fileDialog;
        QString fileName = fileDialog.getSaveFileName(this, tr("Save Pcap File"), name, tr("Pcap File(*.pcap)"));
        if(fileName == "")
        {
            return;
        }
        instance->startToCollect(fileName);
    });

    instance->setTimerCount(ui->spinBox->value());
//    qDebug() << "value : " << ui->spinBox->value();
    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value) {
        qDebug() << "value : " << ui->spinBox->value();
        instance->setTimerCount(value);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startRecord(const std::string &name)
{
    m_streamer->startRecording(name);
}

void MainWindow::stopRecord()
{
    m_streamer->stopRecording();
}

void MainWindow::setConnectFlag(bool flag)
{
    if(flag) {
        ui->connectLabel->setStyleSheet("QLabel#connectLabel{background-color:rgb(0, 255, 0);border-radius:10px;border:3px groove gray;border-style:outset;}");
        ui->importButton->setDisabled(false);
    }
    else {
        ui->connectLabel->setStyleSheet("QLabel#connectLabel{background-color:rgb(195,195,195);border-radius:10px;border:3px groove gray;border-style:outset;}");
        ui->importButton->setDisabled(true);
        ui->runButton->setDisabled(true);
    }
}

void MainWindow::badAckFromServer(const QString &ack)
{
    QMessageBox::information(this, "error", QString("bad ack from server : %1").arg(ack));
    ui->recordButton->setDisabled(false);
}
