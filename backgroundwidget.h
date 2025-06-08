#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QWidget>
#include <QPixmap>

class backgroundWidget : public QWidget
{
    Q_OBJECT

public:
    explicit backgroundWidget(QWidget *parent = nullptr);
    void setBackgroundImage(const QString &path);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap background;
};

#endif // BACKGROUNDWIDGET_H
