/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     chenwei <chenwei_cm@deepin.com>
 *
 * Maintainer: chenwei <chenwei_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "itemdelegate.h"
#include "bubbletitlewidget.h"
#include "bubbleitem.h"
#include "notification/constants.h"
#include "overlapwidet.h"
#include "notifylistview.h"

#include <QDebug>

ItemDelegate::ItemDelegate(NotifyListView *view, NotifyModel *model, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_model(model)
    , m_view(view)
{
}

QWidget *ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    EntityPtr notify = index.data().value<EntityPtr>();
    if (!notify)
        return nullptr;

    QWidget *createdWidget = nullptr;
    if (m_model->isCollapse(notify->appName())) {
        OverLapWidet *widget = new OverLapWidet(m_model, notify, parent);
        widget->setParentView(m_view);
        createdWidget = widget;
    } else if (notify->isTitle()) {
        BubbleTitleWidget *titleWidget = new BubbleTitleWidget(m_model, notify, parent);
        titleWidget->setParentView(m_view);
        createdWidget = titleWidget;
    } else {
        BubbleItem *bubble = new BubbleItem(parent, notify);
        bubble->setParentModel(m_model);
        bubble->setParentView(m_view);
        createdWidget = bubble;
    }
    if (index.row() == index.model()->rowCount() - 1) {
        m_lastItemView = createdWidget;
        qDebug() << "lastItemCreated()" << m_lastItemView;
        Q_EMIT m_view->lastItemCreated();
    }
    return createdWidget;
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    EntityPtr notify = index.data().value<EntityPtr>();
    if (!notify)
        return QSize();

    QSize bubbleSize(BubbleItemWidth, BubbleItem::bubbleItemHeight() + BubbleSpacing);
    if (m_model->isCollapse(notify->appName())) {
        bubbleSize = bubbleSize + QSize(0, m_model->getAppData(notify->appName())->overlapCount() * 10);
    } else if (notify->isTitle()) {
        bubbleSize = QSize(BubbleTitleWidth, BubbleTitleWidget::bubbleTitleWidgetHeight());
    }

    return bubbleSize;
}

void ItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)

    QRect rect = option.rect;
    QSize size = sizeHint(option, index);
    editor->setGeometry(rect.x(), rect.y(), size.width(), size.height() - BubbleSpacing);
}

bool ItemDelegate::eventFilter(QObject *object, QEvent *event)
{
    // we don't need QStyledItemDelegate's implementation,
    // because we only use editor to replace display, and don't edit any data.
    return QAbstractItemDelegate::eventFilter(object, event);
}

QWidget *ItemDelegate::lastItemView() const
{
    return m_lastItemView;
}
