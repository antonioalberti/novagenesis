/****************************************************************************
** Meta object code from reading C++ file 'ngconnectorif_adaptor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ngconnectorif_adaptor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ngconnectorif_adaptor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ConnectionAdaptor_t {
    QByteArrayData data[8];
    char stringdata0[257];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConnectionAdaptor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConnectionAdaptor_t qt_meta_stringdata_ConnectionAdaptor = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ConnectionAdaptor"
QT_MOC_LITERAL(1, 18, 15), // "D-Bus Interface"
QT_MOC_LITERAL(2, 34, 22), // "NGConnector.Connection"
QT_MOC_LITERAL(3, 57, 19), // "D-Bus Introspection"
QT_MOC_LITERAL(4, 77, 165), // "  <interface name=\"NGConnect..."
QT_MOC_LITERAL(5, 228, 8), // "complete"
QT_MOC_LITERAL(6, 237, 0), // ""
QT_MOC_LITERAL(7, 238, 3) // "str"

    },
    "ConnectionAdaptor\0D-Bus Interface\0"
    "NGConnector.Connection\0D-Bus Introspection\0"
    "  <interface name=\"NGConnector.Connection\">\n    <method name=\"comp"
    "lete\">\n      <arg direction=\"in\" type=\"s\" name=\"str\"/>\n    </"
    "method>\n  </interface>\n\0"
    "complete\0\0str"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConnectionAdaptor[] = {

 // content:
       7,       // revision
       0,       // classname
       2,   14, // classinfo
       1,   18, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // classinfo: key, value
       1,    2,
       3,    4,

 // slots: name, argc, parameters, tag, flags
       5,    1,   23,    6, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    7,

       0        // eod
};

void ConnectionAdaptor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ConnectionAdaptor *_t = static_cast<ConnectionAdaptor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->complete((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject ConnectionAdaptor::staticMetaObject = {
    { &QDBusAbstractAdaptor::staticMetaObject, qt_meta_stringdata_ConnectionAdaptor.data,
      qt_meta_data_ConnectionAdaptor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ConnectionAdaptor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConnectionAdaptor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ConnectionAdaptor.stringdata0))
        return static_cast<void*>(const_cast< ConnectionAdaptor*>(this));
    return QDBusAbstractAdaptor::qt_metacast(_clname);
}

int ConnectionAdaptor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractAdaptor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
