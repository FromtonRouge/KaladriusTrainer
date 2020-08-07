import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle {
    id: main

    Text {
        id: time
        font.pointSize: 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        function set() {
            if (countdownTimer.done)
            {
                main.color = "red";
            }
            else
            {
                main.color = "lightGreen";
            }
            time.text = countdownTimer.remainingTimeString
        }
    }

    Timer {
        id: textTimer
        interval: 1
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: time.set()
    }
}
