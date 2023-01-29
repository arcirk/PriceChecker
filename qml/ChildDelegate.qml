import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Rectangle{

    id: smailPane;
    state: hidden ? "hidden" : "shown"
    color: "blue"

    signal closePane

    property bool hidden: true

    Pane {
        anchors.fill: parent

//        ColumnLayout{
//            id: mainPane
//            Layout.fillWidth: true

            RowLayout{
               id: mainPane
               Layout.fillWidth: true
               spacing: 28
               //Component.onCompleted: Scripts.createButtons(mainPane)
               Text{
                   text: "Текст комментария"
               }
            }

//        }
        ToolButton {
            id: btnCloseSmilePane
            anchors.right: parent.right
            text: "x"

            onClicked: {
                //closeBtn.checked = false
                smailPane.closePane()
                smailPane.state = "hidden"
            }

        }
    }
    states: [
        State {
            name: "shown"
            PropertyChanges { target: smailPane; height: 70 }
        },
        State {
            name: "hidden"
            PropertyChanges { target: smailPane; height: 1 }
        }
    ]
    transitions: [
        Transition {
            from: "hidden"
            to: "shown"
            NumberAnimation {
                target: smailPane
                property: "height"
                duration: 200
                easing.type: Easing.InOutQuad
                from: 1
                to: 70
            }
        },
        Transition {
            from: "shown"
            to: "hidden"
            NumberAnimation {
                target: smailPane
                property: "height"
                duration: 200
                easing.type: Easing.InOutQuad
                from: 70
                to: 1
            }

        }
    ]
}
