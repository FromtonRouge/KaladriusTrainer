import QtQuick 2.15
import QtQuick.Layouts 1.3

Item {
    id: main
    width: rectangle.width
    height: rectangle.height
    property var currentProgression: 0
    property var newProgression: 0

    function setWord(txt) {word.text = txt}
    function setProgress(prg)
    {
        changeColor.progress = prg/100;
        if (prg < 0)
        {
            progress.text = "NEW";
            progress.color = "yellow";
            progress.font.pixelSize = 15;
            progress.font.italic = true;
        }
        else
        {
            progress.text = prg;
            progress.color = "#636363";
            progress.font.pixelSize = 20;
            progress.font.italic = false;
        }
        currentProgression = prg;
    }

    function setProgressResult(prg)
    {
        newProgression = prg;
        timer.start();
    }

    function animateProgress()
    {
        if (currentProgression < newProgression)
        {
            currentProgression += 1;
        }
        else if (currentProgression > newProgression)
        {
            currentProgression -= 1;
        }
        else
        {
            timer.stop();
        }

        setProgress(currentProgression);
    }

    Timer {
        id: timer
        interval: 5
        repeat: true
        running: false
        triggeredOnStart: false
        onTriggered: main.animateProgress()
    }

    AnimationController {
        id: changeColor
        animation:
            ColorAnimation {
            target: rectangle
            property: "color"
            from: "#ff958c"
            to: "#88f25e"
        }
        progress: 0
    }

    Rectangle {
        id: rectangle
        width: rowLayout.width
        height: rowLayout.height
        color: "#98fb95"
        radius: 5
        border.width: 2
        anchors.rightMargin: -126

        RowLayout {
            id: rowLayout
            height: 39
            anchors.rightMargin: -95

            Text {
                id: word
                width: 133
                height: 40
                text: qsTr("Word")
                Layout.bottomMargin: 5
                Layout.leftMargin: 8
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.bold: true
                font.pixelSize: 28
            }

            Text {
                id: progress
                width: 94
                height: 40
                text: qsTr("100")
                verticalAlignment: Text.AlignVCenter
                Layout.rightMargin: 8
                horizontalAlignment: Text.AlignHCenter
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.bold: true
                font.pixelSize: 20
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:1.25}D{i:2;anchors_height:46;anchors_width:227;anchors_x:40;anchors_y:74}
D{i:1;anchors_height:46;anchors_width:227;anchors_x:0;anchors_y:0}
}
##^##*/
