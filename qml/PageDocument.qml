import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import QJsonTableModel 1.0

Page {

    id: pageDoc
    padding: 10
    property string theme: "Dark"
    property string ref: ""
    property string docName: ""
    property string currentBarcode: ""
    property int currentQuantity: 0
    property int fontPixelSizeGrey: screenWidth > 1000 ? 20 : 8
    property int imageMaximumHeight: screenWidth > 1000 ? 400 : 250

    property QJsonTableModel wsDocumentTable: QJsonTableModel{

    }

    function setModelSource(value){
        wsDocumentTable.jsonText = value;
        wsDocumentTable.reset();
    }


    function setBarcode(bInfo){
        //console.log(bInfo.barcode)

        let index = wsDocumentTable.getColumnIndex("barcode")
        let indexRef = wsDocumentTable.getColumnIndex("ref")
        let indexQu = wsDocumentTable.getColumnIndex("quantity")
        if(index !== -1){
            let mIndex = wsDocumentTable.findInTable(bInfo.barcode, index, false);
              let q = wsDocumentTable.data(mIndex, Qt.UserRole + indexQu);
            if(q === undefined)
                q = 0;
//            if(mIndex.ref !== undefined){
                wsClient.documentContentUpdate(bInfo.barcode, Number(q) + 1, pageDoc.ref, wsDocumentTable.value(mIndex, Qt.UserRole + indexRef), wsDocumentTable)
//            }else{

//            }
        }
    }

    DialogEditBarcode {
        id: dlgEdit
        visible: false
        theme: pageDoc.theme
    }

    Column{
        anchors.fill: parent
        spacing: 10

        ListView {
            id: listView
            //topMargin: 10 + rowTitle.height + rowTitle.padding
            leftMargin: 10
            rightMargin: 10
            anchors.fill: parent
            displayMarginBeginning: 40
            displayMarginEnd: 40
            spacing: 12
            model: wsDocumentTable

            signal selectedRow(var modelindex)
            signal removeRow(var modelindex)

            delegate: Column {
                spacing: 6

                Row {
                    id: messageRow
                    spacing: 6
//                    DelegateView{
//                        id: messageText
//                        name: model.barcode + ", Количество: " + model.quantity
//                        theme: pageDoc.theme
//                        //icon: "qrc:/img/1cv8.png"
//                        iconSize: 24
//                        width: listView.width - messageRow.spacing - 12
//                        height: messageText.implicitHeight// + 24
//                        ctrlPaddig: 10
//                        textColor: pageDoc.theme !== "Dark" ? "black" : "white"
//                        row: model.row;
//                        fontPixelSize: fontPixelSizeGrey + 8
//                        onSelectItem: function(selRow){
//                            console.log("onSelectItem " + selRow)
//                            pageDoc.currentBarcode = model.barcode
//                            pageDoc.currentQuantity = model.quantity
//                            listView.selectedRow(listView.model.index(selRow,0))

//                        }

//                    }
                    ListItemDelegate{
                        id: delegate
                        modelIndex: model
                        currentTable: "document_table"
                        width: listView.width - messageRow.spacing - 12
                        uuid: model.ref
                        onMenuTriggered: function(command){

                        }
                        onClicked: function(row){
//                            console.log("onSelectItem " + row)
//                            listView.selectedRow(wsDocuments.index(row,0))
                        }
                    }
                }
            }

            onSelectedRow: function(index){
//                dlgEdit.barcode = pageDoc.currentBarcode
//                dlgEdit.quantity = pageDoc.currentQuantity
//                dlgEdit.visible = true


//                var uuid = ""
//                var iUuid = wsDocuments.getColumnIndex("ref")
//                if(iUuid !== -1){
//                    uuid = wsDocuments.value(index, Qt.UserRole + iUuid)
//                    console.log(uuid)
//                }else
//                    return;
////                var doc = ""
////                var iDoc = wsDocuments.getColumnIndex("document_name")
////                if(iDoc !== -1){
////                    doc = wsDocuments.value(index, Qt.UserRole + iDoc)
////                }
////                var uuid_form = ""
////                var iUuid_form = wsDocuments.getColumnIndex("uuid_form")
////                if(iUuid_form !== -1){
////                    uuid_form = wsDocuments.value(index, Qt.UserRole + iUuid_form)
////                }
//                pageDocs.selectDocument(uuid, doc, uuid_form)
            }

            ScrollBar.vertical: ScrollBar {}
        }
    }
}
