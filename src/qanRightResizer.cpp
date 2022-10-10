/*
 Copyright (c) 2008-2022, Benoit AUTHEMAN All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author or Destrat.io nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//-----------------------------------------------------------------------------
// This file is a part of the QuickQanava software library.
//
// \file	qanRightResizer.cpp
// \author	benoit@destrat.io
// \date	2022 10 09
//-----------------------------------------------------------------------------

// Qt headers
#include <QCursor>
#include <QMouseEvent>

// QuickQanava headers
#include "./qanRightResizer.h"
#include "./qanGroupItem.h"

namespace qan {  // ::qan

/* RightResizer Object Management *///-----------------------------------------
RightResizer::RightResizer(QQuickItem* parent) :
    QQuickItem{parent}
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setAcceptHoverEvents(true);
}
//-----------------------------------------------------------------------------

/* Resizer Management *///-----------------------------------------------------
void    RightResizer::setTarget(QQuickItem* target)
{
    if (_target != target) {
        if (_target)
            disconnect(_target.data(),  nullptr,
                       this,            nullptr);  // Disconnect old target width/height monitoring
        _target = target;
        emit targetChanged();
    }
    if (_target) {
        connect(_target,    &QQuickItem::xChanged,
                this,       &RightResizer::onTargetXChanged);
        connect(_target,    &QQuickItem::yChanged,
                this,       &RightResizer::onTargetYChanged);

        connect(_target,    &QQuickItem::widthChanged,
                this,       &RightResizer::onTargetWidthChanged);
        connect(_target,    &QQuickItem::heightChanged,
                this,       &RightResizer::onTargetHeightChanged);
        connect(_target,    &QQuickItem::parentChanged, // When a selected group is dragged in another
                this,       &RightResizer::onUpdate);   // parent group, resize area has to be updated
        onUpdate();
    }
    setVisible(_target != nullptr);
}

void    RightResizer::onTargetXChanged()
{
    if (_target && parentItem() != nullptr) {
        const auto sp = _target->mapToItem(parentItem(), QPointF{0, 0});
        setX(sp.x() + _target->width() - 2.5);
    }
}

void    RightResizer::onTargetYChanged()
{
    if (_target && parentItem() != nullptr) {
        const auto sp = _target->mapToItem(parentItem(), QPointF{0, 0});
        setY(sp.y());
    }
}

void    RightResizer::onTargetWidthChanged()
{
    if (_target && parentItem() != nullptr) {
        const auto sp = _target->mapToItem(parentItem(), QPointF{0, 0});
        setX(sp.x() + _target->width() - 2.5);
        setWidth(5);
    }
}

void    RightResizer::onTargetHeightChanged()
{
    if (_target)
        setHeight(_target->height() - 4);   // Account for a potential bottom right resizer
}

void    RightResizer::onUpdate()
{
    onTargetXChanged();
    onTargetYChanged();
    onTargetWidthChanged();
    onTargetHeightChanged();
}

void    RightResizer::setMinimumTargetSize(QSizeF minimumTargetSize)
{
    if (minimumTargetSize != _minimumTargetSize) {
        _minimumTargetSize = minimumTargetSize;
        emit minimumTargetSizeChanged();
    }
}

void    RightResizer::setPreserveRatio(bool preserveRatio) noexcept
{
    if (preserveRatio != _preserveRatio) {
        _preserveRatio = preserveRatio;
        emit preserveRatioChanged();
    }
}

void    RightResizer::setRatio(qreal ratio) noexcept
{
    if (!qFuzzyCompare(2.0 + _ratio, 2.0 + ratio)) { // Using 2.0 because -1.0 is a valid input (disable ratio...)
        _ratio = ratio;
        emit ratioChanged();
    }
}
//-----------------------------------------------------------------------------

/* Resizer Management *///-----------------------------------------------------
void    RightResizer::hoverEnterEvent(QHoverEvent *event)
{
    if (isVisible()) {
        setCursor(Qt::SplitHCursor);
        event->setAccepted(true);
    }
}
void    RightResizer::hoverLeaveEvent(QHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);
    event->setAccepted(true);
}
void    RightResizer::mouseMoveEvent(QMouseEvent* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    const auto mePos = event->windowPos();
#else
    const auto mePos = event->scenePosition();
#endif
    if (event->buttons() |  Qt::LeftButton &&
            !_dragInitialPos.isNull() &&
            !_targetInitialSize.isEmpty()) {
        const auto groupTarget = qobject_cast<qan::GroupItem*>(_target.data());
        const auto target = groupTarget != nullptr ? groupTarget->getContainer() :
                                                     _target.data();
        QPointF startLocalPos;
        QPointF curLocalPos;
        if (parentItem() != nullptr) {
            startLocalPos = parentItem()->mapFromScene(_dragInitialPos);
            curLocalPos = parentItem()->mapFromScene(mePos);
        } else
            qWarning() << "qan::RightResizer::mouseMoveEvent(): Internal error, no parent item.";
        const QPointF delta{curLocalPos - startLocalPos};
        if (target) {
            // Do not resize below minimumSize
            const qreal targetWidth = _targetInitialSize.width() + delta.x();
            if (targetWidth > _minimumTargetSize.width()) {
                target->setWidth(targetWidth);
                if (_preserveRatio) {
                    const qreal targetHeight = targetWidth * getRatio();
                    if (targetHeight > _minimumTargetSize.height())
                        target->setHeight(targetHeight);
                }
            }
            event->setAccepted(true);
        }
    }
}
void    RightResizer::mousePressEvent(QMouseEvent* event)
{
    if (!isVisible())
        return;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    const auto mePos = event->windowPos();
#else
    const auto mePos = event->scenePosition();
#endif
    const auto groupTarget = qobject_cast<qan::GroupItem*>(_target.data());
    const auto target = groupTarget != nullptr ? groupTarget->getContainer() :
                                                 _target.data();
    if (target) {
        _dragInitialPos = mePos;
        _targetInitialSize = {target->width(), target->height()};
        emit resizeStart(_target ? QSizeF{_target->width(), _target->height()} :  // Use of target ok.
                                  QSizeF{});
        event->setAccepted(true);
    }
}
void    RightResizer::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event)
    _dragInitialPos = {0., 0.};       // Invalid all cached coordinates when button is released
    _targetInitialSize = {0., 0.};
    if (_target)
        emit resizeEnd(_target ? QSizeF{_target->width(), _target->height()} :  // Use of _target ok.
                                 QSizeF{});
}
//-------------------------------------------------------------------------

} // ::qan
