import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Page {

    id: pageStart
    padding: 10
    property string theme: "Dark"
    property alias organization: org.text
    property alias subdivision: suborg.text
    property alias warehouse: stock.text
    property alias price: priceText.text

    property int fontPixelSizeGrey: screenWidth > 1000 ? 20 : 8
//    property int imageMaximumHeight: screenWidth > 1000 ? 400 : 250

//    function setBarcode(bInfo){
//        txtBarcode.text = bInfo.barcode;
//        txtName.text = bInfo.synonym;
//        txtPrice.text = bInfo.price + " " + bInfo.currency;
//        txtStockBalance.text = bInfo.balance + " " + bInfo.unit;
//        if(!bInfo.isLongImgLoad){
//            if(wsSettings.showImage){
//                imageData.source = bInfo.imageSource;
//                longOperation.visible = false
//            }else
//                imageData.visible = false
//        }else{
//            imageData.visible = false
//            longOperation.visible = true
//        }

//        pane.visible = true;
//    }

//    function changeImageSource(bInfo){
//        longOperation.visible = false
//        imageData.source = bInfo.imageSource;
//        imageData.visible = true
//    }

//    function setPaneVisible(value){
//        pane.visible = value;
//    }

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

}
