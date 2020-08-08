import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle {
    id: main
    width: wpm.width + 20
    height: wpm.height + 20
    radius: 10.0
    color: "lightBlue"

    Text {
        id: wpm
        font.pointSize: 20
        anchors.centerIn: parent
        function set() {
            wpm.text = wordCounter.wpm + " WPM"
        }
    }

    Timer {
        id: textTimer
        interval: 1
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: wpm.set()
    }
}
