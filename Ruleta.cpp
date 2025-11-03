#include "Ruleta.h"
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QApplication>
#include <QFont>
#include <QDebug>
#include <QtMath>

RuletaWidget::RuletaWidget(const QList<bool>& puertas, int resultado, QWidget* parent)
    : QWidget(parent),
    opcionesDisponibles(puertas),
    resultadoFinal(resultado),
    anguloActual(0.0),
    velocidad(40),
    girando(true)
{
    setFixedSize(300, 300);
    setWindowTitle("Ruleta Medieval");

    lblResultado = new QLabel(this);
    lblResultado->setAlignment(Qt::AlignCenter);
    lblResultado->setStyleSheet("color: #f5f0e6;");
    lblResultado->setFont(QFont("Times", 14, QFont::Bold));
    lblResultado->hide();

    btnCerrar = new QPushButton("Cerrar", this);
    btnCerrar->setFont(QFont("Times", 11, QFont::Bold));
    btnCerrar->setStyleSheet(
        "QPushButton{ background-color: #6b3f1f; color: #f5f0e6; border-radius: 6px; padding:6px; }"
        "QPushButton:hover{ background-color: #83502a; }"
        );
    btnCerrar->hide();
    connect(btnCerrar, &QPushButton::clicked, this, &RuletaWidget::cerrarRuleta);

    timerGiro = new QTimer(this);
    connect(timerGiro, &QTimer::timeout, this, &RuletaWidget::girarRuleta);
    timerGiro->start(30);
}

QString RuletaWidget::obtenerTextoCategoria(int indice) const
{
    switch (indice)
    {
    case 0: return "Arte";
    case 1: return "Política";
    case 2: return "Ciencia";
    case 3: return "Historia";
    default: return "Desconocido";
    }
}

QColor RuletaWidget::obtenerColorCategoria(int indice) const
{
    switch (indice)
    {
    case 0: return QColor(130, 70, 150);   // Arte
    case 1: return QColor(120, 30, 30);    // Política
    case 2: return QColor(40, 100, 60);    // Ciencia
    case 3: return QColor(150, 120, 40);   // Historia
    default: return QColor(100, 100, 100);
    }
}

void RuletaWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Centro y radio
    int cx = width() / 2;
    int cy = height() / 2;
    int radio = 120;

    // Fondo tipo madera/medieval
    QLinearGradient fondoGrad(0, 0, 0, height());
    fondoGrad.setStart(0, 0);
    fondoGrad.setFinalStop(0, height());
    fondoGrad.setColorAt(0.0, QColor(38, 25, 12));   // muy oscuro
    fondoGrad.setColorAt(1.0, QColor(67, 41, 21));   // marrón madera
    p.fillRect(rect(), fondoGrad);

    // Aro exterior
    QRectF ruedaRect(cx - radio, cy - radio, radio * 2, radio * 2);
    QRadialGradient rimGrad(ruedaRect.center(), radio);
    rimGrad.setColorAt(0.0, QColor(220, 200, 160));
    rimGrad.setColorAt(0.8, QColor(90, 60, 30));
    rimGrad.setColorAt(1.0, QColor(40, 25, 12));
    p.setBrush(rimGrad);
    p.setPen(QPen(QColor(35, 20, 8), 8));
    p.drawEllipse(ruedaRect);

    //Indices Disponibles (bool)
    QStringList nombres = {"Arte", "Política", "Ciencia", "Historia"};
    QVector<QColor> pal = { QColor(130, 70, 150), QColor(120, 30, 30),
                           QColor(40, 100, 60), QColor(150, 120, 40) };

    QVector<int> disponiblesIdx;
    for (int i = 0; i < opcionesDisponibles.size() && i < 4; ++i)
        if (!opcionesDisponibles[i]) disponiblesIdx.append(i);

    int nDisp = disponiblesIdx.size();
    if (nDisp == 0)
    {
        // nada disponible: mensaje
        p.setPen(Qt::white);
        p.setFont(QFont("Times", 16, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "No hay opciones disponibles");
        return;
    }

    double angSector = 360.0 / nDisp;
    double angInicio = anguloActual; // empezamos a rotar por este angulo

    for (int s = 0; s < nDisp; ++s)
    {
        int idx = disponiblesIdx[s];
        QColor base = pal[idx];

        QRadialGradient grad(ruedaRect.center(), radio * 0.9,
                             QPointF(ruedaRect.center().x() + radio * 0.2, ruedaRect.center().y() - radio * 0.2));
        grad.setColorAt(0.0, base.lighter(120));
        grad.setColorAt(0.6, base);
        grad.setColorAt(1.0, base.darker(140));

        p.setBrush(grad);
        p.setPen(QPen(QColor(30, 20, 10), 2));


        p.save();
        p.translate(0, 0);
        p.drawPie(ruedaRect, static_cast<int>(-angInicio * 16), static_cast<int>(-angSector * 16));
        p.restore();

        double midAng = angInicio + angSector / 2.0;
        p.save();
        p.translate(cx, cy);
        p.rotate(-midAng);

        double textDist = radio * 0.62;
        p.translate(textDist, 0);

        p.setPen(QPen(QColor(240, 235, 220)));
        QFont f("Times", 11, QFont::Bold);
        p.setFont(f);
        QRectF txtRect(-60, -12, 120, 24);
        p.drawText(txtRect, Qt::AlignCenter, nombres[idx]);
        p.restore();


        angInicio += angSector;
    }

    // Centrar bola / medallón en el centro
    QRectF centroRect(cx - radio * 0.26, cy - radio * 0.26, radio * 0.52, radio * 0.52);
    QRadialGradient medGrad(centroRect.center(), centroRect.width() / 2);
    medGrad.setColorAt(0.0, QColor(245, 240, 220));
    medGrad.setColorAt(1.0, QColor(120, 80, 40));
    p.setBrush(medGrad);
    p.setPen(QPen(QColor(40, 20, 10), 3));
    p.drawEllipse(centroRect);

    // Decoraciones: anillos concéntricos
    p.setPen(QPen(QColor(20, 12, 6), 2));
    p.drawEllipse(ruedaRect.adjusted(10, 10, -10, -10));
    p.drawEllipse(ruedaRect.adjusted(40, 40, -40, -40));

    // Triángulo
    QPointF a(cx - 18, cy - radio - 6);
    QPointF b(cx + 18, cy - radio - 6);
    QPointF c(cx, cy - radio + 22); // punta hacia abajo
    QPolygonF flecha;
    flecha << a << b << c;
    p.setBrush(QColor(30, 20, 10));
    p.setPen(QPen(QColor(200, 180, 120), 2));
    p.drawPolygon(flecha);
    //contorno
    p.setPen(QPen(QColor(120, 90, 60), 2));
    p.drawPolyline(flecha);

    // Si la ruleta ya se detuvo, dibujar un marco y el resultado en el centro
    if (!girando)
    {
        p.setPen(QPen(QColor(245, 240, 220)));
        QFont big("Times", 10, QFont::Bold);
        p.setFont(big);
        QString txt = obtenerTextoCategoria(resultadoFinal);
        p.drawText(centroRect, Qt::AlignCenter, txt);
    }
}

void RuletaWidget::girarRuleta()
{
    if (!girando) return;

    if (!objetivoCalculado)
    {
        QVector<int> disponiblesIdx;
        for (int i = 0; i < opcionesDisponibles.size() && i < 4; ++i)
            if (!opcionesDisponibles[i])
                disponiblesIdx.append(i);

        int nDisp = disponiblesIdx.size();
        if (nDisp == 0)
        {
            qWarning() << "No hay categorías disponibles.";
            detenerRuleta();
            return;
        }

        double angSector = 360.0 / nDisp;
        int posResultado = disponiblesIdx.indexOf(resultadoFinal);
        if (posResultado == -1) posResultado = 0;

        double angSectorInicio = posResultado * angSector;
        anguloObjetivo = 360.0 - (angSectorInicio + angSector / 2.0);
        anguloObjetivo += 720.0; // dos vueltas extra
        objetivoCalculado = true;
    }

    anguloActual += velocidad * 0.6;
    update();

    if (velocidad > 2)
        velocidad -= 1;
    else if (velocidad > 0)
        velocidad -= 0.25;

    if (velocidad <= 0 || anguloActual >= anguloObjetivo)
    {
        anguloActual = fmod(anguloObjetivo, 360.0);
        velocidad = 0;
        timerGiro->stop();
        detenerRuleta();
    }
}

void RuletaWidget::detenerRuleta()
{
    girando = false;

    lblResultado->setText("Resultado: " + obtenerTextoCategoria(resultadoFinal));
    lblResultado->setGeometry(width()/2 - 140, height() - 110, 280, 30);
    lblResultado->show();

    btnCerrar->setGeometry(width()/2 - 55, height() - 70, 110, 36);
    btnCerrar->show();

    update();
}

void RuletaWidget::cerrarRuleta()
{
    emit ruletaFinalizada(resultadoFinal);
    this->close();
}
