#include "LogBrowser.h"

LogBrowser::LogBrowser(QWidget *parent)
    : QWidget(parent)
{
    this->resize(500, 400);
    is_finished = false;

    browser = new QTextBrowser();
    start_button = new QPushButton();
    clear_button = new QPushButton();

    start_button->setText("start");
    clear_button->setText("clear");

    QHBoxLayout *button_layout = new QHBoxLayout();
    button_layout->addStretch();
    button_layout->addWidget(start_button);
    button_layout->addWidget(clear_button);
    button_layout->setSpacing(10);
    button_layout->setContentsMargins(0, 0, 10, 10);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addWidget(browser);
    main_layout->addLayout(button_layout);
    main_layout->setSpacing(10);
    main_layout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(main_layout);
//    connect(start_button, &QPushButton::clicked, this, &LogBrowser::start);
    connect(clear_button, &QPushButton::clicked, browser, &QTextBrowser::clear);
}

LogBrowser::~LogBrowser()
{
}

void LogBrowser::outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString message;
    switch (type)
    {
    case QtDebugMsg:
        message = QString("Debug:");
        break;

    case QtWarningMsg:
        message = QString("Warning:");
        break;

    case QtCriticalMsg:
        message = QString("Critical:");
        break;

    case QtFatalMsg:
        message = QString("Fatal:");
    }

    browser->append(message.append(msg));
}


void LogBrowser::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton answer = QMessageBox::question(
        this,
        tr("Close Log Browser?"),
        tr("Do you really want to close the log browser?"),
        QMessageBox::Yes | QMessageBox::No);
    if (answer == QMessageBox::Yes)
        event->accept();
    else
        event->ignore();
}

void LogBrowser::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
}
