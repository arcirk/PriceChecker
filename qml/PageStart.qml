import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Page {

    id: pageConnanctions
    padding: 10
    property string theme: "Dark"
    property alias organization: org.text
    property alias subdivision: suborg.text
    property alias warehouse: stock.text
    property alias price: priceText.text

    property int fontPixelSizeGrey: screenWidth > 1000 ? 20 : 12

    function setBarcode(bInfo){
        txtBarcode.text = bInfo.barcode;
        txtName.text = bInfo.synonym;
        txtPrice.text = bInfo.price + " " + bInfo.currency;
        txtStockBalance.text = bInfo.balance + " " + bInfo.unit;
        if(!bInfo.isLongImgLoad){
            if(wsSettings.showImage){
                imageData.source = bInfo.imageSource;
                longOperation.visible = false
            }else
                imageData.visible = false
        }else{
            imageData.visible = false
            longOperation.visible = true
        }

        pane.visible = true;
    }

    function changeImageSource(bInfo){
        longOperation.visible = false
        imageData.source = bInfo.imageSource;
        imageData.visible = true
    }

    function setPaneVisible(value){
        pane.visible = value;
    }

//    function setImageFromByteArray(img){
//        imageData.data = img
//        imageData.visible = true
//    }

    Material.background: "white"

    GridLayout{
        id: grid
        anchors.top: parent.top
        z: 1
        columns: 2

           Text{
               leftPadding: 10
               topPadding: 10
               color: "gray"
               text: "Организация:"
               font.pixelSize: fontPixelSizeGrey
           }
           Text{
               id: org
               topPadding: 10
               color: "gray"
               font.pixelSize: fontPixelSizeGrey
           }

           Text{
               leftPadding: 10
               text: "Подразделение:"
               color: "gray"
               font.pixelSize: fontPixelSizeGrey
           }
           Text{
               id: suborg
               color: "gray"
                font.pixelSize: fontPixelSizeGrey
           }
           Text{
               leftPadding: 10
               text: "Склад:"
               color: "gray"
               font.pixelSize: fontPixelSizeGrey
           }
           Text{
               color: "gray"
               id: stock
               font.pixelSize: fontPixelSizeGrey
           }
           Text{
               leftPadding: 10
               text: "Тип цен:"
               color: "gray"
               font.pixelSize: fontPixelSizeGrey
           }
           Text{
               color: "gray"
               id: priceText
               font.pixelSize: fontPixelSizeGrey
           }
    }

    Pane{
        id: pane
//        anchors.top: parent.top
//        anchors.left: parent.left
//        anchors.bottom: parent.bottom
//        anchors.right: parent.right
        anchors.fill: parent
        visible: false
//        Rectangle {
//            width: pane.width - pageConnanctions.padding * 2
//            height: pane.height - pageConnanctions.padding * 2
//            color: "red"
//        }

        GridLayout{
            columns: 5
            rows: 5
            id: column
            width: pane.width - pageConnanctions.padding * 2

            RowLayout{
                Layout.columnSpan: 5
                Layout.rowSpan: 1
                Layout.row: 1
                Layout.column: 1
                Layout.alignment: Qt.AlignCenter
                Layout.maximumHeight: 400
                Layout.minimumHeight: 400
                //width: 300
                //padding: 5
                Image {
                    id: imageData
                    //height: 300
                    //verticalAlignment: Qt.AlignVCenter
//                    Layout.columnSpan: 5
//                    Layout.rowSpan: 1
//                    Layout.row: 1
//                    Layout.column: 1
//                    Layout.alignment: Qt.AlignCenter
                    //source: "qrc:/img/exampleQr.png"
                    Layout.maximumHeight: 400
//                    Layout.minimumHeight: 300
                    //Layout.alignment: Qt.AlignCenter
                    fillMode:Image.PreserveAspectFit; clip:true
                    visible: wsSettings.showImage
                }

                AnimatedImage{
                    id: longOperation
                    source: "qrc:/img/longOperation.gif"
//                    Layout.columnSpan: 5
//                    Layout.rowSpan: 1
//                    Layout.row: 1
//                    Layout.column: 1
//                    Layout.maximumHeight: 300
//                    Layout.minimumHeight: 300
                    Layout.alignment: Qt.AlignCenter
                    visible: false
                }

            }


            Text {

                Layout.fillWidth: true
                Layout.columnSpan: 3
                Layout.rowSpan: 1
                Layout.row: 2
                Layout.column: 2
                id: txtBarcode
                padding: 4
                font.pixelSize: 28
                //width: thisPage.width - thisPage.padding * 2
                width: column.width / 2
                fontSizeMode: Text.Fit
                minimumPixelSize: 8 // minimum height of the font
                font.bold: true
                //color: "blue"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: ""
            }
            Text {
                //Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.columnSpan: 3
                Layout.rowSpan: 1
                Layout.row: 3
                Layout.column: 2
                id: txtName
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
                width: column.width / 2
                //width: thisPage.width - thisPage.padding * 2
                horizontalAlignment: Qt.AlignHCenter
                font.pixelSize: 38
                color: "#536AC2"//pageConnanctions.theme !== "Dark" ? "black" : "white"
                text: ""

            }


            GridLayout{
                Layout.columnSpan: 1
                Layout.rowSpan: 1
                Layout.row: 4
                Layout.column: 3
                Layout.alignment: Qt.AlignCenter
                columns: 2

                Text {
                    text: "Цена:"
                    horizontalAlignment: Qt.AlignHCenter
                    font.pixelSize: 28
                }
                Text {
                    id: txtPrice
                    textFormat: Text.RichText
                    horizontalAlignment: Qt.AlignHCenter
                    font.pixelSize: 28
                    font.bold: true
                }
                Text {
                    text: "Остаток:"
                    font.pixelSize: 28
                }
                Text {
                    id: txtStockBalance
                    textFormat: Text.RichText
                    horizontalAlignment: Qt.AlignHCenter
                    font.pixelSize: 28
                    font.bold: true
                }

            }
        }

    }

}
