import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import QJsonTableModel 1.0
import QProxyModel 1.0

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
    property bool findToolBar: false


    DoQueryBox{
        id: queryBox
        visible: false

        onAccept: {
            wsClient.removeRecord(queryBox.uuid, wsDocumentTable)
        }
    }

    function setFilterBarcode(value){
        txtFilter.text = value
        //wsProxyModel.setFilter("{\"barcode\":\"%1\"}".arg(txtFilter.text))
    }

    property QJsonTableModel wsDocumentTable: QJsonTableModel{
        onCurrentRowChanged: {
//            wsDocumentTable.reset();
            console.debug("wsDocumentTable.onCurrentRowChanged " + wsDocumentTable.currentRow)
        }
    }

    property QProxyModel wsProxyModel: QProxyModel{
        sourceModel: wsDocumentTable
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

    function editBarcode(barcode, quantity){
        let index = wsDocumentTable.getColumnIndex("barcode")
        let indexRef = wsDocumentTable.getColumnIndex("ref")
        let mIndex = wsDocumentTable.findInTable(barcode, index, false);
        wsClient.documentContentUpdate(barcode, Number(quantity), pageDoc.ref, wsDocumentTable.value(mIndex, Qt.UserRole + indexRef), wsDocumentTable)
    }

    signal viewBarcodeInfo(string br);

    DialogEditBarcode {
        id: dlgEdit
        visible: false
        theme: pageDoc.theme

        onAccept: {
            if(dlgEdit.quantity.trim() === ''){
                wsClient.displayError("Ошибка", "Не указано количество!")
                return;
            }
            if(Number(dlgEdit.quantity) === 0){
                wsClient.displayError("Ошибка", "Не указано количество!")
                return;
            }

            pageDoc.editBarcode(dlgEdit.barcode, dlgEdit.quantity)
            wsDocumentTable.reset()
        }

        onVisibleChanged: {
//            if(!dlgEdit.visible)
//                wsDocumentTable.reset()
        }
    }

    header: ToolBar{
        id: toolBarFind
        visible: pageDoc.findToolBar
        Material.background: myTheme === "Light" ? "#efebe9" : "#424242"
        padding: 10
//        TextEdit{
//            id: txtFind
//            anchors.fill: parent
//            //Material.background: "#efebe9"
//            color: "#efebe9"
//        }
        Rectangle {
            color: "#efebe9"
            TextInput {
                id: txtFilter
                padding: 5
                text: ""
                font.bold: true
                font.pixelSize: 18
                Material.accent: Material.Blue
                width: parent.width
                onTextChanged: {
                    wsProxyModel.setFilter("{\"barcode\":\"%1\"}".arg(txtFilter.text))
                }
                onAccepted: {
                    txtFilter.focus = false
                }
            }
            anchors.fill: parent
            Component.onCompleted: {
                txtFilter.focus = true
            }


//            width: childrenRect.width
//            height: childrenRect.height
        }

        onVisibleChanged: {
            txtFilter.focus = toolBarFind.visible
            if(!toolBarFind.visible && txtFilter.text !== ""){
                txtFilter.text = ""
                wsProxyModel.setFilter("{\"barcode\":\"%1\"}".arg(txtFilter.text))
            }
        }
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
            model: wsProxyModel //wsDocumentTable

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
                        selectedIndex: wsDocumentTable.currentRow === model.row ? true : false
                        //backgroundColor: delegate.selectedIndex ? "red" : control.Material.backgroundColor
                        onMenuTriggered: function(command){
                            //console.debug("onMenuTriggered: " + command)
                            wsDocumentTable.currentRow = model.row;
                            //listView.model.reset();
                            if(command === "mnuOpen"){
                                pageDoc.viewBarcodeInfo(model.barcode)
                            }else if(command === "mnuDelete"){
                                queryBox.text = "Удалить выбранную строку?"
                                queryBox.uuid = model.ref
                                queryBox.version = Number(model.version);
                                queryBox.visible = true
                            }
                        }
                        onClicked: function(row){
                            wsClient.debugViewTime();
                            //console.log("onSelectItem " + row)
                            wsDocumentTable.currentRow = Number(row);

                            listView.selectedRow(model)
                            //listView.model.reset();

                            //listView.model.dataChanged(listView.model.index(wsDocumentTable.currentRow,0), listView.model.index(wsDocumentTable.currentRow,0))
                        }

                    }
                }
            }

            onSelectedRow: function(index){
//                //var uuid = ""
//                var iUuid = wsDocumentTable.getColumnIndex("ref")
//                var iBr = wsDocumentTable.getColumnIndex("barcode")
//                var iQ = wsDocumentTable.getColumnIndex("quantity")
//                //uuid = wsDocumentTable.value(index, Qt.UserRole + iUuid)



                dlgEdit.row = index.row
                dlgEdit.uuid = index.ref// wsDocumentTable.data(index, Qt.UserRole + iUuid).toString();
                dlgEdit.barcode = index.barcode //wsDocumentTable.data(index, Qt.UserRole + iBr).toString();
                dlgEdit.quantity = index.quantity // wsDocumentTable.data(index, Qt.UserRole + iQ).toInt();
                dlgEdit.visible = true

                //listView.model.reset();

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
