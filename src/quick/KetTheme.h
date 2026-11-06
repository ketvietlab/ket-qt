// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QColor>
#include <QObject>
#include <QQuickTextDocument>
#include <QStringList>
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
    Q_PROPERTY(QStringList iconCodes READ iconCodes CONSTANT)
  public:
    // Same order as ket::ButtonVariant, ket::ControlSize and ket::BadgeTone.
    enum class ButtonVariant { Primary, Secondary, Ghost, Danger, GhostDanger };
    Q_ENUM(ButtonVariant)
    enum class ControlSize { Small, Medium };
    Q_ENUM(ControlSize)
    enum class BadgeTone { Neutral, Accent, Info, Success, Warning, Danger };
    Q_ENUM(BadgeTone)
    enum class IconWeight { Light, Regular };
    Q_ENUM(IconWeight)
    enum class ToggleKind { Checkbox, Radio };
    Q_ENUM(ToggleKind)
    enum class BannerTone { Info, Warning, Danger };
    Q_ENUM(BannerTone)
    enum class ConversationStatus { NotStarted, Working, NeedsInput, Error, Completed, Stopped };
    Q_ENUM(ConversationStatus)
    enum class TaskStatus { Running, Stopping, Completed, Failed, Stopped };
    Q_ENUM(TaskStatus)

    explicit QuickTheme(QObject* parent = nullptr);
    static QuickTheme* create(QQmlEngine*, QJSEngine*);

    [[nodiscard]] bool dark() const;
    [[nodiscard]] qreal zoom() const;
    [[nodiscard]] QVariantMap tokens() const { return tokens_; }
    [[nodiscard]] QString fontFamily() const;
    [[nodiscard]] QString monoFontFamily() const;
    [[nodiscard]] QString source() const;
    [[nodiscard]] QStringList iconCodes() const;

    // Pass KetTheme.dark and KetTheme.zoom so a binding re-evaluates when either changes.
    // Lengths in the returned recipes are already scaled to the zoom.
    Q_INVOKABLE QVariantMap buttonRecipe(bool dark, qreal zoom, int variant, int size, bool hovered,
                                         bool pressed, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap iconButtonRecipe(bool dark, qreal zoom, int size, bool hovered,
                                             bool pressed, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap badgeRecipe(bool dark, qreal zoom, int tone) const;
    Q_INVOKABLE QVariantMap tagRecipe(bool dark, qreal zoom, const QColor& color, bool overflow,
                                      bool hovered) const;
    Q_INVOKABLE QVariantMap fieldRecipe(bool dark, qreal zoom, bool hovered, bool focused,
                                        bool enabled, bool invalid) const;
    Q_INVOKABLE QVariantMap toggleRecipe(bool dark, qreal zoom, int kind, bool checked,
                                         bool hovered, bool pressed, bool enabled,
                                         bool focused) const;
    Q_INVOKABLE QVariantMap choiceRecipe(bool dark, qreal zoom, bool checked, bool hovered,
                                         bool pressed, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap fieldRowRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap bannerRecipe(bool dark, qreal zoom, int tone) const;
    Q_INVOKABLE QVariantMap toastRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap dialogRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap menuRecipe(bool dark, qreal zoom) const;
    // `tone` is a ket::MenuItemTone: 0 normal, 1 danger, 2 warning.
    Q_INVOKABLE QVariantMap menuItemRecipe(bool dark, qreal zoom, bool highlighted, bool enabled,
                                           int tone) const;
    Q_INVOKABLE QVariantMap tooltipRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap navItemRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                          bool pressed, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap kbdRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap segmentedRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap segmentRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                          bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap tabBarRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap tabRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                      bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap progressRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap activityRecipe(bool dark, qreal zoom, const QColor& color) const;
    // [height, opacity] of one activity bar `elapsedMs` into the animation, in the units of
    // `recipe` (an activityRecipe() result, already scaled).
    Q_INVOKABLE QVariantList activityBarFrame(const QVariantMap& recipe, int bar,
                                              qreal elapsedMs) const;
    Q_INVOKABLE QVariantMap emptyStateRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap conversationRowRecipe(bool dark, qreal zoom, bool selected,
                                                  bool hovered, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap conversationStatusRecipe(bool dark, int status) const;
    Q_INVOKABLE QString conversationStatusLabel(int status) const;
    Q_INVOKABLE QVariantMap composerRecipe(bool dark, qreal zoom, bool focused) const;
    // The composer's one action circle: send, or stop the turn while the box is empty.
    Q_INVOKABLE QVariantMap composerActionRecipe(bool dark, qreal zoom, bool busy, bool hasText,
                                                 bool hovered, bool pressed, bool focused) const;
    Q_INVOKABLE QVariantMap userMessageRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap assistantMessageRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap dayDividerRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap activityRowRecipe(bool dark, qreal zoom, bool hovered, bool enabled,
                                              bool focused) const;
    // Gives every block of a TextEdit's document a CSS line box of `pixels` (font size × line
    // height); Qt's proportional height would scale the font's line spacing instead.
    Q_INVOKABLE void applyLineHeight(QQuickTextDocument* document, qreal pixels) const;
    // Syntax colours for a code TextEdit in the fence's language (ket::SyntaxHighlighter,
    // kept on the document); call again when the theme changes.
    Q_INVOKABLE void highlightCode(QQuickTextDocument* document, const QString& language,
                                   bool dark) const;
    // Each line of `code` as rich text with syntax colours, for a Text item per line.
    Q_INVOKABLE QStringList highlightedLines(const QString& code, const QString& language,
                                             bool dark) const;
    Q_INVOKABLE QVariantMap markdownRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap codeBlockRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap mentionPickerRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap mentionOptionRecipe(bool dark, qreal zoom, bool highlighted) const;
    Q_INVOKABLE QVariantMap usageRingRecipe(bool dark, qreal zoom, qreal used, bool hovered) const;
    // [{ kind: "text" | "code", text, language }] in order (ket::splitMarkdown).
    Q_INVOKABLE QVariantList markdownSegments(const QString& markdown) const;
    // Styles a TextEdit holding Markdown, as ket::MarkdownText is styled.
    Q_INVOKABLE void styleMarkdown(QQuickTextDocument* document, bool dark, qreal zoom) const;
    // { start, end, query } for an `@agent/` mention at `cursor`, or null.
    Q_INVOKABLE QVariant agentMention(const QString& text, int cursor) const;
    Q_INVOKABLE QString agentMentionText(const QString& number) const;
    // Két Symbol (ket::Icons): the qrc URL of the master for a `box` (already zoomed), the
    // logical size the drawing occupies and its whole-device-pixel offset in the box.
    Q_INVOKABLE QString iconSource(const QString& code, qreal box, int weight) const;
    Q_INVOKABLE qreal iconDrawSize(qreal box) const;
    Q_INVOKABLE qreal iconOffset(qreal box, qreal devicePixelRatio) const;
    // The Két code for a KetPlus Lucide name, or "".
    Q_INVOKABLE QString iconForLucide(const QString& lucide) const;
    // A token length at `zoom` in whole pixels: KetTheme.px(KetTheme.t.space4, KetTheme.zoom).
    Q_INVOKABLE qreal px(qreal logicalPixels, qreal zoom) const;

    // Chat extras (KetChatRecipes).
    Q_INVOKABLE QVariantMap popoverRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap meterRecipe(bool dark, qreal zoom, bool compact, qreal fraction) const;
    Q_INVOKABLE QVariantMap attachmentChipRecipe(bool dark, qreal zoom, bool removeHovered) const;
    Q_INVOKABLE QVariantMap suggestionRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                             bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap questionCardRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap activityGroupRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                                bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap galleryRecipe(bool dark, qreal zoom, bool hovered) const;

    // Controls and chrome (KetControlRecipes).
    Q_INVOKABLE QVariantMap sliderRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                         bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap avatarRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap treeItemRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                           bool pressed, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap groupHeaderRecipe(bool dark, qreal zoom, bool selected, bool hovered,
                                              bool pressed, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap routeTabRecipe(bool dark, qreal zoom, bool active, bool hovered,
                                           bool pressed, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap statusBarRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap breadcrumbRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap paneHeaderRecipe(bool dark, qreal zoom) const;

    // Surfaces (KetSurfaceRecipes).
    Q_INVOKABLE QVariantMap commandPaletteRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap commandResultRecipe(bool dark, qreal zoom, bool highlighted) const;
    Q_INVOKABLE QVariantMap fileRowRecipe(bool dark, qreal zoom, bool active, bool hovered,
                                          bool pressed, bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap changedFilesRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap diffRecipe(bool dark, qreal zoom) const;
    Q_INVOKABLE QVariantMap taskRowRecipe(bool dark, qreal zoom, int status, bool current) const;
    Q_INVOKABLE QVariantMap taskBarRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                          bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap updateCardRecipe(bool dark, qreal zoom, bool ready) const;
    Q_INVOKABLE QVariantMap updatePillRecipe(bool dark, qreal zoom, bool hovered, bool pressed,
                                             bool enabled, bool focused) const;
    Q_INVOKABLE QVariantMap themeCardRecipe(bool dark, qreal zoom, bool selected, bool previewDark,
                                            bool hovered, bool pressed, bool enabled,
                                            bool focused) const;
    Q_INVOKABLE QVariantMap accentSwatchRecipe(bool dark, qreal zoom) const;

  signals:
    void themeChanged();
    void zoomChanged();

  private:
    QVariantMap tokens_;
};

} // namespace ket
