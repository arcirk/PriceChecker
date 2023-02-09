import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Popup {

    id: dialog
    anchors.centerIn: parent
    width: screenWidth > 1000 ? parent.width / 2 : parent.width - 20
    //height: 200
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    property int fontPixelSize: screenWidth > 1000 ? 20 : 12

    property string uuid: ""
    property int row: 0
    property alias barcode: txtBarcode.text
    property alias quantity: txtQuantity.text
    property string theme: "Dark"
    GridLayout{
        id: grid
        columns: 2
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.margins: 10
        columnSpacing: fontPixelSize
        Text{
            font.pixelSize: fontPixelSize
            text: "Штрихкод:"
            color: dialog.theme === "Light" ? "#424242" : "#efebe9"
        }
        Text{
            id: txtBarcode
            font.pixelSize: fontPixelSize
            color: dialog.theme === "Light" ? "#424242" : "#efebe9"
        }
        Text{
            font.pixelSize: fontPixelSize
            text: "Количество:"
            color: dialog.theme === "Light" ? "#424242" : "#efebe9"
        }
        TextField{
            id: txtQuantity
            font.pixelSize: fontPixelSize
            //text: wsSettings.url() //"ws://<domainname>"
            color: dialog.theme === "Light" ? "#424242" : "#efebe9"
            validator: IntValidator {bottom: 1; top: 10000}
            Material.accent: Material.Blue
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            //enabled: !wsClient.isStarted();
            onEditingFinished: {
                //wsSettings.setUrl(txtServer.text);
            }

        }
        RowLayout{
            Layout.alignment: Qt.AlignRight
            Layout.columnSpan: 2 // screenWidth > 1000 ? 1 : 2
            Button{
                id: btnOK
                text: screenWidth > 1000 ? "OK" : ""
                icon.source:  screenWidth > 1000 ? "" : "qrc:/img/to_data.png"
                onClicked: {


                }
            }

            Button{
                id: btnDislogClose
                text: "Закрыть"

                onClicked: {
                    dialog.visible = false
                }
            }
        }

    }

}
