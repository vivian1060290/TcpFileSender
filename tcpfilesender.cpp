#include "TcpFileSender.h"

TcpFileSender::TcpFileSender(QWidget *parent)
    : QDialog(parent), totalBytes(0), bytesWritten(0), bytesToWrite(0), loadSize(1024 * 4)
{
    setWindowTitle(QStringLiteral("(煩ㄟ)檔案傳送"));

    ipLineEdit = new QLineEdit(this);
    portLineEdit = new QLineEdit(this);
    clientProgressBar = new QProgressBar(this);
    clientProgressBar->setRange(0, 100);

    startButton = new QPushButton(QStringLiteral("開始"), this);
    openButton = new QPushButton(QStringLiteral("開檔"), this);
    quitButton = new QPushButton(QStringLiteral("退出"), this);
    clientStatusLabel = new QLabel(QStringLiteral("客戶端就緒"), this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(clientProgressBar);
    mainLayout->addWidget(clientStatusLabel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(QStringLiteral("IP:"), ipLineEdit);
    formLayout->addRow(QStringLiteral("Port:"), portLineEdit);
    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(quitButton);
    mainLayout->addLayout(buttonLayout);

    connect(startButton, &QPushButton::clicked, this, &TcpFileSender::onStartClicked);
    connect(openButton, &QPushButton::clicked, this, &TcpFileSender::onOpenClicked);
    connect(quitButton, &QPushButton::clicked, this, &TcpFileSender::onQuitClicked);
    connect(&tcpClient, &QTcpSocket::connected, this, &TcpFileSender::startTransfer);
    connect(&tcpClient, &QTcpSocket::bytesWritten, this, &TcpFileSender::updateClientProgress);

    startButton->setEnabled(false);

    connect(ipLineEdit, &QLineEdit::textChanged, this, [=]() {
        startButton->setEnabled(!ipLineEdit->text().isEmpty() && !portLineEdit->text().isEmpty());
    });

    connect(portLineEdit, &QLineEdit::textChanged, this, [=]() {
        startButton->setEnabled(!ipLineEdit->text().isEmpty() && !portLineEdit->text().isEmpty());
    });

    setLayout(mainLayout);
}

TcpFileSender::~TcpFileSender() {
    // 清理代码，如果需要
}

void TcpFileSender::openFile() {
    fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) startButton->setEnabled(true);
}

void TcpFileSender::onOpenClicked() {
    openFile(); // 调用现有的 openFile 方法
}

void TcpFileSender::onQuitClicked() {
    close(); // 关闭对话框
}

void TcpFileSender::onStartClicked() {
    startButton->setEnabled(false);
    bytesWritten = 0;
    clientStatusLabel->setText(QStringLiteral("連接中..."));

    QString ip = ipLineEdit->text();
    quint16 port = static_cast<quint16>(portLineEdit->text().toUInt());

    tcpClient.connectToHost(QHostAddress(ip), port);
}

void TcpFileSender::startTransfer() {
    localFile = new QFile(fileName);
    if (!localFile->open(QFile::ReadOnly)) {
        QMessageBox::warning(this, QStringLiteral("應用程式"),
                             QStringLiteral("無法讀取 %1:\n%2.").arg(fileName)
                                 .arg(localFile->errorString()));
        return;
    }

    totalBytes = localFile->size();
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    QString currentFile = fileName.right(fileName.size() - fileName.lastIndexOf("/") - 1);
    sendOut << qint64(0) << qint64(0) << currentFile;
    totalBytes += outBlock.size();

    sendOut.device()->seek(0);
    sendOut << totalBytes << qint64((outBlock.size() - sizeof(qint64) * 2));
    bytesToWrite = totalBytes - tcpClient.write(outBlock);
    clientStatusLabel->setText(QStringLiteral("已連接"));
    qDebug() << currentFile << totalBytes;
    outBlock.resize(0);
}

void TcpFileSender::updateClientProgress(qint64 numBytes) {
    bytesWritten += (int)numBytes;
    if (bytesToWrite > 0) {
        outBlock = localFile->read(qMin(bytesToWrite, loadSize));
        bytesToWrite -= (int)tcpClient.write(outBlock);
        outBlock.resize(0);
    } else {
        localFile->close(); // 确保在文件传输完成后关闭文件
        clientStatusLabel->setText(QStringLiteral("傳送完成")); // 更新状态信息
        startButton->setEnabled(true); // 使能开始按钮以便可以再次发送文件
    }
}

void TcpFileSender::start() {
    // 如果有特定的初始化逻辑，可以在这里添加
    // 或者如果不需要特别逻辑，可以移除这个函数
}
