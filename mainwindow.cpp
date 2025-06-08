#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QFile>
#include "backgroundwidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    backgroundWidget *bgWidget = new backgroundWidget(this);

    QNetworkAccessManager* iconManager = new QNetworkAccessManager(this);

    networkManager = new QNetworkAccessManager(this);

    connect(ui->fetchButton, &QPushButton::clicked, this, [=]() {
        QString city = ui->cityInput->text().trimmed();
        if (city.isEmpty()) {
            ui->weatherLabel->setText("Please enter a city.");
            return;
        }

        QString apiKey = getApiKey();
        QString url = QString("https://api.openweathermap.org/data/2.5/weather?q=%1&appid=%2&units=imperial")
                          .arg(city)
                          .arg(apiKey);
        QNetworkRequest request{QUrl(url)};
        networkManager->get(request);
    });

    connect(networkManager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply) {
        if (reply->error()) {
            qDebug() << "Network error:" << reply->errorString();
            ui->weatherLabel->setText("Error fetching weather.");
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        QJsonObject main = obj["main"].toObject();
        QJsonObject wind = obj["wind"].toObject();
        double temp = main["temp"].toDouble();
        double feelsLike = main["feels_like"].toDouble();
        int humidity = main["humidity"].toInt();
        double windSpeed = wind["speed"].toDouble();

        QString description = "No description";
        QString iconCode;

        if (obj.contains("weather") && obj["weather"].isArray()) {
            QJsonArray weatherArray = obj["weather"].toArray();
            if (!weatherArray.isEmpty() && weatherArray[0].isObject()) {
                QJsonObject weatherObj = weatherArray[0].toObject();
                description = weatherObj["description"].toString();
                iconCode = weatherObj["icon"].toString().trimmed();
            }
        }

        // Display info
        ui->weatherLabel->setText(QString("🌡️ %1°F — %2").arg(temp).arg(description));
        ui->feelsLikeLabel->setText(QString("🌡️ Feels like: %1°F").arg(feelsLike));
        ui->humidityLabel->setText(QString("💧 Humidity: %1%").arg(humidity));
        ui->windLabel->setText(QString("💨 Wind: %1 m/s").arg(windSpeed));

        bgWidget->setGeometry(this->rect());
        bgWidget->lower();
        bgWidget->show();

        bgWidget->setBackgroundImage(":/resources/images/default.jpg");

        QString iconUrl = QString("http://openweathermap.org/img/wn/%1@2x.png").arg(iconCode);

        QNetworkReply* iconReply = iconManager->get(QNetworkRequest(QUrl(iconUrl)));

        connect(iconReply, &QNetworkReply::finished, this, [=]() {
            QPixmap pix;
            pix.loadFromData(iconReply->readAll());
            ui->iconLabel->setPixmap(pix);
            iconReply->deleteLater();
        });

        fadeInWidget(ui->weatherLabel);
        fadeInWidget(ui->feelsLikeLabel);
        fadeInWidget(ui->humidityLabel);
        fadeInWidget(ui->windLabel);
        fadeInWidget(ui->iconLabel);


        reply->deleteLater();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setWeatherBackground(const QString& condition)
{
    QString imagePath;

    if (condition.contains("clear", Qt::CaseInsensitive))
        imagePath = ":/resources/images/clear.jpg";
    else if (condition.contains("cloud", Qt::CaseInsensitive))
        imagePath = ":/resources/images/clouds.jpg";
    else if (condition.contains("rain", Qt::CaseInsensitive))
        imagePath = ":/resources/images/rain.jpg";
    else if (condition.contains("snow", Qt::CaseInsensitive))
        imagePath = ":/resources/images/snow.jpg";
    else
        imagePath = ":/resources/images/default.jpg";

    QPixmap bg(imagePath);

    // Scale the image to the size of the background frame
    QPixmap scaled = bg.scaled(
        ui->backgroundFrame->size(),
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
        );

    QPalette pal;
    pal.setBrush(QPalette::Window, scaled);

    ui->backgroundFrame->setAutoFillBackground(true);
    ui->backgroundFrame->setPalette(pal);
    ui->backgroundFrame->update();

    qDebug() << "Exists:" << QFile::exists(imagePath);
}


void MainWindow::fadeInWidget(QWidget* widget)
{
    auto* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);

    auto* anim = new QPropertyAnimation(effect, "opacity", widget);
    anim->setDuration(800);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

/*void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (bgWidget)
        bgWidget->setGeometry(this->rect());
}
*/
QString MainWindow::getApiKey() {
    QFile file("config.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("API_KEY="))
            return line.mid(QString("API_KEY=").length()).trimmed();
    }
    return "";
}

