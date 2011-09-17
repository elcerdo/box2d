#ifndef __SOUND_HH__
#define __SOUND_HH__

#include <QObject>
#include <QAudioOutput>

typedef quint8 Sample;

class Generator : public QIODevice
{
    Q_OBJECT
    public:
	Generator(QObject* parent=NULL);
	~Generator();
	void beepOn();
	qint64 readData(char* buffer, qint64 size);
	qint64 writeData(const char* buffer, qint64 size);
	//qint64 bytesAvailable() const;
    protected:
	qint64 beepcount;
	qint64 nperiod;
	qint64 position;
	Sample* wave;
};


class Sound : public QObject
{
    Q_OBJECT
    public:
	Sound(QObject* parent=NULL);
	void playBeep();
    protected:
	QAudioOutput* device;
	Generator* generator;
};

#endif
