import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Popup {
    id: popupSettingsDialog
    anchors.centerIn: parent
    width: screenWidth > 1000 ? parent.width / 2 : parent.width - 20
    //height: 200
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    property int fontPixelSize: screenWidth > 1000 ? 20 : 12

    property string theme: "Dark"

    function updateHttpServiceConfiguration(hsHost, hsUser, hsPwd){
        wsSettings.httpService = hsHost;
        txtHttpService.text = hsHost;
    }

    signal webSocketConnect();

    onVisibleChanged: {
        if(!popupSettingsDialog.visible)
            wsSettings.save();
        else
            if(screenWidth > 1000)
                btnConnected.text = wsClient.isStarted() ? "Отключится" : "Подключится";
            else
                btnConnected.icon.source = wsClient.isStarted() ? "qrc:/img/lan_check_online.png" : "qrc:/img/lan_check_offline.png"
    }

    function connectionState(value){
        if(screenWidth > 1000)
            btnConnected.text = value ? "Отключится" : "Подключится";
        else
            btnConnected.icon.source = wsClient.isStarted() ? "qrc:/img/lan_check_online.png" : "qrc:/img/lan_check_offline.png"
    }

    GridLayout{
        id: grid
        columns: 2
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.margins: screenWidth > 1000 ? 10 : 2
        columnSpacing: fontPixelSize
        rowSpacing: 0
        Text{
            font.pixelSize: fontPixelSize
            text: "Сервер:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
            padding: 0
        }

        TextField{
            id: txtServer
            font.pixelSize: fontPixelSize
            text: wsSettings.url() //"ws://<domainname>"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
            Material.accent: Material.Blue
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            padding: 0
            onEditingFinished: {
                wsSettings.setUrl(txtServer.text);
            }

            onAccepted: {
                txtServer.focus = false
            }
        }

        Text{
            font.pixelSize: fontPixelSize
            text: "Пользователь:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
            padding: 0
        }

        TextField{
            id: txtUser
            font.pixelSize: fontPixelSize
            text: wsSettings.userName
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
            Material.accent: Material.Blue
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            padding: 0
            onEditingFinished: {
                wsSettings.userName = txtUser.text;
                wsSettings.httpUser = txtUser.text;
            }

            onAccepted: {
                txtUser.focus = false
            }

        }

        Text{
            font.pixelSize: fontPixelSize
            text: "Пароль:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
        }

        RowLayout{
            Layout.fillWidth: true
            spacing: 0
            Layout.margins: 0
            TextField{
                id: txtPass
                font.pixelSize: fontPixelSize
                text: wsSettings.hash.length > 0 ? "***" : "";
                color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"

                echoMode: TextInput.Password
                Material.accent: Material.Blue
                placeholderText: qsTr("Пароль")
                enabled: pwdEditPass.checked
                Layout.fillWidth: true
                wrapMode: Text.WordWrap

                onEditingFinished: {
                    wsSettings.hash = wsClient.generateHash(txtUser.text, txtPass.text)
                    wsSettings.httpPwd = wsClient.cryptPass(txtPass.text, "my_key");
                    wsSettings.save();
                }

                onEnabledChanged: {
                    var textColor = popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
                    if(txtPass.enabled){
                        txtPass.color = popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
                    }else{
                        txtPass.color = "grey";
                    }
                }

                onAccepted: {
                    txtPass.focus = false
                }
            }
            ToolButton{
                id: pwdViev
                icon.source: "qrc:/img/viewPwd1.png"
                checkable: true
                enabled: pwdEditPass.checked
                Material.accent: Material.Blue
                Layout.alignment: Qt.AlignRight
                padding: 0
                Layout.topMargin: 0
                onCheckedChanged: {
                    pwdViev.icon.source = pwdViev.checked ? "qrc:/img/viewPwd.png" : "qrc:/img/viewPwd1.png"
                }
                onClicked: {
                    txtPass.echoMode = pwdViev.checked ? TextInput.Normal : TextInput.Password
                }
            }
            ToolButton{
                id: pwdEditPass
                icon.source: "qrc:/img/itemEdit.png"
                checkable: true
                Material.accent: Material.Blue
                Layout.alignment: Qt.AlignRight
                padding: 0
                Layout.topMargin: 0
                onCheckedChanged: {
                    txtPass.enabled = pwdEditPass.checked
                    if(pwdEditPass.checked){
                        if(txtPass.text === "***")
                            txtPass.text = "";
                    }else{
                        if(txtPass.text === "")
                            txtPass.text = "***"
                        pwdViev.checked = false
                    }

                }
            }
        }
        Text{
            font.pixelSize: fontPixelSize
            text: "Идентификатор:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
        }

        TextField{
            id: txtDeviceId
            font.pixelSize: fontPixelSize
            text: wsSettings.deviceId
            color: "grey"
            Material.accent: Material.Blue
            Layout.fillWidth: true
            enabled: false
            wrapMode: Text.WordWrap

        }
        Text{
            font.pixelSize: fontPixelSize
            text: "Продукт:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
        }
        TextField{
            id: txtProduct
            font.pixelSize: fontPixelSize
            text: wsSettings.product
            color: "grey"
            Material.accent: Material.Blue
            Layout.fillWidth: true
            enabled: false
            wrapMode: Text.WordWrap

        }
        Text{
            font.pixelSize: fontPixelSize
            text: "Http сервис 1C:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
        }
        TextField{
            id: txtHttpService
            font.pixelSize: fontPixelSize
            text: wsSettings.httpService
            //color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
            Material.accent: Material.Blue
            Layout.fillWidth: true
            color: "grey"
            enabled: false //берем с сервера после подключения
            wrapMode: Text.WordWrap

            onEditingFinished: {
                wsSettings.httpService = txtHttpService.text
            }
            onAccepted: {
                txtHttpService.focus = false
            }
        }
        Label{
            text: ""
        }

        CheckBox{
            id: showImage
            checked: wsSettings.showImage;
            text: "Показывать картинку"
            font.pixelSize: fontPixelSize
            Layout.fillWidth: true
            Material.accent: Material.Blue
            padding: 0
            contentItem: Label {
                text: showImage.text
                font: showImage.font
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: showImage.indicator.width + showImage.spacing
                wrapMode: Label.Wrap
                topPadding: 0
                bottomPadding: 0
            }
            onCheckedChanged: {
                wsSettings.showImage = showImage.checked
                wsSettings.save()
            }
        }
        Label{
            text: ""
        }
        CheckBox{
            id: isKeyboardMode
            checked: wsSettings.keyboardInputMode;
            text: "Режим клавиатурного ввода"
            font.pixelSize: fontPixelSize
            Layout.fillWidth: true
            Material.accent: Material.Blue
            padding: 0
            contentItem: Label {
                text: isKeyboardMode.text
                font: isKeyboardMode.font
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: isKeyboardMode.indicator.width + isKeyboardMode.spacing
                wrapMode: Label.Wrap
                topPadding: 0
                bottomPadding: 0
            }
            onCheckedChanged: {
                wsSettings.keyboardInputMode = isKeyboardMode.checked
                wsSettings.save()
            }
        }
        Label{
            text: ""            
        }
        CheckBox{
            id: isPriceCheckerMode
            checked: wsSettings.priceCheckerMode;
            text: "Режим прайс-чекера"
            font.pixelSize: fontPixelSize
            Layout.fillWidth: true
            Material.accent: Material.Blue
            padding: 0
            contentItem: Label {
                text: isPriceCheckerMode.text
                font: isPriceCheckerMode.font
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: isPriceCheckerMode.indicator.width + isPriceCheckerMode.spacing
                wrapMode: Label.Wrap
                topPadding: 0
                bottomPadding: 0
            }
            onCheckedChanged: {
                wsSettings.priceCheckerMode = isPriceCheckerMode.checked
                wsSettings.save()
            }

            //enabled: false //в этом проекте всегда ложь
        }
        Label{
            text: ""
        }
        CheckBox{
            id: isAutoConnect
            checked: wsSettings.autoConnect;
            text: "Авто подключение при разрыве соединения"
            font.pixelSize: fontPixelSize
            Layout.fillWidth: true
            Material.accent: Material.Blue
            padding: 0
            contentItem: Label {
                text: isAutoConnect.text
                font: isAutoConnect.font
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: isAutoConnect.indicator.width + isAutoConnect.spacing
                wrapMode: Label.Wrap
                topPadding: 0
                bottomPadding: 0
            }
            onCheckedChanged: {
                wsSettings.autoConnect = isAutoConnect.checked
                wsSettings.save()
            }
        }
        Label{
            text: ""
            visible: screenWidth > 1000 ? true : false
        }
        RowLayout{
            Layout.alignment: Qt.AlignRight
            Layout.columnSpan: screenWidth > 1000 ? 1 : 2
            Button{
                id: btnConnected
                text: screenWidth > 1000 ? (wsClient.isStarted() ? "Отключится" : "Подключится") : ""
                icon.source:  screenWidth > 1000 ? (wsClient.isStarted() ? "qrc:/img/lan_check_online.png" : "qrc:/img/lan_check_offline.png") : ""
                onClicked: {
                    if(!wsClient.isStarted())
                        popupSettingsDialog.webSocketConnect();
                    else
                       wsClient.close();
                }
            }

            Button{
                id: btnRegistyDevice
                text: screenWidth > 1000 ? "Зарегистрировать" : ""
                icon.source:  screenWidth > 1000 ? "" : "qrc:/img/to_data.png"
                onClicked: {
                    if(!wsClient.isStarted())
                        popupMessage.showError("Ошибка", "Клиент не подключен!")
                    else{
                        wsClient.registerDevice();
                    }

                }
            }

            Button{
                id: btnDislogClose
                text: "Закрыть"

                onClicked: {
                    popupSettingsDialog.visible = false
                }
            }
        }
    }

}
