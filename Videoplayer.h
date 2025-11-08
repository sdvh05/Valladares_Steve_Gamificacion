#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QVBoxLayout>
#include <QUrl>
#include <QFileInfo>
#include <QCloseEvent>

class VideoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayer(const QString& videoPath, QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QMediaPlayer* player;
    QVideoWidget* videoWidget;
    QAudioOutput* audioOutput;
};

#endif // VIDEOPLAYER_H
