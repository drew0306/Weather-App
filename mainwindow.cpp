#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QNetworkAccessManager* iconManager = new QNetworkAccessManager(this);

    networkManager = new QNetworkAccessManager(this);

    connect(ui->fetchButton, &QPushButton::clicked, this, [=]() {
        QString city = ui->cityInput->text().trimmed();
        if (city.isEmpty()) {
            ui->weatherLabel->setText("Please enter a city.");
            return;
        }

        QString apiKey = "c61217f33df7836384a22acc4b3f677a";
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

        QString iconUrl = QString("http://openweathermap.org/img/wn/%1@2x.png").arg(iconCode);

        QNetworkReply* iconReply = iconManager->get(QNetworkRequest(QUrl(iconUrl)));

        connect(iconReply, &QNetworkReply::finished, this, [=]() {
            QPixmap pix;
            pix.loadFromData(iconReply->readAll());
            ui->iconLabel->setPixmap(pix);
            iconReply->deleteLater();
        });


        reply->deleteLater();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}
