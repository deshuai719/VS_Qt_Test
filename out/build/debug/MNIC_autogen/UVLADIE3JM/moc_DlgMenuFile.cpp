/****************************************************************************
** Meta object code from reading C++ file 'DlgMenuFile.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/DlgMenuFile.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DlgMenuFile.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSDlgMenuSCOPEModelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDlgMenuSCOPEModelENDCLASS = QtMocHelpers::stringData(
    "DlgMenu::Model"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDlgMenuSCOPEModelENDCLASS_t {
    uint offsetsAndSizes[2];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDlgMenuSCOPEModelENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDlgMenuSCOPEModelENDCLASS_t qt_meta_stringdata_CLASSDlgMenuSCOPEModelENDCLASS = {
    {
        QT_MOC_LITERAL(0, 14)   // "DlgMenu::Model"
    },
    "DlgMenu::Model"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDlgMenuSCOPEModelENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject DlgMenu::Model::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_meta_stringdata_CLASSDlgMenuSCOPEModelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDlgMenuSCOPEModelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDlgMenuSCOPEModelENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Model, std::true_type>
    >,
    nullptr
} };

void DlgMenu::Model::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *DlgMenu::Model::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DlgMenu::Model::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDlgMenuSCOPEModelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int DlgMenu::Model::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDlgMenuSCOPEDelegateENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDlgMenuSCOPEDelegateENDCLASS = QtMocHelpers::stringData(
    "DlgMenu::Delegate"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDlgMenuSCOPEDelegateENDCLASS_t {
    uint offsetsAndSizes[2];
    char stringdata0[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDlgMenuSCOPEDelegateENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDlgMenuSCOPEDelegateENDCLASS_t qt_meta_stringdata_CLASSDlgMenuSCOPEDelegateENDCLASS = {
    {
        QT_MOC_LITERAL(0, 17)   // "DlgMenu::Delegate"
    },
    "DlgMenu::Delegate"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDlgMenuSCOPEDelegateENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject DlgMenu::Delegate::staticMetaObject = { {
    QMetaObject::SuperData::link<QStyledItemDelegate::staticMetaObject>(),
    qt_meta_stringdata_CLASSDlgMenuSCOPEDelegateENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDlgMenuSCOPEDelegateENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDlgMenuSCOPEDelegateENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Delegate, std::true_type>
    >,
    nullptr
} };

void DlgMenu::Delegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *DlgMenu::Delegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DlgMenu::Delegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDlgMenuSCOPEDelegateENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QStyledItemDelegate::qt_metacast(_clname);
}

int DlgMenu::Delegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDlgMenuSCOPEListViewENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDlgMenuSCOPEListViewENDCLASS = QtMocHelpers::stringData(
    "DlgMenu::ListView",
    "DeleteItem",
    "",
    "Path"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDlgMenuSCOPEListViewENDCLASS_t {
    uint offsetsAndSizes[8];
    char stringdata0[18];
    char stringdata1[11];
    char stringdata2[1];
    char stringdata3[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDlgMenuSCOPEListViewENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDlgMenuSCOPEListViewENDCLASS_t qt_meta_stringdata_CLASSDlgMenuSCOPEListViewENDCLASS = {
    {
        QT_MOC_LITERAL(0, 17),  // "DlgMenu::ListView"
        QT_MOC_LITERAL(18, 10),  // "DeleteItem"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 4)   // "Path"
    },
    "DlgMenu::ListView",
    "DeleteItem",
    "",
    "Path"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDlgMenuSCOPEListViewENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x06,    1 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

Q_CONSTINIT const QMetaObject DlgMenu::ListView::staticMetaObject = { {
    QMetaObject::SuperData::link<QListView::staticMetaObject>(),
    qt_meta_stringdata_CLASSDlgMenuSCOPEListViewENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDlgMenuSCOPEListViewENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDlgMenuSCOPEListViewENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ListView, std::true_type>,
        // method 'DeleteItem'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>
    >,
    nullptr
} };

void DlgMenu::ListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ListView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->DeleteItem((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ListView::*)(QString );
            if (_t _q_method = &ListView::DeleteItem; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *DlgMenu::ListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DlgMenu::ListView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDlgMenuSCOPEListViewENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QListView::qt_metacast(_clname);
}

int DlgMenu::ListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void DlgMenu::ListView::DeleteItem(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDlgMenuSCOPEDlgMuENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDlgMenuSCOPEDlgMuENDCLASS = QtMocHelpers::stringData(
    "DlgMenu::DlgMu"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDlgMenuSCOPEDlgMuENDCLASS_t {
    uint offsetsAndSizes[2];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDlgMenuSCOPEDlgMuENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDlgMenuSCOPEDlgMuENDCLASS_t qt_meta_stringdata_CLASSDlgMenuSCOPEDlgMuENDCLASS = {
    {
        QT_MOC_LITERAL(0, 14)   // "DlgMenu::DlgMu"
    },
    "DlgMenu::DlgMu"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDlgMenuSCOPEDlgMuENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject DlgMenu::DlgMu::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSDlgMenuSCOPEDlgMuENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDlgMenuSCOPEDlgMuENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDlgMenuSCOPEDlgMuENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DlgMu, std::true_type>
    >,
    nullptr
} };

void DlgMenu::DlgMu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *DlgMenu::DlgMu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DlgMenu::DlgMu::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDlgMenuSCOPEDlgMuENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DlgMenu::DlgMu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDlgMenuARGSCOPEModelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDlgMenuARGSCOPEModelENDCLASS = QtMocHelpers::stringData(
    "DlgMenuARG::Model"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDlgMenuARGSCOPEModelENDCLASS_t {
    uint offsetsAndSizes[2];
    char stringdata0[18];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDlgMenuARGSCOPEModelENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDlgMenuARGSCOPEModelENDCLASS_t qt_meta_stringdata_CLASSDlgMenuARGSCOPEModelENDCLASS = {
    {
        QT_MOC_LITERAL(0, 17)   // "DlgMenuARG::Model"
    },
    "DlgMenuARG::Model"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDlgMenuARGSCOPEModelENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject DlgMenuARG::Model::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_meta_stringdata_CLASSDlgMenuARGSCOPEModelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDlgMenuARGSCOPEModelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDlgMenuARGSCOPEModelENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Model, std::true_type>
    >,
    nullptr
} };

void DlgMenuARG::Model::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *DlgMenuARG::Model::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DlgMenuARG::Model::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDlgMenuARGSCOPEModelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int DlgMenuARG::Model::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDlgMenuARGSCOPEDelegateENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDlgMenuARGSCOPEDelegateENDCLASS = QtMocHelpers::stringData(
    "DlgMenuARG::Delegate"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDlgMenuARGSCOPEDelegateENDCLASS_t {
    uint offsetsAndSizes[2];
    char stringdata0[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDlgMenuARGSCOPEDelegateENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDlgMenuARGSCOPEDelegateENDCLASS_t qt_meta_stringdata_CLASSDlgMenuARGSCOPEDelegateENDCLASS = {
    {
        QT_MOC_LITERAL(0, 20)   // "DlgMenuARG::Delegate"
    },
    "DlgMenuARG::Delegate"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDlgMenuARGSCOPEDelegateENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject DlgMenuARG::Delegate::staticMetaObject = { {
    QMetaObject::SuperData::link<QStyledItemDelegate::staticMetaObject>(),
    qt_meta_stringdata_CLASSDlgMenuARGSCOPEDelegateENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDlgMenuARGSCOPEDelegateENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDlgMenuARGSCOPEDelegateENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Delegate, std::true_type>
    >,
    nullptr
} };

void DlgMenuARG::Delegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *DlgMenuARG::Delegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DlgMenuARG::Delegate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDlgMenuARGSCOPEDelegateENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QStyledItemDelegate::qt_metacast(_clname);
}

int DlgMenuARG::Delegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QStyledItemDelegate::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDlgMenuARGSCOPEListViewENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDlgMenuARGSCOPEListViewENDCLASS = QtMocHelpers::stringData(
    "DlgMenuARG::ListView",
    "UpdateItemVolume",
    "",
    "DeleteItem",
    "Item",
    "ItemShiftUp",
    "ItemShiftDN"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDlgMenuARGSCOPEListViewENDCLASS_t {
    uint offsetsAndSizes[14];
    char stringdata0[21];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[5];
    char stringdata5[12];
    char stringdata6[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDlgMenuARGSCOPEListViewENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDlgMenuARGSCOPEListViewENDCLASS_t qt_meta_stringdata_CLASSDlgMenuARGSCOPEListViewENDCLASS = {
    {
        QT_MOC_LITERAL(0, 20),  // "DlgMenuARG::ListView"
        QT_MOC_LITERAL(21, 16),  // "UpdateItemVolume"
        QT_MOC_LITERAL(38, 0),  // ""
        QT_MOC_LITERAL(39, 10),  // "DeleteItem"
        QT_MOC_LITERAL(50, 4),  // "Item"
        QT_MOC_LITERAL(55, 11),  // "ItemShiftUp"
        QT_MOC_LITERAL(67, 11)   // "ItemShiftDN"
    },
    "DlgMenuARG::ListView",
    "UpdateItemVolume",
    "",
    "DeleteItem",
    "Item",
    "ItemShiftUp",
    "ItemShiftDN"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDlgMenuARGSCOPEListViewENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   38,    2, 0x06,    1 /* Public */,
       3,    1,   41,    2, 0x06,    3 /* Public */,
       5,    1,   44,    2, 0x06,    5 /* Public */,
       6,    1,   47,    2, 0x06,    7 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject DlgMenuARG::ListView::staticMetaObject = { {
    QMetaObject::SuperData::link<QListView::staticMetaObject>(),
    qt_meta_stringdata_CLASSDlgMenuARGSCOPEListViewENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDlgMenuARGSCOPEListViewENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDlgMenuARGSCOPEListViewENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ListView, std::true_type>,
        // method 'UpdateItemVolume'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'DeleteItem'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Item, std::false_type>,
        // method 'ItemShiftUp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'ItemShiftDN'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void DlgMenuARG::ListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ListView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->UpdateItemVolume((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->DeleteItem((*reinterpret_cast< std::add_pointer_t<Item>>(_a[1]))); break;
        case 2: _t->ItemShiftUp((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->ItemShiftDN((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Item >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ListView::*)(int );
            if (_t _q_method = &ListView::UpdateItemVolume; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ListView::*)(Item );
            if (_t _q_method = &ListView::DeleteItem; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ListView::*)(int );
            if (_t _q_method = &ListView::ItemShiftUp; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ListView::*)(int );
            if (_t _q_method = &ListView::ItemShiftDN; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *DlgMenuARG::ListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DlgMenuARG::ListView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDlgMenuARGSCOPEListViewENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QListView::qt_metacast(_clname);
}

int DlgMenuARG::ListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void DlgMenuARG::ListView::UpdateItemVolume(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DlgMenuARG::ListView::DeleteItem(Item _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DlgMenuARG::ListView::ItemShiftUp(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DlgMenuARG::ListView::ItemShiftDN(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDlgMenuARGSCOPEDlgARGENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDlgMenuARGSCOPEDlgARGENDCLASS = QtMocHelpers::stringData(
    "DlgMenuARG::DlgARG",
    "AddArg",
    "",
    "UpdateItemVolume",
    "DelArg",
    "Item",
    "ShiftUp",
    "ShiftDown",
    "Recived",
    "Rejected"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSDlgMenuARGSCOPEDlgARGENDCLASS_t {
    uint offsetsAndSizes[20];
    char stringdata0[19];
    char stringdata1[7];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[7];
    char stringdata5[5];
    char stringdata6[8];
    char stringdata7[10];
    char stringdata8[8];
    char stringdata9[9];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSDlgMenuARGSCOPEDlgARGENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSDlgMenuARGSCOPEDlgARGENDCLASS_t qt_meta_stringdata_CLASSDlgMenuARGSCOPEDlgARGENDCLASS = {
    {
        QT_MOC_LITERAL(0, 18),  // "DlgMenuARG::DlgARG"
        QT_MOC_LITERAL(19, 6),  // "AddArg"
        QT_MOC_LITERAL(26, 0),  // ""
        QT_MOC_LITERAL(27, 16),  // "UpdateItemVolume"
        QT_MOC_LITERAL(44, 6),  // "DelArg"
        QT_MOC_LITERAL(51, 4),  // "Item"
        QT_MOC_LITERAL(56, 7),  // "ShiftUp"
        QT_MOC_LITERAL(64, 9),  // "ShiftDown"
        QT_MOC_LITERAL(74, 7),  // "Recived"
        QT_MOC_LITERAL(82, 8)   // "Rejected"
    },
    "DlgMenuARG::DlgARG",
    "AddArg",
    "",
    "UpdateItemVolume",
    "DelArg",
    "Item",
    "ShiftUp",
    "ShiftDown",
    "Recived",
    "Rejected"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDlgMenuARGSCOPEDlgARGENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x0a,    1 /* Public */,
       3,    1,   57,    2, 0x0a,    2 /* Public */,
       4,    1,   60,    2, 0x0a,    4 /* Public */,
       6,    1,   63,    2, 0x0a,    6 /* Public */,
       7,    1,   66,    2, 0x0a,    8 /* Public */,
       8,    0,   69,    2, 0x0a,   10 /* Public */,
       9,    0,   70,    2, 0x0a,   11 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject DlgMenuARG::DlgARG::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSDlgMenuARGSCOPEDlgARGENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDlgMenuARGSCOPEDlgARGENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDlgMenuARGSCOPEDlgARGENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DlgARG, std::true_type>,
        // method 'AddArg'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'UpdateItemVolume'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'DelArg'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Item, std::false_type>,
        // method 'ShiftUp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'ShiftDown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'Recived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Rejected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void DlgMenuARG::DlgARG::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DlgARG *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->AddArg(); break;
        case 1: _t->UpdateItemVolume((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->DelArg((*reinterpret_cast< std::add_pointer_t<Item>>(_a[1]))); break;
        case 3: _t->ShiftUp((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->ShiftDown((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->Recived(); break;
        case 6: _t->Rejected(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Item >(); break;
            }
            break;
        }
    }
}

const QMetaObject *DlgMenuARG::DlgARG::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DlgMenuARG::DlgARG::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDlgMenuARGSCOPEDlgARGENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DlgMenuARG::DlgARG::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
