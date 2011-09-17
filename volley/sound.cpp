#include "sound.h"

#include <QDebug>
#include <QAudioFormat>
#include <QAudioDeviceInfo>

void printFormat(const QAudioFormat& format)
{
    qDebug() << "***** AUDIO FORMAT *****";
    qDebug() << "rate" << format.frequency();
    qDebug() << "channels" << format.channels();
    qDebug() << "sampleSize" << format.sampleSize();
    qDebug() << "byteOrder" << (format.byteOrder()==QAudioFormat::BigEndian ? "bigendian" : "littleendian");
    qDebug() << "sampleType" << format.sampleType();
    qDebug() << "codec" << format.codec();
    qDebug() << "************************";
}

void Generator::beepOn()
{
    beepcount = 4410;
}

Generator::Generator(QObject* parent)
    : QIODevice(parent)
{
    nperiod = 200;
    position = 0;

    const int amp = 16;
    const int nhigh = 100;
    wave = new Sample[nperiod];
    memset(wave,128-amp,nperiod);
    memset(wave,128+amp,nhigh);
}

Generator::~Generator()
{
    delete [] wave;
}

qint64 Generator::readData(char* buffer, qint64 size)
{
    qint64 total = 0;
    while (total<size)
    {
	const qint64 length = qMin(size-total,nperiod-position);
	if (beepcount>0) {
	    beepcount -= length;
	    memcpy(buffer+total,wave+position,length);
	} else {
	    memset(buffer+total,128,length);
	}
	position += length;
	position %= nperiod;
	total += length;
    }
    Q_ASSERT(total==size);
    return total;
}

qint64 Generator::writeData(const char* buffer, qint64 size)
{
    return 0;
}

//qint64 Generator::bytesAvailable() const
//{
//    //return QIODevice::bytesAvailable();
//    return 128;
//}

Sound::Sound(QObject* parent)
    : QObject(parent), device(NULL)
{
    QAudioFormat format;
    format.setFrequency(44100);
    format.setChannels(1);
    format.setSampleSize(sizeof(Sample)*8);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    format.setCodec("audio/pcm");
    printFormat(format);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
	qDebug() << "trying nearest audio format";
	format = info.nearestFormat(format);
	printFormat(format);
    }

    Q_ASSERT(info.isFormatSupported(format));

    device = new QAudioOutput(format,this);
    generator = new Generator(this);

    generator->open(QIODevice::ReadOnly);
    device->start(generator);
}

void Sound::playBeep()
{
    qDebug() << "beep";
    if (generator) generator->beepOn();
}

