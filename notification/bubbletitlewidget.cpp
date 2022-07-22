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

#include "bubbletitlewidget.h"
#include "notification/bubbletool.h"
#include "notification/iconbutton.h"
#include "notifylistview.h"
#include "style.h"
#include "helper.hpp"

#include <QKeyEvent>
#include <QBoxLayout>
#include <QScroller>

#include <DFontSizeManager>
#include <DSysInfo>
#include <QMenu>

BubbleTitleWidget::BubbleTitleWidget(NotifyModel *model, EntityPtr entity, QWidget *parent)
    : BubbleBase(parent, entity)
    , m_titleLabel(nullptr)
{
    setParentModel(model);

    this->setObjectName(entity->appName());
    m_titleLabel = new DLabel;
    m_titleLabel->setForegroundRole(QPalette::BrightText);
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QFont font = m_titleLabel->font();
    font.setBold(true);
    font.setWeight(QFont::DemiBold);
    m_titleLabel->setFont(font);
    m_titleLabel->setText(BubbleTool::getDeepinAppName(entity->appName()));
    DFontSizeManager::instance()->bind(m_titleLabel, DFontSizeManager::T4);

    m_toggleNotificationFolding = new CicleIconButton(nullptr);
    m_toggleNotificationFolding->setObjectName(m_appName + "-ToggleNotificationFolding");
    m_toggleNotificationFolding->setFixedSize(UI::Bubble::buttonSize);
    m_toggleNotificationFolding->setIconSize(UI::Bubble::collapseIconSize);
    connect(m_toggleNotificationFolding, &DIconButton::clicked, this, &BubbleTitleWidget::toggleNotificationFolding);

    m_settingBtn->setFixedSize(UI::Bubble::buttonSize);
    m_settingBtn->setIconSize(UI::Bubble::collapseIconSize);

    m_closeButton->setFixedSize(UI::Bubble::buttonSize);
    m_closeButton->setIconSize(UI::Bubble::clearIconSize);

    QHBoxLayout *head_Layout = new QHBoxLayout;
    head_Layout->setContentsMargins(10, 0, 0, 0);
    head_Layout->addWidget(m_titleLabel, Qt::AlignLeft);
    head_Layout->addStretch();
    head_Layout->addWidget(m_toggleNotificationFolding, 0, Qt::AlignRight);
    head_Layout->addWidget(m_settingBtn, 0, Qt::AlignRight);
    head_Layout->addWidget(m_closeButton, 0, Qt::AlignRight);
    setLayout(head_Layout);

    connect(m_closeButton, &CicleIconButton::clicked, this, [ = ] {
        m_model->removeAppGroup(entity->appName());
    });

    updateNotificationFoldingStatus();
    updateTabOrder();
}

void BubbleTitleWidget::setIndexRow(int row)
{
    m_indexRow = row;
}

QList<QPointer<QWidget> > BubbleTitleWidget::bubbleElements() const
{
    QList<QPointer<QWidget>> bubble_elements{BubbleBase::bubbleElements()};
    bubble_elements.prepend(m_toggleNotificationFolding);
    return bubble_elements;
}

int BubbleTitleWidget::bubbleTitleWidgetHeight()
{
    return qMax(QFontMetrics(DFontSizeManager::instance()->t4()).height(), BubbleTitleHeight);
}

void BubbleTitleWidget::updateNotificationFoldingStatus()
{
    const ListItemPtr item = m_model->getAppData(m_appName);
    const bool isCollapse = item->isCollapse();

    m_toggleNotificationFolding->setIcon(isCollapse ? DDciIcon::fromTheme("arrow_ordinary_down")
                                                    : DDciIcon::fromTheme("arrow_ordinary_up"));
}

void BubbleTitleWidget::toggleNotificationFolding()
{
    const ListItemPtr item = m_model->getAppData(m_appName);
    const bool isCollapse = item->isCollapse();

    if (isCollapse) {
        m_model->expandDataByAppName(m_appName);
    } else {
        m_model->collapseDataByAppName(m_appName);
    }
    updateNotificationFoldingStatus();
}
