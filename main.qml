import QtQuick 2.6
import QtQuick.Window 2.2

import Test 1.0

TabletWindow {
    id: root
    visible: true
    title: qsTr("Hello World")

    width: 1404
    height: 1872

    onPenEvent: {
//        rect.x = position.x
//        rect.y = position.y
//        rect.opacity = pressure
    }
//    canvases: repeater.children

    Component.onCompleted: {
        var list = []
        var rows = 24
        var columns = 24
        for (var i = 0; i < rows*columns; i++) {
            var width = 1404 / rows
            var height = 1872 / columns
            var row = Math.floor(i / rows)
            var column = Math.floor(i % columns)
            var properties  = {
                x: width * column,
                y: height * row,
                width: width,
                height: height,
            }
            var canvas = canvasComponent.createObject(root, properties)
            list.push(canvas)
        }
        canvases = list
    }

    Component {
        id: canvasComponent
        TabletCanvas {
            id: tabletCanvas
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "white"
    }

//    BezierCurve {
//        anchors.fill: parent
//    }

    Text {
        anchors.centerIn: parent
        text: "Hello, World!"
    }

    Grid {
        id: grid
        anchors.fill: parent
    }

    Text {
        anchors {
            left: parent.left
            top: parent.top
            margins: 32
        }
    }

//    Rectangle {
//        id: rect
//        color: "grey"
//        width: 200
//        height: 200
//        x: 400
//        y: 400

//        MouseArea {
//            anchors.fill: parent
//            drag.target: parent
//        }
//    }
}
