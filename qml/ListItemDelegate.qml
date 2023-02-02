import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QJsonTableModel 1.0

RoundPane {
    id: control
    padding: 2

    property int selectedIndex: 0

    property int fontPixelSize: screenWidth > 1000 ? 20 : 16

    property QtObject modelIndex: undefined
    property string currentTable: "documents"
    property string theme: "Dark"
    property bool isSelected: false
    property bool menuDisable: false
    property bool checkable: false
    property bool menuDeleteDisable: false
    property string uuid: ""

    function getValue(field){
        if(control.model === undefined)
            return undefined;
        let index = control.model.getColumnIndex(field);
        if(index !== -1)
            return control.model.value(control.modelIndex, Qt.UserRole + index)
        else
            return undefined
    }

    function getSynonim(){
        if(control.currentTable === "documents")
            return control.modelIndex.second + "  " + control.modelIndex.number + " от " + wsClient.documentDate(control.modelIndex.date)
        else if(control.currentTable === "document_table")
            return control.modelIndex.barcode + ", Количество: " + control.modelIndex.quantity
    }

    function getComment(){
        let cm = control.modelIndex.cache
        if(cm !== undefined && cm !== ""){
            let obj = JSON.parse(cm)
            return obj['comment']
        }else
            return ""
    }

    function getSourceComment(){
        let cm = control.modelIndex.cache
        if(cm !== undefined && cm !== ""){
            let obj = JSON.parse(cm)
            return obj
        }else
            return ""
    }

    Material.elevation: {
        if(isSelected)
            1
        else
            7
    }

    radius: 3

    Material.background:{

        if(control.checked && !control.chldrenList){
            if(control.theme == "Dark")
                "#424242"
            else
                "#efebe9"
        }else
            undefined
    }

    signal menuTriggered(string name)
    //signal imageClick()
    signal clicked(string buttonId)
    //signal selectItem(int selRow)

    Column{
        id: colControl
        spacing: 12
        anchors.fill: parent

        Text {
            id: txt;
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
            text: control.getSynonim()
            width: parent.width
            //padding: 10
            font.pixelSize: control.fontPixelSize
            color: control.theme === "Light" ? "#424242" : "#efebe9"

            leftPadding: 10
            rightPadding: 10
            topPadding: 10
            bottomPadding: 10//control.getComment() !== "" ? 0 : 10
            MouseArea{
                id:mouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor;

                onClicked: {
                    control.clicked(modelIndex.row)
                    console.log("onClicked");
                }
                onEntered: {
                    if(!control.checkable){
                        control.Material.elevation = 1
                    }
                    console.log("onEntered");
                }
                onExited: {
                    if(!control.checkable)
                        control.Material.elevation = 7
                    console.log("onExited");
                    //selectItem(control.row)
                }
                onPressAndHold: {
                        if (!control.menuDisable)
                            contextMenu.popup()
                    }

                Menu {
                    id: contextMenu
                    Action {
                        text: control.currentTable === "documents" ? "Изменить" : "Информация"
                        icon.source: "qrc:/img/edit_document.svg"
                        onTriggered: {
                            control.menuTriggered("mnuOpen")
                        }
                    }
//                        Action { text: "Копировать" }
//                        Action { text: "Переслать" }
                    Action {
                        text: "Удалить";
                        enabled: !menuDeleteDisable;
                        icon.source: "qrc:/img/delete_doc.svg"
                        onTriggered: {
                            console.debug("delete")
                        }
                    }
                }
            }
        }

        Pane{
            leftPadding: 10
            rightPadding: 10
            topPadding: 0
            bottomPadding: 0
            anchors.top: txt.bottom
        Rectangle {
            width: control.width - 20
            height: 1
            color: "gray"
            visible: getComment() !== "" ? true : false

        }
        }
        Text {
            id: txtComment;
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
            text: control.getComment()
            width: parent.width
            font.pixelSize: control.fontPixelSize
            color: control.theme === "Light" ? "#424242" : "#efebe9"
            leftPadding: 10
            rightPadding: 10
            topPadding: 0
            bottomPadding: 10
            visible: control.getComment() !== "" ? true : false

            MouseArea{
                id:mouseAreaComment
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor;

                onClicked: {
                    control.clicked(control.row)
                    //selectItem(control.row)
                    console.log("onClicked");
                }
                onEntered: {
                    if(!control.checkable){
                        control.Material.elevation = 1
                    }
                    console.log("onEntered");
                }
                onExited: {
                    if(!control.checkable)
                        control.Material.elevation = 7
                    console.log("onExited");                   
                }
                onPressAndHold: {
                        if (!menuDisable)
                            contextMenuComment.popup()
                    }

                Menu {
                    id: contextMenuComment
                    Action {
                        text: control.currentTable === "documents" ? "Изменить" : "Информация"
                        icon.source: "qrc:/img/edit_document.svg"
                        onTriggered: {
                            control.menuTriggered("mnuOpen")
                        }
                    }
//                        Action { text: "Копировать" }
//                        Action { text: "Переслать" }
                    Action {
                        text: "Удалить";
                        enabled: !menuDeleteDisable;
                        icon.source: "qrc:/img/delete_doc.svg"
                        onTriggered: {
                            console.debug("delete")
                        }
                    }
                }
            }
        }
    }

//    property int  _id: 0
//    property string first: ""
//    property string second: ""
//    property string ref: ""
//    property string cache: ""
//    property string number: ""
//    property date date: new Date()
//    property string xml_type: ""
//    property string device_id: ""
//    property double price: 0
//    property double quantity: 0
//    property string barcode: ""
//    property string parent: ""



//    function clearObject(){
//        control._id = 0
//        control.first = ""
//        control.second = ""
//        control.ref = ""
//        control.cache = ""
//        control.number = ""
//        control.date = new Date()
//        control.xml_type = ""
//        control.device_id = ""
//        control.price = 0
//        control.quantity = 0
//        control.barcode = ""
//        control.parent = ""
//    }

//    function loadObject(){
//        let id_ = control.model.getColumnIndex("_id");
//        let first_ = control.model.getColumnIndex("first");
//        let second_ = control.model.getColumnIndex("second");
//        let ref_ = control.model.getColumnIndex("ref");
//        let cache_ = control.model.getColumnIndex("cache");
//        let xml_type_ = -1;
//        let device_id_ = -1;
//        let price_ = -1;
//        let quantity_ = -1;
//        let barcode_ = -1;
//        let parent_ = -1;
//        if(control.currentTable === "Documents"){
//            xml_type_ = control.model.getColumnIndex("xml_type");
//            device_id_ = control.model.getColumnIndex("device_id");
//        }else if(control.currentTable === "document_table"){
//            price_ = control.model.getColumnIndex("price");
//            quantity_ = control.model.getColumnIndex("quantity");
//        }

//        control._id = control.model.value(control.modelIndex, Qt.UserRole + id_)
//    }

//    onModelIndexChanged: {
//        //clearObject()
//        if(control.modelIndex !== undefined)  {

//        }
//    }

}
