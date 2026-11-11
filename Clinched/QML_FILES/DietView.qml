import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // Temporary data, will come from presets.json later
    ListModel {
        id: foodsModel
        ListElement { name: "Chicken Breast"; calsPer100g: 165 }
        ListElement { name: "White Rice";     calsPer100g: 130 }
        ListElement { name: "Rolled Oats";    calsPer100g: 389 }
        ListElement { name: "Whole Eggs";     calsPer100g: 143 }
        ListElement { name: "Peanut Butter";  calsPer100g: 588 }
    }

    // Meals the user adds
    ListModel { id: mealsModel }

    property real gramsValue: parseFloat(gramsField.text) || 0
    property real previewCalories: gramsValue * foodsModel.get(foodCombo.currentIndex).calsPer100g / 100
    property real totalCalories: 0

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Label {
            text: "Insert a meal"
            font.pixelSize: 22
            font.bold: true
        }

        RowLayout {
            spacing: 12
            Layout.fillWidth: true

            ComboBox {
                id: foodCombo
                model: foodsModel
                textRole: "name"
                Layout.fillWidth: true
            }

            TextField {
                id: gramsField
                placeholderText: "Grams"
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 1; top: 5000 }
                Layout.preferredWidth: 100
            }
        }

        Label {
            text: "Calories: " + Math.round(previewCalories) + " kcal"
            font.pixelSize: 18
        }

        Button {
            text: "Add Meal"
            highlighted: true          // accent-colored button
            enabled: gramsValue > 0

            onClicked: {
                var cals = Math.round(previewCalories)
                mealsModel.append({
                    foodName: foodCombo.currentText,
                    grams: gramsValue,
                    calories: cals
                })
                totalCalories += cals
                gramsField.clear()
            }
        }

        Label {
            text: "Total today: " + totalCalories + " kcal"
            font.bold: true
            font.pixelSize: 16
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: mealsModel

            delegate: ItemDelegate {
                width: ListView.view.width
                text: foodName + ": " + grams + " g - " + calories + " kcal"
            }
        }
    }
}