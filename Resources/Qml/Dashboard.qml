import QtQuick 2.0
import QtQuick.Layouts 1.3

Item {
    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        spacing: 5

        CountdownTimer {
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 5
            WordCounter { }
            Accuracy { }
        }
    }
}
