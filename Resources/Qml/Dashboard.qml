import QtQuick 2.0
import QtQuick.Layouts 1.3

Item {
    id: dashboard

    function update() {
        wpm.text = wordCounter.wpm.toFixed(0) + " WPM"
        spm.text = wordCounter.spm.toFixed(0) + " SPM"
        accuracy.text = wordCounter.accuracy.toFixed(1) + " %"
        if (countdownTimer.done)
        {
            time.color = "red"
        }
        else
        {
            if (countdownTimer.remainingTime < 10000)
            {
                time.color = "orange"
            }
            else
            {
                time.color = "green"
            }
        }
        time.text = countdownTimer.remainingTimeString
    }

    Timer {
        id: timer
        interval: 1
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: dashboard.update()
    }

    Text {
        id: time
        x: 204
        width: 230
        height: 60
        text: "1:00.000"
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 15
        horizontalAlignment: Text.AlignHCenter
        Layout.fillWidth: true
        font.pixelSize: 50
    }

    RowLayout {
        id: rowLayout
        x: 84
        width: 404
        height: 56
        anchors.horizontalCenterOffset: 13
        anchors.horizontalCenter: time.horizontalCenter
        anchors.top: time.bottom
        anchors.topMargin: 6
        spacing: 0

        Text {
            id: wpm
            font.pixelSize: 28
            color: "blue"
            text: "240 WPM"
            font.bold: true
        }

        Text {
            id: spm
            text: "240 SPM"
            color: "lightBlue"
            horizontalAlignment: Text.AlignLeft
            font.bold: true
            font.pixelSize: 28
        }

        Text {
            id: accuracy
            font.pixelSize: 28
            color: "violet"
            text: "100 %"
            horizontalAlignment: Text.AlignLeft
            font.bold: true
        }




    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:2;anchors_height:60;anchors_x:204;anchors_y:124}
D{i:3;anchors_y:113}
}
##^##*/
