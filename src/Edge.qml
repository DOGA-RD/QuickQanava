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
// \file	CurvedEdge.qml
// \author	benoit@destrat.io
// \date	2016 09 04
//-----------------------------------------------------------------------------

import QtQuick          2.7
import QtQuick.Controls 2.1
import QtQuick.Shapes   1.0

import QuickQanava        2.0 as Qan
import "qrc:/QuickQanava" as Qan

Qan.EdgeItem {
    id: edgeItem

    // Private hack for visual connector edge color dynamic modification
    property color color: style ? style.lineColor : Qt.rgba(0.,0.,0.,1.)
    Qan.EdgeTemplate {
        anchors.fill: parent
        edgeItem: parent
        color: parent.color
    }
    Pane {
        id: labelPane
        anchors.centerIn: parent
        opacity: 0.80
        padding: 0
        z: 2
        width: label.implicitWidth
        height: label.implicitHeight
        visible: (label.text !== "")

        Label {
            id: label
            z: 3
            text: edgeItem.edge.label
            visible: true
        }
    }
}
