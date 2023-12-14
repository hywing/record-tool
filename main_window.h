#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Streamer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

    void startRecord(const std::string &name);

    void stopRecord();

protected:
    void setConnectFlag(bool flag);

    void badAckFromServer(const QString &ack);

private:
    Ui::MainWindow *ui;
    Streamer *m_streamer;
    QString m_jsonPath;
};
#endif // MAINWINDOW_H
