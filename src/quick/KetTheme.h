// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QColor>
#include <QObject>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

class QJSEngine;
class QQmlEngine;

namespace ket {

// QML singleton `KetTheme` in the `Ket` module. It mirrors ket::Appearance and exposes
// the token map `t` and the shared component recipes, so QML and Widgets agree.
class QuickTheme final : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(KetTheme)
    QML_SINGLETON
    // QML spells values with their enum, e.g. KetTheme.BadgeTone.Danger.
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
    Q_PROPERTY(bool dark READ dark NOTIFY themeChanged)
    Q_PROPERTY(qreal zoom READ zoom NOTIFY zoomChanged)
    Q_PROPERTY(QVariantMap t READ tokens NOTIFY themeChanged)
    Q_PROPERTY(QString fontFamily READ fontFamily CONSTANT)
    Q_PROPERTY(QString monoFontFamily READ monoFontFamily CONSTANT)
    Q_PROPERTY(QString source READ source CONSTANT)
  public:
    // Same order as ket::ButtonVariant, ket::ControlSize and ket::BadgeTone.
    enum class ButtonVariant { Primary, Secondary, Ghost, Danger, GhostDanger };
    Q_ENUM(ButtonVariant)
    enum class ControlSize { Small, Medium };
    Q_ENUM(ControlSize)
    enum class BadgeTone { Neutral, Accent, Info, Success, Warning, Danger };
    Q_ENUM(BadgeTone)

    explicit QuickTheme(QObject* parent = nullptr);
    static QuickTheme* create(QQmlEngine*, QJSEngine*);

    [[nodiscard]] bool dark() const;
    [[nodiscard]] qreal zoom() const;
    [[nodiscard]] QVariantMap tokens() const { return tokens_; }
    [[nodiscard]] QString fontFamily() const;
    [[nodiscard]] QString monoFontFamily() const;
    [[nodiscard]] QString source() const;

    // Pass KetTheme.dark and KetTheme.zoom so a binding re-evaluates when either changes.
    // Lengths in the returned recipes are already scaled to the zoom.
    Q_INVOKABLE QVariantMap buttonRecipe(bool dark, qreal zoom, int variant, int size, bool hovered,
                                         bool pressed, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap iconButtonRecipe(bool dark, qreal zoom, int size, bool hovered,
                                             bool pressed, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap badgeRecipe(bool dark, qreal zoom, int tone) const;
    Q_INVOKABLE QVariantMap tagRecipe(bool dark, qreal zoom, const QColor& color, bool overflow,
                                      bool hovered) const;
    // A token length at `zoom` in whole pixels: KetTheme.px(KetTheme.t.space4, KetTheme.zoom).
    Q_INVOKABLE qreal px(qreal logicalPixels, qreal zoom) const;

  signals:
    void themeChanged();
    void zoomChanged();

  private:
    QVariantMap tokens_;
};

} // namespace ket
