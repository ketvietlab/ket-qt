// SPDX-License-Identifier: MPL-2.0
#pragma once

#include <QStringList>
#include <QWidget>

class QListWidget;
class QStackedWidget;

namespace ket::kitchensink {

// Page list on the left, the selected page on the right, theme and zoom controls above.
class KitchenSinkWindow final : public QWidget {
    Q_OBJECT
  public:
    explicit KitchenSinkWindow(QWidget* parent = nullptr);

    [[nodiscard]] static QStringList pageIds();
    // Selects a page by id; false when there is no such page.
    bool showPage(const QString& id);

  private:
    QListWidget* nav_ = nullptr;
    QStackedWidget* pages_ = nullptr;
    QStringList ids_;
};

} // namespace ket::kitchensink
