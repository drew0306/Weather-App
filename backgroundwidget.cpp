#include "backgroundwidget.h"
#include <QPainter>
#include <QDebug>

backgroundWidget::backgroundWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void backgroundWidget::setBackgroundImage(const QString &path)
{
    background = QPixmap(path);
    if (background.isNull()) {
        qDebug() << "Failed to load background image:" << path;
    }
    update();
}

void backgroundWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    if (!background.isNull()) {
        QPixmap scaled = background.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, scaled);
    }
}
