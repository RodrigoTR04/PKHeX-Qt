/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../host/src/MainWindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onMenuOpen",
        "",
        "onMenuExportSav",
        "onMenuSavePkm",
        "onMenuExportBak",
        "onMenuExit",
        "onBoxSelected",
        "index",
        "onBoxLeft",
        "onBoxRight",
        "onLegalityClicked",
        "onPkmFieldEdited",
        "onShowdownImport",
        "onShowdownExportPkm",
        "onShowdownExportParty",
        "onShowdownExportBox",
        "onMenuAbout",
        "onOpenBatch",
        "onDumpBoxes",
        "onDumpBox",
        "onOpenPkmDatabase",
        "onOpenEncounterDatabase",
        "onOpenWondercards",
        "onOpenInventory",
        "onOpenPokedex",
        "onOpenSaveBlock",
        "onOpenEventFlags",
        "openAccessoryPage",
        "page",
        "clickQr",
        "importQrFromClipboard",
        "exportQrWindow"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onMenuOpen'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMenuExportSav'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMenuSavePkm'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMenuExportBak'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMenuExit'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBoxSelected'
        QtMocHelpers::SlotData<void(int)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Slot 'onBoxLeft'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBoxRight'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLegalityClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPkmFieldEdited'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onShowdownImport'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onShowdownExportPkm'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onShowdownExportParty'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onShowdownExportBox'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMenuAbout'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenBatch'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDumpBoxes'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDumpBox'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenPkmDatabase'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenEncounterDatabase'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenWondercards'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenInventory'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenPokedex'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenSaveBlock'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenEventFlags'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'openAccessoryPage'
        QtMocHelpers::SlotData<void(const QString &)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 29 },
        }}),
        // Slot 'clickQr'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'importQrFromClipboard'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'exportQrWindow'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onMenuOpen(); break;
        case 1: _t->onMenuExportSav(); break;
        case 2: _t->onMenuSavePkm(); break;
        case 3: _t->onMenuExportBak(); break;
        case 4: _t->onMenuExit(); break;
        case 5: _t->onBoxSelected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onBoxLeft(); break;
        case 7: _t->onBoxRight(); break;
        case 8: _t->onLegalityClicked(); break;
        case 9: _t->onPkmFieldEdited(); break;
        case 10: _t->onShowdownImport(); break;
        case 11: _t->onShowdownExportPkm(); break;
        case 12: _t->onShowdownExportParty(); break;
        case 13: _t->onShowdownExportBox(); break;
        case 14: _t->onMenuAbout(); break;
        case 15: _t->onOpenBatch(); break;
        case 16: _t->onDumpBoxes(); break;
        case 17: _t->onDumpBox(); break;
        case 18: _t->onOpenPkmDatabase(); break;
        case 19: _t->onOpenEncounterDatabase(); break;
        case 20: _t->onOpenWondercards(); break;
        case 21: _t->onOpenInventory(); break;
        case 22: _t->onOpenPokedex(); break;
        case 23: _t->onOpenSaveBlock(); break;
        case 24: _t->onOpenEventFlags(); break;
        case 25: _t->openAccessoryPage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 26: _t->clickQr(); break;
        case 27: _t->importQrFromClipboard(); break;
        case 28: _t->exportQrWindow(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 29;
    }
    return _id;
}
QT_WARNING_POP
