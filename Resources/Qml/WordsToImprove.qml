import QtQuick 2.0
import QtQuick.Layouts 1.3

Item {
    id: main

    Connections {
        target: wordsToImprove

        function onWordsToImproveChanged(words)
        {
            var i = 0;

            // Hide all cards by default
            for (i = 1; i < 5; ++i)
            {
                repeaterWordCards.itemAt(i).visible = false;
            }

            i = 0;
            for (i in words)
            {
                repeaterWordCards.itemAt(i).visible = true;
                repeaterWordCards.itemAt(i).setWord(words[i]);
            }
        }

        function onProgressValuesChanged(progressValues)
        {
            var i = 0;

            // Hide all cards by default
            for (i = 1; i < 5; ++i)
            {
                repeaterWordCards.itemAt(i).visible = false;
            }

            i = 0;
            for (i in progressValues)
            {
                repeaterWordCards.itemAt(i).visible = true;
                repeaterWordCards.itemAt(i).setProgress(progressValues[i]);
            }
        }
    }

    Row {
        y: 3
        spacing: 8
        Repeater {
            id: repeaterWordCards
            model: 5

            WordCard {
            }
        }
    }
}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:1;anchors_x:267}
}
##^##*/
