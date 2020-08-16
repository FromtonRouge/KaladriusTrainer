import QtQuick 2.0
import QtQuick.Layouts 1.3

Item {
    id: main
    visible: false

    Connections {
        target: wordRater
        function onWordToCompleteChanged(value) { textToRate.text = value }
    }

    Text {
        id: textToRate
        y: 183
        width: 419
        height: 47
        text: qsTr("complete")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenterOffset: 1
        anchors.verticalCenter: parent.verticalCenter
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 34
    }
}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:1;anchors_x:267}
}
##^##*/
