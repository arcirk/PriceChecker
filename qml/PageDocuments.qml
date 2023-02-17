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

    DialogDocumentInfo{
        id: docInfo
        visible: false

//        onAdd_document: function() {
//            wsClient.documentUpdate(docInfo.docNumber, docInfo.docDate, docInfo.docComent, "", docInfo.uuid)
//        }
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
//        Row{
//            id: rowTitle
//            topPadding: 10
//            Layout.fillWidth: true
//            width: parent.width
//            spacing: 10

//            ToolBar {
//                id: toolBar
//                ToolButton {
//                    id: btn
//                    icon.source: "qrc:/img/qr16.png"
//                }
//            }

//            Text {
//                id: txtTitle
//                leftPadding: 10
//                color: "gray"
//                wrapMode: Text.WordWrap
//                width: parent.width
//            }
//            Rectangle{
//                width: pageDocs.width - txtTitle.implicitWidth - 20
//                height: 1
//                color: "gray"
//                anchors.bottom: txtTitle.bottom
//            }

//        }

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
                            }
                        }
                        onClicked: function(row){
                            console.log("onSelectItem " + row)
                            listView.selectedRow(wsDocuments.index(row,0))
                        }
                    }
                }
            }
            //property QtObject selDelegate;

//            delegate: Column {
//                spacing: 6

//                Row {
//                    id: messageRow
//                    spacing: 6
////                    DelegateView{
////                        id: messageText
////                        name: model.second + "  " + model.number + " от " + wsClient.documentDate(model.date)
////                        theme: pageDocs.theme
////                        //icon: "qrc:/img/1cv8.png"
////                        //iconSize: 24
////                        width: listView.width - messageRow.spacing - 12
////                        height: messageText.implicitHeight// + 24
////                        ctrlPaddig: 10
////                        textColor: pageDocs.theme !== "Dark" ? "black" : "white"
////                        row: model.row;
////                        fontPixelSize: fontPixelSizeGrey + 8
////                        expandIcon: true
//////                        onSelectItem: function(selRow){
//////                            //console.log("onSelectItem " + selRow)
//////                            //listView.selectedRow(listView.model.index(selRow,0))
//////                            if(messageText.state !== "shown"){
//////                                messageText.state = "shown"
//////                                messageText.checked = true;
//////                                messageText.hidden = false;
//////                                messageText.expandState = "expanded"
//////                            }else{
//////                                messageText.state = "close"
//////                                messageText.checked = false;
//////                                messageText.hidden = true;
//////                                messageText.expandState = "collapsed"
//////                            }
//////                        }
////                        onMenuTriggered: function(command){
////                            if(command === "mnuOpen"){
////                                docInfo.visible = true
////                            }
////                        }

////                    }
//                    DocumentsDelegate{
//                        id: messageText
//                        width: listView.width - messageRow.spacing - 24 - 12
//                        //height: messageText.implicitHeight// + 38
//                        expandIcon:true
//                        name: model.second + "  " + model.number + " от " + wsClient.documentDate(model.date)
//                        ctrlPaddig: 10
//                        textColor: pageDocs.theme !== "Dark" ? "black" : "white"
//                        row: model.row;
//                        fontPixelSize: fontPixelSizeGrey + 8
////                        icon: "qrc:/img/1cv8.png"
////                        iconSize: 24
////                        onSelectedChanged: function(item){
////                            if(listView.selDelegate){
////                                listView.selDelegate.checked = false;
////                                listView.selDelegate.hidden = true;
////                                listView.selDelegate.expandState = "collapsed"
////                            }
////                            if(listView.selDelegate !== item)
////                                listView.selDelegate = item
////                            else{
////                                listView.selDelegate = null
////                            }
////                        }
//                    }
//                }
//            }

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
