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

    function senBarcode(bInfo){
        txtBarcode.text = bInfo.barcode;
        txtName.text = bInfo.synonym;
        txtPrice.text = bInfo.price;
        txtStockBalance.text = bInfo.balance;
    }

Material.background: "white"

    GridLayout{
        id: grid
        columns: 2

           Text{
               leftPadding: 10
               topPadding: 10
               color: "gray"
               text: "Организация:"
           }
           Text{
               id: org
               topPadding: 10
               color: "gray"
           }

           Text{
               leftPadding: 10
               text: "Подразделение:"
               color: "gray"
           }
           Text{
               id: suborg
               color: "gray"

           }
           Text{
               leftPadding: 10
               text: "Склад:"
               color: "gray"
           }
           Text{
               color: "gray"
               id: stock
           }
           Text{
               leftPadding: 10
               text: "Тип цен:"
               color: "gray"
           }
           Text{
               color: "gray"
               id: priceText
           }
    }

    Pane{
        id: pane
        anchors.top: grid.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right

//        Rectangle {
//            width: pane.width - pageConnanctions.padding * 2
//            height: pane.height - pageConnanctions.padding * 2
//            color: "red"
//        }

        GridLayout{
            columns: 5
            rows: 5
            id: column
            //spacing: 10
            width: pane.width - pageConnanctions.padding * 2
            //height: pane.height - pageConnanctions.padding * 2
//            Layout.fillWidth: true
               // Row{


                    //Layout.row: 0
                    //Layout.column: 3
                    //Layout.columnSpan: 5
                    //id: rowImage
                    //Layout.fillWidth: true
                    //anchors.horizontalCenter: parent.horizontalCenter

                        Image {
                            //Layout.fillHeight: true
                            //Layout.fillWidth: true
                            Layout.columnSpan: 5
                            Layout.rowSpan: 1
                            Layout.row: 1
                            Layout.column: 1
                            id: imgBarcode
                            Layout.alignment: Qt.AlignCenter
                            source: "qrc:/img/exampleQr.png"

                            //visible: false
                        }




               // }
//                Row{
//                    Layout.fillWidth: true
//                    id: rowBarcode
//                    //anchors.top: rowImage.bottom
//                    anchors.horizontalCenter: parent.horizontalCenter
//                    //width: column.width / 2

                    Text {
                                            //Layout.fillHeight: true
                                            Layout.fillWidth: true
                                            Layout.columnSpan: 3
                                            Layout.rowSpan: 1
                                            Layout.row: 2
                                            Layout.column: 2
                        id: txtBarcode
                        padding: 4
                        font.pixelSize: 24
                        //width: thisPage.width - thisPage.padding * 2
                        width: column.width / 2
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 8 // minimum height of the font
                        font.bold: true
                        //color: "blue"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: "23023457483927"
                    }
//                Row{
//                    id: rowName
//                    anchors.top: rowBarcode.bottom
//                    anchors.horizontalCenter: parent.horizontalCenter
//                    width: column.width / 2
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
                        font.pixelSize: 32
                        color: "#536AC2"//pageConnanctions.theme !== "Dark" ? "black" : "white"
                        text: "ж/комплект/Лика дресс/ 5653/92/Good Night A"

                    }
//                }

                GridLayout{
//                    Layout.fillHeight: true
                    //Layout.fillWidth: true
                    Layout.columnSpan: 1
                    Layout.rowSpan: 1
                    Layout.row: 4
                    Layout.column: 3
                    Layout.alignment: Qt.AlignCenter
                    columns: 2
//                    anchors.top: rowName.bottom
//                    width: column.width / 2
                    Text {
//                        Layout.fillHeight: true
//                        Layout.fillWidth: true
//                        Layout.columnSpan: 1
//                        Layout.rowSpan: 1
//                        Layout.row: 4
//                        Layout.column: 2
                        text: "Цена:"
                        horizontalAlignment: Qt.AlignHCenter
                    }
                    Text {
//                        Layout.fillHeight: true
//                        Layout.fillWidth: true
//                        Layout.columnSpan: 2
//                        Layout.rowSpan: 1
//                        Layout.row: 4
//                        Layout.column: 3
                        id: txtPrice
                        textFormat: Text.RichText
                        //width: pageConnanctions.width - pageConnanctions.padding * 2
                        horizontalAlignment: Qt.AlignHCenter
                        font.pixelSize: 22
                        //color: pageConnanctions.theme !== "Dark" ? "black" : "white"
                        text: "100 руб"
                    }
                    Text {
//                        Layout.columnSpan: 1
//                        Layout.rowSpan: 2
//                        Layout.row: 5
//                        Layout.column: 2
                        text: "Количество:"
                        //verticalAlignment: Qt.AlignRight

                    }
                    Text {
//                        Layout.columnSpan: 2
//                        Layout.rowSpan: 2
//                        Layout.row: 5
//                        Layout.column: 3
                        id: txtStockBalance
                        textFormat: Text.RichText
                        //width: pageConnanctions.width - pageConnanctions.padding * 2
                        horizontalAlignment: Qt.AlignHCenter
                        font.pixelSize: 22
                        //color: pageConnanctions.theme !== "Dark" ? "black" : "white"
                        text: "10 шт"
                    }


//                }
            }
        }

    }




}
