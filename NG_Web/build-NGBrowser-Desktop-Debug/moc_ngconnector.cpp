/****************************************************************************
** Meta object code from reading C++ file 'ngconnector.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../NGBrowser/ngconnector.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ngconnector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NGConnector_t {
    QByteArrayData data[11];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NGConnector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NGConnector_t qt_meta_stringdata_NGConnector = {
    {
QT_MOC_LITERAL(0, 0, 11), // "NGConnector"
QT_MOC_LITERAL(1, 12, 11), // "showMessage"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 3), // "str"
QT_MOC_LITERAL(4, 29, 15), // "completeProcess"
QT_MOC_LITERAL(5, 45, 8), // "arrFiles"
QT_MOC_LITERAL(6, 54, 8), // "complete"
QT_MOC_LITERAL(7, 63, 15), // "searchByLiteral"
QT_MOC_LITERAL(8, 79, 6), // "phrase"
QT_MOC_LITERAL(9, 86, 14), // "searchByMurmur"
QT_MOC_LITERAL(10, 101, 6) // "murmur"

    },
    "NGConnector\0showMessage\0\0str\0"
    "completeProcess\0arrFiles\0complete\0"
    "searchByLiteral\0phrase\0searchByMurmur\0"
    "murmur"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NGConnector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   45,    2, 0x0a /* Public */,
       7,    1,   48,    2, 0x0a /* Public */,
       9,    1,   51,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QStringList,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,   10,

       0        // eod
};

void NGConnector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NGConnector *_t = static_cast<NGConnector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->completeProcess((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 2: _t->complete((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->searchByLiteral((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->searchByMurmur((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NGConnector::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NGConnector::showMessage)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (NGConnector::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NGConnector::completeProcess)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject NGConnector::staticMetaObject = {
    { &NGConnectorIf::staticMetaObject, qt_meta_stringdata_NGConnector.data,
      qt_meta_data_NGConnector,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NGConnector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NGConnector::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NGConnector.stringdata0))
        return static_cast<void*>(const_cast< NGConnector*>(this));
    return NGConnectorIf::qt_metacast(_clname);
}

int NGConnector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = NGConnectorIf::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void NGConnector::showMessage(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NGConnector::completeProcess(QStringList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
