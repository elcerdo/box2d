/****************************************************************************
** Meta object code from reading C++ file 'world.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "world.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'world.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_World[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,
      28,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
      48,   39,    6,    6, 0x0a,
      66,    6,    6,    6, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_World[] = {
    "World\0\0worldStepped(World*)\0testMark()\0"
    "stepping\0setStepping(bool)\0stepWorld()\0"
};

const QMetaObject World::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_World,
      qt_meta_data_World, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &World::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *World::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *World::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_World))
        return static_cast<void*>(const_cast< World*>(this));
    return QObject::qt_metacast(_clname);
}

int World::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: worldStepped((*reinterpret_cast< World*(*)>(_a[1]))); break;
        case 1: testMark(); break;
        case 2: setStepping((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: stepWorld(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void World::worldStepped(World * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void World::testMark()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
