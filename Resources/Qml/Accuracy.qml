import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle {
    id: main
    width: accuracy.width + 20
    height: accuracy.height + 20
    radius: 10.0
    color: "violet"

    Text {
        id: accuracy
        font.pointSize: 20
        anchors.centerIn: parent
        function set() {
            accuracy.text = wordCounter.accuracy.toFixed(1) + " %"
        }
    }

    Timer {
        id: textTimer
        interval: 1
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: accuracy.set()
    }
}
