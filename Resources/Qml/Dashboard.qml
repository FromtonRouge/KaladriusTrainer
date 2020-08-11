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

    Column {
        id: column
        x: 143
        y: 88
        width: 247
        height: 148
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            id: wpm
            font.pixelSize: 35
            color: "blue"
            text: "240 WPM"
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignLeft
            Layout.leftMargin: 40
            Layout.fillWidth: true
            font.bold: true
        }

        Text {
            id: spm
            text: "240 SPM"
            color: "lightBlue"
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignLeft
            Layout.leftMargin: 40
            Layout.fillWidth: true
            font.bold: true
            font.pixelSize: 35
        }

        Text {
            id: accuracy
            font.pixelSize: 35
            color: "violet"
            text: "100 %"
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignLeft
            Layout.leftMargin: 40
            Layout.fillWidth: true
            font.bold: true
        }


    }





}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:2;anchors_height:60;anchors_x:204;anchors_y:124}
}
##^##*/
