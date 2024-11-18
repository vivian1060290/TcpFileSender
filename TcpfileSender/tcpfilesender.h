#ifndef TCPFILESENDER_H
#define TCPFILESENDER_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QFile>
#include <QDataStream>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

class TcpFileSender : public QDialog {
    Q_OBJECT

public:
    TcpFileSender(QWidget *parent = nullptr);
    ~TcpFileSender();

private slots:
    void onStartClicked();
    void onOpenClicked();
    void onQuitClicked();
    void start();
    void startTransfer();
    void updateClientProgress(qint64 numBytes);
    void openFile();

private:
    QLineEdit *ipLineEdit;          // 输入 IP 地址的行编辑框
    QLineEdit *portLineEdit;        // 输入端口号的行编辑框
    QProgressBar *clientProgressBar; // 进度条
    QLabel *clientStatusLabel;      // 状态标签
    QPushButton *startButton;       // 开始按钮
    QPushButton *openButton;        // 打开文件按钮
    QPushButton *quitButton;        // 退出按钮
    QFile *localFile;               // 本地文件
    QString fileName;               // 文件名
    QTcpSocket tcpClient;           // TCP 客户端
    QByteArray outBlock;            // 发送数据块
    quint64 totalBytes;             // 总字节数
    quint64 bytesWritten;           // 已写入字节数
    quint64 bytesToWrite;           // 剩余字节数
    quint64 loadSize;               // 每次加载的字节数
};

#endif // TCPFILESENDER_H
