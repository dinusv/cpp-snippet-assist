/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of C++ Snippet Assist application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/


import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Rectangle {
    width: 600
    height: 600
    color : "#262626"

    Rectangle{
        id : consoleWrap
        anchors.top  : parent.top
        anchors.left : parent.left
        anchors.topMargin: 13
        anchors.leftMargin: 80
        width : parent.width - 70
        height : 26
        color : "#262626"

        TextInput{
            id : consoleInput
            anchors.fill: parent
            anchors.margins: 5
            focus : true
            font.pixelSize: 16
            font.family : "Courier New"
            font.weight: Font.Normal
            color : "#fff"
            selectByMouse : true
            selectionColor : "#886631"
            text : command
            cursorDelegate: Rectangle{
                width : 9
                color : "#585858"
                opacity : 0.75
            }
            Keys.onReturnPressed: {
                if (commandInterpreter.execute(text)){
                    text = ""
                }
            }

        }
    }
    Image{
        source : "logo.png"
        anchors.top : parent.top
        anchors.topMargin: 14
        anchors.left: parent.left
        anchors.leftMargin: 8
    }

    Text{
        text: ">"
        font.family: "Arial"
        font.pixelSize: 26
        font.weight: Font.Black
        anchors.top : parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 60
        color : "#474545"
    }
    Rectangle{
        id : consoleBorder
        anchors.top :parent.top
        anchors.topMargin: 43
        width : parent.width
        height : 8
        color : "#3a3a3a"
    }

    Rectangle{
        anchors.top : consoleBorder.bottom
        anchors.topMargin: 0
        width : parent.width
        anchors.left: parent.left
        height : parent.height - consoleWrap.height - 23
        clip : true

        Rectangle{
            anchors.left: parent.left
            anchors.top : parent.top
            anchors.topMargin: 0
            width : 28
            height : parent.height
            color : "#d7d7d7"
        }

        Component {
            id: contactDelegate
            Item {
                Rectangle{
                    height : parent.height
                    width : 28
                    color : syntaxTreeModel.selected === model.index ? "#aaa" : "#ccc"
                }
                Text{
                    text : model.line
                    anchors.top : parent.top
                    anchors.topMargin: 3
                    horizontalAlignment: Text.AlignRight
                    width: 22
                    color : "#555"
                }

                width: parent.width;
                height: 20
                Rectangle{
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 28
                    width : parent.width - 28
                    height : parent.height
                    color : "#eeeeee"
                }

                Text {
                    anchors.left : parent.left
                    anchors.leftMargin: 35 + 15 * model.indent
                    anchors.top: parent.top
                    anchors.topMargin: 3
                    textFormat: Text.RichText;
                    font.pixelSize: 13
                    font.family: "Courier New"
                    color : "#000"
                    text: {
                        if ( syntaxTreeModel.selected === model.index )
                            return '<b><span style="color : #0000cc">' + model.type + ':</span> ' + model.identifier + '</b>';
                        else
                            return '<span style="color : #0000cc">' + model.type + ':</span> ' + model.identifier;
                    }
                }

            }
        }
        ScrollView{
            anchors.fill: parent

            ListView {
                    id : syntaxTreeList
                    anchors.fill: parent
                    anchors.rightMargin: 2
                    anchors.bottomMargin: 5
                    anchors.topMargin: 0
                    boundsBehavior : Flickable.StopAtBounds
                    model: syntaxTreeModel
                    delegate: contactDelegate
                    highlight: Rectangle {
                        color: "#ccc";
                    }
                }
        }
    }
}
