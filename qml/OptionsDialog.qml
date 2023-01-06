import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Popup {
    id: popupSettingsDialog
    anchors.centerIn: parent
    width: parent.width / 2
    //height: 200
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    property string theme: "Dark"

    function updateHttpServiceConfiguration(hsHost, hsUser, hsPwd){
        wsSettings.httpService = hsHost;
        txtHttpService.text = hsHost;
    }

    signal webSocketConnect();

    onVisibleChanged: {
        wsSettings.save();
    }

    GridLayout{
        id: grid
        columns: 2
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.margins: 10
        columnSpacing: 20

        Text{
            font.pixelSize: 20
            text: "Сервер:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
        }

        TextField{
            id: txtServer
            font.pixelSize: 20
            text: wsSettings.url() //"ws://<domainname>"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
            Material.accent: Material.Blue
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            //enabled: !wsClient.isStarted();
            onEditingFinished: {
                wsSettings.setUrl(txtServer.text);
            }

            onAccepted: {
                txtServer.focus = false
            }
        }

        Text{
            font.pixelSize: 20
            text: "Пользователь:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
        }

        TextField{
            id: txtUser
            font.pixelSize: 20
            text: wsSettings.userName
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
            Material.accent: Material.Blue
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            //enabled: !wsClient.isStarted();
            onEditingFinished: {
                wsSettings.userName = txtUser.text;
                wsSettings.httpUser = txtUser.text;
            }

            onAccepted: {
                txtUser.focus = false
            }

        }

        Text{
            font.pixelSize: 20
            text: "Пароль:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
        }

        RowLayout{
            Layout.fillWidth: true
            TextField{
                id: txtPass
                font.pixelSize: 20
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
                //enabled: !wsClient.isStarted();
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
            font.pixelSize: 20
            text: "Идентификатор:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
        }

        TextField{
            id: txtDeviceId
            font.pixelSize: 20
            text: wsSettings.deviceId
            color: "grey"
            Material.accent: Material.Blue
            Layout.fillWidth: true
            enabled: false
            wrapMode: Text.WordWrap

        }
        Text{
            font.pixelSize: 20
            text: "Продукт:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
        }
        TextField{
            id: txtProduct
            font.pixelSize: 20
            text: wsSettings.product
            color: "grey"
            Material.accent: Material.Blue
            Layout.fillWidth: true
            enabled: false
            wrapMode: Text.WordWrap

        }
        Text{
            font.pixelSize: 20
            text: "Http сервис 1C:"
            color: popupSettingsDialog.theme === "Light" ? "#424242" : "#efebe9"
        }
        TextField{
            id: txtHttpService
            font.pixelSize: 20
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
            id: isQrImage
            checked: wsSettings.isQrImage;
            text: "Показывать картинку штрихода"
            Layout.fillWidth: true
            Material.accent: Material.Blue
            contentItem: Label {
                text: isQrImage.text
                font: isQrImage.font
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: isQrImage.indicator.width + isQrImage.spacing
                wrapMode: Label.Wrap
            }
            onCheckedChanged: {
                wsSettings.isQrImage = isQrImage.checked
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
            Layout.fillWidth: true
            Material.accent: Material.Blue
            contentItem: Label {
                text: isKeyboardMode.text
                font: isKeyboardMode.font
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: isKeyboardMode.indicator.width + isKeyboardMode.spacing
                wrapMode: Label.Wrap
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
            Layout.fillWidth: true
            Material.accent: Material.Blue
            contentItem: Label {
                text: isPriceCheckerMode.text
                font: isPriceCheckerMode.font
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                leftPadding: isPriceCheckerMode.indicator.width + isPriceCheckerMode.spacing
                wrapMode: Label.Wrap
            }
            onCheckedChanged: {
                wsSettings.priceCheckerMode = isPriceCheckerMode.checked
                wsSettings.save()
            }

            enabled: false //в этом проекте всегда ложь
        }
        Label{
            text: ""
        }
        RowLayout{
            Layout.alignment: Qt.AlignRight
            Button{
                id: btnConnected
                //text: wsClient.isStarted() ? "Отключится" : "Подключится"
                text: "Подключится"

                onClicked: {
                    popupSettingsDialog.webSocketConnect();
                }
            }

            Button{
                id: btnRegistyDevice
                text: "Зарегистрировать"

                onClicked: {
                    if(!wsClient.isStarted())
                        popupMessage.showError("Ошибка", "Клиент не подключен!")
                    else{
//                        var result = wsClient.registerClientOn1C(wsSettings.httpService, wsSettings.userName, wsClient.crypt(wsSettings.httpPwd, "my_key"))
//                        wsClient.registerClientToDatabase();
                        wsClient.registerDevice();
                    }

                }
            }

            Button{
                id: btnDislogClose
                text: "Закрыть"

                onClicked: {
                    popupSettingsDialog.visible = false
    //                if(!wsClient.isStarted())
    //                    popupMessage.showError("Ошибка", "Клиент не подключен!")
    //                else{
    //                    var result = wsClient.registerClientOn1C(wsSettings.httpService, wsSettings.userName, wsClient.crypt(wsSettings.httpPwd, "my_key"))
    //                    wsClient.registerClientToDatabase();
    //                }
                }
            }
        }
    }

}
