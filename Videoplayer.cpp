#include "VideoPlayer.h"
#include <QApplication>
#include <QScreen>
#include <QDebug>

VideoPlayer::VideoPlayer(const QString& videoPath, QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("Reproductor de Video");
    resize(800, 450);

    QFileInfo file(videoPath);
    if (!file.exists()) {
        qWarning() << "El archivo de video no existe:" << videoPath;
        return;
    }

    // --- Crear reproductor, salida de audio y widget de video ---
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    videoWidget = new QVideoWidget(this);

    player->setAudioOutput(audioOutput);
    player->setVideoOutput(videoWidget);

    // Volumen inicial
    audioOutput->setVolume(1.0);

    // Layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(videoWidget);
    setLayout(layout);

    player->setSource(QUrl::fromLocalFile(videoPath));

    // Cerrar ventana al finalizar video
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia)
            close();
    });

    move(QApplication::primaryScreen()->geometry().center() - rect().center());
    player->play();
}

// --- 🔹 Evento de cierre manual ---
void VideoPlayer::closeEvent(QCloseEvent* event)
{
    if (player) {
        player->stop();       // Detiene la reproducción
        player->setSource(QUrl()); // Libera el recurso del video
    }
    QWidget::closeEvent(event); // Llama al comportamiento base
}
