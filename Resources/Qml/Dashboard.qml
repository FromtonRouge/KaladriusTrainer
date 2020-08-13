import QtQuick 2.0
import QtQuick.Layouts 1.3

Item {
    id: main

    Connections {
        target: dashboard

        function onLastWpmChanged(value) { wpmLast.text = value.toFixed(0) + " WPM" }
        function onLastSpmChanged(value) { spmLast.text = value.toFixed(0) + " SPM"}
        function onLastAccuracyChanged(value) { accuracyLast.text = value.toFixed(1) + " %" }
        function onMaxWpmChanged(value) { wpmMax.text = value.toFixed(0) + " WPM" }
        function onMaxSpmChanged(value) { spmMax.text = value.toFixed(0) + " SPM"}
        function onMaxAccuracyChanged(value) { accuracyMax.text = value.toFixed(1) + " %" }
    }

    function update() {
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

        wpm.text = wordCounter.wpm.toFixed(0) + " WPM"
        spm.text = wordCounter.spm.toFixed(0) + " SPM"
        accuracy.text = wordCounter.accuracy.toFixed(1) + " %"
    }

    Timer {
        id: timer
        interval: 1
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: main.update()
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

    GridLayout {
        id: gridLayout
        x: 164
        y: 81
        width: 404
        height: 79
        anchors.horizontalCenter: time.horizontalCenter
        columnSpacing: 0
        rowSpacing: 0
        columns: 3

        Text {
            id: wpm
            font.pixelSize: 28
            color: "blue"
            text: "240 WPM"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.bold: true
        }

        Text {
            id: spm
            text: "240 SPM"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
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
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            font.bold: true
        }

        Text {
            id: wpmLast
            color: "#0000ff"
            text: "240 WPM"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.bold: true
            font.pixelSize: 20
        }

        Text {
            id: spmLast
            color: "#add8e6"
            text: "240 SPM"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 20
        }

        Text {
            id: accuracyLast
            color: "#ee82ee"
            text: "100 %"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 20
        }

        Text {
            id: wpmMax
            color: "#0000ff"
            text: "240 WPM"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.underline: true
            font.italic: false
            font.bold: true
            font.pixelSize: 20
        }

        Text {
            id: spmMax
            color: "#add8e6"
            text: "240 SPM"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.underline: true
            font.italic: false
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 20
        }

        Text {
            id: accuracyMax
            color: "#ee82ee"
            text: "100 %"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            font.underline: true
            font.italic: false
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 20
        }
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:2;anchors_height:60;anchors_x:204;anchors_y:124}
}
##^##*/
