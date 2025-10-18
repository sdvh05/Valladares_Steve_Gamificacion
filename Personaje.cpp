#include "Personaje.h"
#include<QDebug>

Personaje::Personaje(QWidget*parent):QLabel(parent),frameActual(0),miradoDerecha(true),ultimaDireccionDerecha(true){

    this->resize(128,128);
    this->move(600,500);


    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&Personaje::AvanzarFrame);

}

void Personaje::SetAnimacion(const QString &ruta, int cantidadFrames)
{

    bool debeRefrescarFlip=false;

    if(miradoDerecha!=ultimaDireccionDerecha)
    {

        debeRefrescarFlip=true;
        ultimaDireccionDerecha=miradoDerecha;

    }

    if(animacionActual==ruta&&!debeRefrescarFlip)
    {

        if(!timer->isActive())
            timer->start(100);
        return;

    }

    QPixmap spriteSheet(ruta);
    if(spriteSheet.isNull())
    {

        qDebug()<<"no se pudo cargar la animacion: "<<ruta;
        return;

    }
    animacionActual=ruta;
    frames.clear();
    frameActual=0;

    int frameAncho=spriteSheet.width()/cantidadFrames;

    for(int i=0;i<cantidadFrames;++i)
    {

        QPixmap frame = spriteSheet.copy(i*frameAncho,0,frameAncho,spriteSheet.height());
        if(!miradoDerecha)
        {

            frame=frame.transformed(QTransform().scale(-1,1));

        }
        frames.append(frame);

    }

    setPixmap(frames[frameActual]);

    timer->start(100);


}

void Personaje::DetenerAnimacion()
{

    timer->stop();
    if(!frames.isEmpty())
    {

        setPixmap(frames[0]);

    }
    animacionActual.clear();

}

void Personaje::AvanzarFrame()
{

    if(frames.isEmpty())return;

    frameActual=(frameActual+1)%frames.size();
    setPixmap(frames[frameActual]);

}

void Personaje::MoverIzquierda()
{

    this->move(x()-getVelocidadMovimiento(),y());
    miradoDerecha=false;

}

void Personaje::MoverDerecha()
{

    this->move(x()+getVelocidadMovimiento(),y());
    miradoDerecha=true;

}

void Personaje::MoverArriba()
{

    this->move(x(),y()-getVelocidadMovimiento());

}

void Personaje::MoverAbajo()
{

    this->move(x(),y()+getVelocidadMovimiento());

}

void Personaje::MoverSiNoColisiona(int dx, int dy, const QVector<QRect> &obstaculos)
{

    QRect nuevaPos=this->geometry();
    nuevaPos.translate(dx,dy);

    //con este for, revisa la colision con cada obstaculo
    for(const QRect& obs:obstaculos)
    {

        if(nuevaPos.intersects(obs))
        {
            return;
        }

    }
    //si no, si se mueve
    this->move(nuevaPos.topLeft());

    //aqui se actualiza la direccion para el flip
    if(dx>0)miradoDerecha=true;
    else if(dx<0)miradoDerecha=false;

}

int Personaje::getCorazones(){
    return this->corazones;
}

void Personaje::AumentarCorazones(){
    int nuevaCantidad=this->getCorazones()+1;

    if (nuevaCantidad < 0) nuevaCantidad = 0;
    if (nuevaCantidad > 4) nuevaCantidad = 4;

    corazones = nuevaCantidad;
}



Personaje::DatosAnimacion Personaje::obtenerAnimacion(const QString &tipo){

    QString base="Sprites/Personajes/Prota/";
    DatosAnimacion datos;



        if(tipo=="idle")     datos={base+"Idle.png",         6};
        else if(tipo=="run") datos={base+"Run.png",          10};
        else if(tipo=="walk")datos={base+"Walk.png",         10};
        else if(tipo=="dead") datos={base+"Dead.png",        4};
        else if(tipo=="melee") datos={base+"Attack.png", 5};



    return datos;

}

void Personaje::SetAnimacionMovimiento(int velocidad)
{
        //qDebug() << "velocidad" << velocidad;
        this->velocidadMovimiento=velocidad;

}





