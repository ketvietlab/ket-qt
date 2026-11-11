// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QWidget>

namespace ket::detail {

// A window container (a QQuickView, a web view) is a native child window: it sits above
// everything drawn into its window's own surface, so an in-window overlay drawn there is
// hidden beneath it and its clicks reach the container instead.
inline bool hasNativeChildren(const QWidget* window, const QWidget* overlay) {
    const auto children = window->findChildren<QWidget*>();
    for (const QWidget* child : children)
        if (child != overlay && !overlay->isAncestorOf(child) && child->isVisible() &&
            (child->inherits("QWindowContainer") || child->internalWinId() != 0))
            return true;
    return false;
}

// Makes the overlay a native child window too, so raise() puts it above those children.
// Its translucent pixels then show the window's own surface rather than the children.
inline void makeNativeOverlay(QWidget* overlay) {
    if (overlay->testAttribute(Qt::WA_NativeWindow))
        return;
    overlay->setAttribute(Qt::WA_DontCreateNativeAncestors);
    overlay->setAttribute(Qt::WA_TranslucentBackground);
    // Qt otherwise makes the overlay's siblings native too. A sibling that holds a window
    // container (the page around an Agent Workspace) then gets its own native window above the
    // container's, and the container disappears beneath it. Keep them as they are.
    const bool keepSiblings =
        QCoreApplication::testAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
    QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
    overlay->setAttribute(Qt::WA_NativeWindow);
    QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, keepSiblings);
}

// What the window shows now, native children included, for an overlay to draw beneath its
// translucent parts. Null where the platform cannot grab a window (offscreen).
inline QPixmap windowSnapshot(QWidget* window) {
    QScreen* screen = window->screen();
    if (screen == nullptr || QGuiApplication::platformName() == QStringLiteral("offscreen"))
        return {};
    return screen->grabWindow(window->winId());
}

} // namespace ket::detail
