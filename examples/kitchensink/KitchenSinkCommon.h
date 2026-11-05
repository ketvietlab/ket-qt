// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QLabel>
#include <QString>

class QQuickWidget;
class QVBoxLayout;

namespace ket::kitchensink {

// A label whose colour and size are design-system tokens and follow ket::Appearance.
class TokenLabel final : public QLabel {
    Q_OBJECT
  public:
    TokenLabel(const QString& text, QString colorToken, QString sizeToken, int weight = 400,
               QWidget* parent = nullptr, bool wrap = true);

  private:
    void refresh();

    QString colorToken_;
    QString sizeToken_;
    int weight_;
};

// A scrollable page with the design-system page title and description.
struct PageFrame {
    QWidget* page = nullptr;
    QVBoxLayout* body = nullptr;
};
PageFrame makePage(const QString& title, const QString& description);

// A section heading inside a page.
QWidget* sectionTitle(const QString& text, QWidget* parent);

// A Qt Quick view of `<type>.qml` in the KetKitchenSink module that sizes to its content.
QQuickWidget* quickView(const QString& type, QWidget* parent);

} // namespace ket::kitchensink
