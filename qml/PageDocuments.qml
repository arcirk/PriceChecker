import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import QJsonTableModel 1.0

Page {

    id: pageDocs
    padding: 10
    property string theme: "Dark"

    property int fontPixelSizeGrey: screenWidth > 1000 ? 20 : 8
    property int imageMaximumHeight: screenWidth > 1000 ? 400 : 250

    property string currentDocument: ""

    function setModelSource(value){
        wsDocuments.jsonText = value;
        wsDocuments.reset();
        console.log("setModelSource")
    }

    property QJsonTableModel wsDocuments: QJsonTableModel{

    }

    DoQueryBox{
        id: queryBox
        visible: false

        onAccept: {
            wsClient.deleteDocument(queryBox.uuid, queryBox.version)
            wsClient.getDocuments()
        }
    }

    DialogDocumentInfo{
        id: docInfo
        visible: false

        onAccept:function(modelIndex){
            let source = "";
            if(modelIndex !== undefined){
                try{
                    source = wsDocuments.getObjectToString(modelIndex.row)
                }catch(e){
                    source = "";
                }
            }
            wsClient.documentUpdate(docInfo.docNumber, docInfo.docDate, docInfo.docComent, source, docInfo.uuid)
        }

    }

    function new_number(){
        let m = wsDocuments.max("_id");
        if(m > 0)
            return wsClient.documentGenerateNewNumber(m + 1);
        else
            return wsClient.documentGenerateNewNumber(1);
    }

    signal getContent(string ref);

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
            model: wsDocuments

            signal selectedRow(var modelindex)
            signal removeRow(var modelindex)

            delegate: Column {
                spacing: 6
                Row {
                    id: messageRow
                    spacing: 6
                    Layout.fillWidth: true
                    ListItemDelegate{
                        id: delegate
                        modelIndex: model
                        currentTable: "documents"
                        width: listView.width - messageRow.spacing - 12
                        uuid: model.ref
                        onMenuTriggered: function(command){
                            console.debug("onMenuTriggered: " + command)
                            if(command === "mnuOpen"){
                                docInfo.docNumber = model.number
                                docInfo.docDate = wsClient.documentDate(model.date)
                                docInfo.docComent = delegate.getComment()
                                //docInfo.docSourceComment = delegate.getSourceComment();
                                docInfo.modelIndex = model;
                                docInfo.visible = true
                            }else if(command === "mnuDelete"){
                                queryBox.text = "Удалить выбранный документ?"
                                queryBox.uuid = model.ref
                                queryBox.version = Number(model.version);
                                queryBox.visible = true
                            }
                        }
                        onClicked: function(row){
                            console.log("onSelectItem " + row)
                            listView.selectedRow(wsDocuments.index(model.row,0))
                        }
                    }
                }
            }

            onSelectedRow: function(index){
                var uuid = ""
                var iUuid = wsDocuments.getColumnIndex("ref")
                if(iUuid !== -1){
                    uuid = wsDocuments.value(index, Qt.UserRole + iUuid)
                    console.log(uuid)
                    pageDocs.getContent(uuid)
                }else
                    return;
//                var doc = ""
//                var iDoc = wsDocuments.getColumnIndex("document_name")
//                if(iDoc !== -1){
//                    doc = wsDocuments.value(index, Qt.UserRole + iDoc)
//                }
//                var uuid_form = ""
//                var iUuid_form = wsDocuments.getColumnIndex("uuid_form")
//                if(iUuid_form !== -1){
//                    uuid_form = wsDocuments.value(index, Qt.UserRole + iUuid_form)
//                }
//                pageDocs.selectDocument(uuid, doc, uuid_form)
            }

            ScrollBar.vertical: ScrollBar {}
        }
    }

}
