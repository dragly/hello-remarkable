import QtQuick 2.6
import QtQuick.Window 2.2

import Test 1.0

TabletWindow {
    id: root
    visible: true
    title: qsTr("Hello World")

    width: 1404
    height: 1872

    canvas: tabletCanvas

    Rectangle {
        anchors.fill: parent
        color: "white"
    }

    TabletCanvas {
        id: tabletCanvas
        anchors.fill: parent
    }

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
}
