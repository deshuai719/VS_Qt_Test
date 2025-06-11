/****************************************************************************
** Meta object code from reading C++ file 'CentralWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/CentralWindow.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CentralWindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSCWDSCOPEModelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCWDSCOPEModelENDCLASS = QtMocHelpers::stringData(
    "CWD::Model"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSCWDSCOPEModelENDCLASS_t {
    uint offsetsAndSizes[2];
    char stringdata0[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSCWDSCOPEModelENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSCWDSCOPEModelENDCLASS_t qt_meta_stringdata_CLASSCWDSCOPEModelENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10)   // "CWD::Model"
    },
    "CWD::Model"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCWDSCOPEModelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject CWD::Model::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_meta_stringdata_CLASSCWDSCOPEModelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCWDSCOPEModelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCWDSCOPEModelENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Model, std::true_type>
    >,
    nullptr
} };

void CWD::Model::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *CWD::Model::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CWD::Model::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCWDSCOPEModelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int CWD::Model::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSCWDSCOPEDelegateENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCWDSCOPEDelegateENDCLASS = QtMocHelpers::stringData(
    "CWD::Delegate"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSCWDSCOPEDelegateENDCLASS_t {
    uint offsetsAndSizes[2];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSCWDSCOPEDelegateENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSCWDSCOPEDelegateENDCLASS_t qt_meta_stringdata_CLASSCWDSCOPEDelegateENDCLASS = {
    {
        QT_MOC_LITERAL(0, 13)   // "CWD::Delegate"
    },
    "CWD::Delegate"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCWDSCOPEDelegateENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject CWD::Delegate::staticMetaObject = { {
    QMetaObject::SuperData::link<QStyledItemDelegate::staticMetaObject>(),
    qt_meta_stringdata_CLASSCWDSCOPEDelegateENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCWDSCOPEDelegateENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCWDSCOPEDelegateENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Delegate, std::true_type>
    >,
    nullptr
} };

void CWD::Delegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *CWD::Delegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CWD::Delegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCWDSCOPEDelegateENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QStyledItemDelegate::qt_metacast(_clname);
}

int CWD::Delegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSCWDSCOPECentralWidgetENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCWDSCOPECentralWidgetENDCLASS = QtMocHelpers::stringData(
    "CWD::CentralWidget",
    "NetRecovery",
    "",
    "NetLoss",
    "UpdateSockInfo",
    "StartTest",
    "OpenLog",
    "UpdateCheckResult",
    "TASKWZ::MsgToCentralWt",
    "msg",
    "UpdateVersion",
    "ClockEvent",
    "NetConnected",
    "NetDisconnected",
    "TimingDetectionEvent"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSCWDSCOPECentralWidgetENDCLASS_t {
    uint offsetsAndSizes[30];
    char stringdata0[19];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[15];
    char stringdata5[10];
    char stringdata6[8];
    char stringdata7[18];
    char stringdata8[23];
    char stringdata9[4];
    char stringdata10[14];
    char stringdata11[11];
    char stringdata12[13];
    char stringdata13[16];
    char stringdata14[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSCWDSCOPECentralWidgetENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSCWDSCOPECentralWidgetENDCLASS_t qt_meta_stringdata_CLASSCWDSCOPECentralWidgetENDCLASS = {
    {
        QT_MOC_LITERAL(0, 18),  // "CWD::CentralWidget"
        QT_MOC_LITERAL(19, 11),  // "NetRecovery"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 7),  // "NetLoss"
        QT_MOC_LITERAL(40, 14),  // "UpdateSockInfo"
        QT_MOC_LITERAL(55, 9),  // "StartTest"
        QT_MOC_LITERAL(65, 7),  // "OpenLog"
        QT_MOC_LITERAL(73, 17),  // "UpdateCheckResult"
        QT_MOC_LITERAL(91, 22),  // "TASKWZ::MsgToCentralWt"
        QT_MOC_LITERAL(114, 3),  // "msg"
        QT_MOC_LITERAL(118, 13),  // "UpdateVersion"
        QT_MOC_LITERAL(132, 10),  // "ClockEvent"
        QT_MOC_LITERAL(143, 12),  // "NetConnected"
        QT_MOC_LITERAL(156, 15),  // "NetDisconnected"
        QT_MOC_LITERAL(172, 20)   // "TimingDetectionEvent"
    },
    "CWD::CentralWidget",
    "NetRecovery",
    "",
    "NetLoss",
    "UpdateSockInfo",
    "StartTest",
    "OpenLog",
    "UpdateCheckResult",
    "TASKWZ::MsgToCentralWt",
    "msg",
    "UpdateVersion",
    "ClockEvent",
    "NetConnected",
    "NetDisconnected",
    "TimingDetectionEvent"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCWDSCOPECentralWidgetENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x06,    1 /* Public */,
       3,    0,   81,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   82,    2, 0x0a,    3 /* Public */,
       5,    0,   83,    2, 0x0a,    4 /* Public */,
       6,    0,   84,    2, 0x0a,    5 /* Public */,
       7,    1,   85,    2, 0x0a,    6 /* Public */,
      10,    0,   88,    2, 0x0a,    8 /* Public */,
      11,    0,   89,    2, 0x0a,    9 /* Public */,
      12,    0,   90,    2, 0x0a,   10 /* Public */,
      13,    0,   91,    2, 0x0a,   11 /* Public */,
      14,    0,   92,    2, 0x0a,   12 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject CWD::CentralWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSCWDSCOPECentralWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCWDSCOPECentralWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCWDSCOPECentralWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CentralWidget, std::true_type>,
        // method 'NetRecovery'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'NetLoss'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'UpdateSockInfo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'StartTest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'OpenLog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'UpdateCheckResult'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<TASKWZ::MsgToCentralWt, std::false_type>,
        // method 'UpdateVersion'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ClockEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'NetConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'NetDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'TimingDetectionEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void CWD::CentralWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CentralWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->NetRecovery(); break;
        case 1: _t->NetLoss(); break;
        case 2: _t->UpdateSockInfo(); break;
        case 3: _t->StartTest(); break;
        case 4: _t->OpenLog(); break;
        case 5: _t->UpdateCheckResult((*reinterpret_cast< std::add_pointer_t<TASKWZ::MsgToCentralWt>>(_a[1]))); break;
        case 6: _t->UpdateVersion(); break;
        case 7: _t->ClockEvent(); break;
        case 8: _t->NetConnected(); break;
        case 9: _t->NetDisconnected(); break;
        case 10: _t->TimingDetectionEvent(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CentralWidget::*)();
            if (_t _q_method = &CentralWidget::NetRecovery; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CentralWidget::*)();
            if (_t _q_method = &CentralWidget::NetLoss; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *CWD::CentralWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CWD::CentralWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCWDSCOPECentralWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CWD::CentralWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void CWD::CentralWidget::NetRecovery()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CWD::CentralWidget::NetLoss()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
