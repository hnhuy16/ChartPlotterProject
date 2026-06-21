/****************************************************************************
** Meta object code from reading C++ file 'ChartBackend.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../ChartPlugin/ChartBackend.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChartBackend.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
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
struct qt_meta_tag_ZN12ChartBackendE_t {};
} // unnamed namespace

template <> constexpr inline auto ChartBackend::qt_create_metaobjectdata<qt_meta_tag_ZN12ChartBackendE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ChartBackend",
        "QML.Element",
        "auto",
        "chartDataChanged",
        "",
        "onlineModeChanged",
        "viewportChanged",
        "appendRealtimePoint",
        "setOfflineMode",
        "setOnlineMode",
        "generateDummyData",
        "clearData",
        "chartData",
        "QList<QPointF>",
        "onlineMode",
        "minX",
        "maxX"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'chartDataChanged'
        QtMocHelpers::SignalData<void()>(3, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'onlineModeChanged'
        QtMocHelpers::SignalData<void()>(5, 4, QMC::AccessPublic, QMetaType::Void),
        // Signal 'viewportChanged'
        QtMocHelpers::SignalData<void()>(6, 4, QMC::AccessPublic, QMetaType::Void),
        // Slot 'appendRealtimePoint'
        QtMocHelpers::SlotData<void()>(7, 4, QMC::AccessPrivate, QMetaType::Void),
        // Method 'setOfflineMode'
        QtMocHelpers::MethodData<void()>(8, 4, QMC::AccessPublic, QMetaType::Void),
        // Method 'setOnlineMode'
        QtMocHelpers::MethodData<void()>(9, 4, QMC::AccessPublic, QMetaType::Void),
        // Method 'generateDummyData'
        QtMocHelpers::MethodData<void()>(10, 4, QMC::AccessPublic, QMetaType::Void),
        // Method 'clearData'
        QtMocHelpers::MethodData<void()>(11, 4, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'chartData'
        QtMocHelpers::PropertyData<QList<QPointF>>(12, 0x80000000 | 13, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'onlineMode'
        QtMocHelpers::PropertyData<bool>(14, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
        // property 'minX'
        QtMocHelpers::PropertyData<double>(15, QMetaType::Double, QMC::DefaultPropertyFlags, 2),
        // property 'maxX'
        QtMocHelpers::PropertyData<double>(16, QMetaType::Double, QMC::DefaultPropertyFlags, 2),
    };
    QtMocHelpers::UintData qt_enums {
    };
    QtMocHelpers::UintData qt_constructors {};
    QtMocHelpers::ClassInfos qt_classinfo({
            {    1,    2 },
    });
    return QtMocHelpers::metaObjectData<ChartBackend, void>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums, qt_constructors, qt_classinfo);
}
Q_CONSTINIT const QMetaObject ChartBackend::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ChartBackendE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ChartBackendE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12ChartBackendE_t>.metaTypes,
    nullptr
} };

void ChartBackend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ChartBackend *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->chartDataChanged(); break;
        case 1: _t->onlineModeChanged(); break;
        case 2: _t->viewportChanged(); break;
        case 3: _t->appendRealtimePoint(); break;
        case 4: _t->setOfflineMode(); break;
        case 5: _t->setOnlineMode(); break;
        case 6: _t->generateDummyData(); break;
        case 7: _t->clearData(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ChartBackend::*)()>(_a, &ChartBackend::chartDataChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChartBackend::*)()>(_a, &ChartBackend::onlineModeChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChartBackend::*)()>(_a, &ChartBackend::viewportChanged, 2))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QList<QPointF>*>(_v) = _t->chartData(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->onlineMode(); break;
        case 2: *reinterpret_cast<double*>(_v) = _t->minX(); break;
        case 3: *reinterpret_cast<double*>(_v) = _t->maxX(); break;
        default: break;
        }
    }
}

const QMetaObject *ChartBackend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChartBackend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ChartBackendE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ChartBackend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ChartBackend::chartDataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ChartBackend::onlineModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ChartBackend::viewportChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
